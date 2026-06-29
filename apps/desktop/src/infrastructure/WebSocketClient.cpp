#include "WebSocketClient.h"
#include <QCryptographicHash>
#include <QRegularExpression>

// --- WebSocket protocol helpers (RFC 6455) ---

static QString computeAcceptKey(const QString& key)
{
    const QString magic = QStringLiteral("258EAFA5-E914-47DA-95CA-C5AB0DC85B11");
    QByteArray concat = (key + magic).toUtf8();
    QByteArray hash = QCryptographicHash::hash(concat, QCryptographicHash::Sha1);
    return QString::fromLatin1(hash.toBase64());
}

static quint16 readBe16(const QByteArray& data, int pos)
{
    return static_cast<quint16>(
        (static_cast<quint8>(data[pos]) << 8) |
        static_cast<quint8>(data[pos + 1]));
}

// -------------------------------------------------

WebSocketClient::WebSocketClient(const QUrl& wsUrl, QObject* parent)
    : QObject(parent)
    , m_url(wsUrl)
{
    m_reconnectTimer.setSingleShot(true);
    connect(&m_reconnectTimer, &QTimer::timeout, this, &WebSocketClient::attemptReconnect);

    m_debounceTimer.setSingleShot(true);
    m_debounceTimer.setInterval(DEBOUNCE_MS);
    connect(&m_debounceTimer, &QTimer::timeout, this, &WebSocketClient::onDebounceTimeout);

    m_pingTimer.setSingleShot(true);
    m_pingTimer.setInterval(PING_TIMEOUT_MS);
    connect(&m_pingTimer, &QTimer::timeout, this, [this]() {
        qWarning().noquote() << QStringLiteral("[WebSocketClient] Ping timeout — reconnecting");
        m_socket.abort();
        scheduleReconnect();
    });

    connect(&m_socket, &QTcpSocket::connected, this, &WebSocketClient::onConnected);
    connect(&m_socket, &QTcpSocket::disconnected, this, &WebSocketClient::onDisconnected);
    connect(&m_socket, &QTcpSocket::readyRead, this, &WebSocketClient::onReadyRead);
    connect(&m_socket, &QTcpSocket::errorOccurred, this, &WebSocketClient::onError);
}

WebSocketClient::~WebSocketClient()
{
    m_reconnectTimer.stop();
    m_debounceTimer.stop();
    m_pingTimer.stop();
    if (m_socket.state() != QAbstractSocket::UnconnectedState) {
        sendClose();
        m_socket.waitForDisconnected(1000);
        m_socket.abort();
    }
}

WebSocketClient::State WebSocketClient::state() const
{
    return m_state;
}

void WebSocketClient::connectToServer()
{
    if (m_state == Connected || m_state == Connecting)
        return;

    if (m_url.scheme() == QStringLiteral("wss")) {
        qWarning().noquote() << QStringLiteral("[WebSocketClient] wss:// scheme requires SSL, which is not available in this build");
        qWarning().noquote() << QStringLiteral("[WebSocketClient] Use ws:// for local connections or deploy with a TLS-terminating proxy");
        // Fall through: the TCP connection attempt will fail and reconnect logic will handle it
    }

    setState(Connecting);
    qDebug().noquote() << QStringLiteral("[WebSocketClient] Connecting to %1").arg(m_url.toString());
    m_socket.connectToHost(m_url.host(), m_url.port(m_url.scheme() == QStringLiteral("wss") ? 443 : 80));
}

void WebSocketClient::disconnectFromServer()
{
    m_reconnectTimer.stop();
    if (m_socket.state() != QAbstractSocket::UnconnectedState) {
        sendClose();
        m_socket.abort();
    }
    setState(Disconnected);
}

void WebSocketClient::onConnected()
{
    m_handshakeDone = false;
    m_buffer.clear();
    sendHttpUpgrade();
}

void WebSocketClient::onDisconnected()
{
    m_handshakeDone = false;
    m_pingTimer.stop();
    qDebug().noquote() << QStringLiteral("[WebSocketClient] Disconnected from %1").arg(m_url.toString());
    if (m_state == Connected || m_state == Reconnecting) {
        scheduleReconnect();
    } else {
        setState(Disconnected);
    }
}

void WebSocketClient::onReadyRead()
{
    m_buffer.append(m_socket.readAll());

    if (!m_handshakeDone) {
        if (processHandshake()) {
            m_handshakeDone = true;
            m_reconnectDelay = 1000;
            setState(Connected);
            m_pingTimer.start();
            qDebug().noquote() << QStringLiteral("[WebSocketClient] Connected to %1").arg(m_url.toString());
        }
        return;
    }

    processFrames();
}

void WebSocketClient::onError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error)
    qWarning().noquote() << QStringLiteral("[WebSocketClient] Socket error: %1").arg(m_socket.errorString());
    if (m_state == Connecting || m_state == Connected) {
        scheduleReconnect();
    }
}

void WebSocketClient::attemptReconnect()
{
    if (m_state == Connected)
        return;

    setState(Reconnecting);
    qDebug().noquote() << QStringLiteral("[WebSocketClient] Reconnecting to %1 (delay=%2ms)")
        .arg(m_url.toString())
        .arg(m_reconnectDelay);
    m_socket.connectToHost(m_url.host(), m_url.port(m_url.scheme() == QStringLiteral("wss") ? 443 : 80));
}

void WebSocketClient::onDebounceTimeout()
{
    qDebug().noquote() << QStringLiteral("[WebSocketClient] Debounce expired — emitting tasksChanged");
    emit tasksChanged();
}

void WebSocketClient::onPingTimeout()
{
    qWarning().noquote() << QStringLiteral("[WebSocketClient] No data received — reconnecting");
    m_socket.abort();
    scheduleReconnect();
}

void WebSocketClient::setState(State newState)
{
    if (m_state == newState)
        return;
    m_state = newState;
    qDebug().noquote() << QStringLiteral("[WebSocketClient] State: %1").arg(stateToString(m_state));
    emit stateChanged();
}

void WebSocketClient::scheduleReconnect()
{
    if (m_state == Disconnected || m_state == Connecting)
        setState(Reconnecting);
    m_reconnectTimer.start(m_reconnectDelay);
    m_reconnectDelay = qMin(m_reconnectDelay * 2, MAX_RECONNECT_DELAY_MS);
}

void WebSocketClient::sendHttpUpgrade()
{
    const QString key = QStringLiteral("dGhlIHNhbXBsZSBub25jZQ==");
    m_expectedAccept = computeAcceptKey(key);

    QString path = m_url.path();
    if (path.isEmpty()) path = QStringLiteral("/");
    if (m_url.hasQuery()) path += QStringLiteral("?") + m_url.query();

    QByteArray request;
    request.append(QStringLiteral("GET %1 HTTP/1.1\r\n").arg(path).toUtf8());
    request.append(QStringLiteral("Host: %1").arg(m_url.host()).toUtf8());
    if (m_url.port() > 0 && m_url.port() != (m_url.scheme() == QStringLiteral("wss") ? 443 : 80))
        request.append(QStringLiteral(":%1").arg(m_url.port()).toUtf8());
    request.append(QStringLiteral("\r\n").toUtf8());
    request.append(QStringLiteral("Upgrade: websocket\r\n").toUtf8());
    request.append(QStringLiteral("Connection: Upgrade\r\n").toUtf8());
    request.append(QStringLiteral("Sec-WebSocket-Key: %1\r\n").arg(key).toUtf8());
    request.append(QStringLiteral("Sec-WebSocket-Version: 13\r\n").toUtf8());
    request.append(QStringLiteral("\r\n").toUtf8());

    m_socket.write(request);
    m_socket.flush();
}

bool WebSocketClient::processHandshake()
{
    int headerEnd = m_buffer.indexOf(QByteArrayLiteral("\r\n\r\n"));
    if (headerEnd == -1)
        return false;

    QByteArray header = m_buffer.left(headerEnd);
    m_buffer.remove(0, headerEnd + 4);

    if (!header.startsWith("HTTP/1.1 101")) {
        qWarning().noquote() << QStringLiteral("[WebSocketClient] Handshake failed: expected HTTP/1.1 101, got:");
        qWarning().noquote() << QString::fromUtf8(header.left(200));
        m_socket.abort();
        return false;
    }

    QRegularExpression acceptRe(QStringLiteral("Sec-WebSocket-Accept:\\s*(\\S+)"));
    auto match = acceptRe.match(QString::fromUtf8(header));
    if (!match.hasMatch()) {
        qWarning().noquote() << QStringLiteral("[WebSocketClient] Handshake failed: missing Sec-WebSocket-Accept");
        m_socket.abort();
        return false;
    }

    if (match.captured(1) != m_expectedAccept) {
        qWarning().noquote() << QStringLiteral("[WebSocketClient] Handshake failed: Sec-WebSocket-Accept mismatch");
        m_socket.abort();
        return false;
    }

    return true;
}

void WebSocketClient::processFrames()
{
    while (m_buffer.size() >= 2) {
        quint8 b0 = static_cast<quint8>(m_buffer[0]);
        quint8 b1 = static_cast<quint8>(m_buffer[1]);
        quint8 opcode = b0 & 0x0F;
        bool masked = (b1 & 0x80) != 0;
        quint64 payloadLen = b1 & 0x7F;
        int offset = 2;

        if (payloadLen == 126) {
            if (m_buffer.size() < 4) return;
            payloadLen = readBe16(m_buffer, offset);
            offset += 2;
        } else if (payloadLen == 127) {
            if (m_buffer.size() < 10) return;
            payloadLen = 0;
            for (int i = 0; i < 8; ++i)
                payloadLen = (payloadLen << 8) | static_cast<quint8>(m_buffer[offset + i]);
            offset += 8;
        }

        quint8 maskKey[4] = {0};
        if (masked) {
            if (m_buffer.size() < offset + 4) return;
            for (int i = 0; i < 4; ++i)
                maskKey[i] = static_cast<quint8>(m_buffer[offset + i]);
            offset += 4;
        }

        if (static_cast<quint64>(m_buffer.size()) < static_cast<quint64>(offset) + payloadLen)
            return;

        QByteArray frameData = m_buffer.mid(offset, static_cast<int>(payloadLen));
        m_buffer.remove(0, offset + static_cast<int>(payloadLen));

        if (masked) {
            for (int i = 0; i < frameData.size(); ++i)
                frameData[i] = frameData[i] ^ maskKey[i % 4];
        }

        switch (opcode) {
        case 0x1: {
            m_pingTimer.start();
            QString text = QString::fromUtf8(frameData);
            QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8());
            if (doc.isObject()) {
                QJsonObject obj = doc.object();
                if (obj.value(QStringLiteral("type")).toString() == QStringLiteral("tasks.changed")) {
                    qDebug().noquote() << QStringLiteral("[WebSocketClient] Received tasks.changed event — debouncing refresh");
                    m_debounceTimer.start();
                }
            }
            break;
        }
        case 0x8:
            qDebug().noquote() << QStringLiteral("[WebSocketClient] Received close frame");
            m_socket.disconnectFromHost();
            return;
        case 0x9:
            sendPong(frameData);
            break;
        case 0xA:
            break;
        }
    }
}

void WebSocketClient::sendPong(const QByteArray& payload)
{
    QByteArray frame;
    frame.append(static_cast<char>(0x8A));
    if (payload.size() < 126) {
        frame.append(static_cast<char>(payload.size()));
    } else {
        frame.append(static_cast<char>(126));
        frame.append(static_cast<char>((payload.size() >> 8) & 0xFF));
        frame.append(static_cast<char>(payload.size() & 0xFF));
    }
    frame.append(payload);
    m_socket.write(frame);
    m_socket.flush();
}

void WebSocketClient::sendClose()
{
    QByteArray frame;
    frame.append(static_cast<char>(0x88));
    frame.append(static_cast<char>(0x00));
    m_socket.write(frame);
    m_socket.flush();
}

QString WebSocketClient::stateToString(State s) const
{
    switch (s) {
    case Disconnected: return QStringLiteral("Disconnected");
    case Connecting:   return QStringLiteral("Connecting");
    case Connected:    return QStringLiteral("Connected");
    case Reconnecting: return QStringLiteral("Reconnecting");
    }
    return QStringLiteral("Unknown");
}
