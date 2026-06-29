#include "WebSocketClient.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

WebSocketClient::WebSocketClient(const QUrl& wsUrl, QObject* parent)
    : TaskEventsClient(parent)
    , m_socket(QString(), QWebSocketProtocol::VersionLatest, this)
    , m_url(wsUrl)
{
    m_reconnectTimer.setSingleShot(true);
    connect(&m_reconnectTimer, &QTimer::timeout, this, &WebSocketClient::attemptReconnect);

    m_debounceTimer.setSingleShot(true);
    m_debounceTimer.setInterval(DEBOUNCE_MS);
    connect(&m_debounceTimer, &QTimer::timeout, this, &WebSocketClient::onDebounceTimeout);

    connect(&m_socket, &QWebSocket::connected, this, &WebSocketClient::onConnected);
    connect(&m_socket, &QWebSocket::disconnected, this, &WebSocketClient::onDisconnected);
    connect(&m_socket, &QWebSocket::textMessageReceived, this, &WebSocketClient::onTextMessageReceived);
    connect(&m_socket, &QWebSocket::errorOccurred, this, &WebSocketClient::onError);
}

WebSocketClient::~WebSocketClient()
{
    disconnectFromServer();
}

TaskEventsClient::State WebSocketClient::state() const
{
    return m_state;
}

void WebSocketClient::connectToServer()
{
    if (m_state == Connected || m_state == Connecting)
        return;

    m_stopRequested = false;
    m_reconnectTimer.stop();
    setState(Connecting);
    qDebug().noquote() << QStringLiteral("[WebSocketClient] Connecting to %1").arg(m_url.toString());
    m_socket.open(m_url);
}

void WebSocketClient::disconnectFromServer()
{
    m_stopRequested = true;
    m_reconnectTimer.stop();
    m_debounceTimer.stop();

    if (m_socket.state() == QAbstractSocket::UnconnectedState) {
        setState(Disconnected);
        return;
    }

    m_socket.close(QWebSocketProtocol::CloseCodeNormal, QStringLiteral("Client shutting down"));
}

void WebSocketClient::onConnected()
{
    resetReconnectDelay();
    setState(Connected);
    qDebug().noquote() << QStringLiteral("[WebSocketClient] Connected");
}

void WebSocketClient::onDisconnected()
{
    qDebug().noquote() << QStringLiteral("[WebSocketClient] Disconnected from %1").arg(m_url.toString());
    if (m_stopRequested) {
        setState(Disconnected);
        return;
    }
    scheduleReconnect();
}

void WebSocketClient::onTextMessageReceived(const QString& message)
{
    const auto doc = QJsonDocument::fromJson(message.toUtf8());
    if (!doc.isObject())
        return;

    const auto obj = doc.object();
    if (obj.value(QStringLiteral("type")).toString() != QStringLiteral("tasks.changed"))
        return;

    qDebug().noquote() << QStringLiteral("[WebSocketClient] Received tasks.changed event; debouncing refresh");
    m_debounceTimer.start();
}

void WebSocketClient::onError(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error)
    qWarning().noquote() << QStringLiteral("[WebSocketClient] Socket error: %1").arg(m_socket.errorString());
    if (!m_stopRequested && m_socket.state() == QAbstractSocket::UnconnectedState)
        scheduleReconnect();
}

void WebSocketClient::attemptReconnect()
{
    if (m_stopRequested || m_state == Connected)
        return;

    setState(Reconnecting);
    qDebug().noquote() << QStringLiteral("[WebSocketClient] Reconnecting to %1").arg(m_url.toString());
    m_socket.open(m_url);
}

void WebSocketClient::onDebounceTimeout()
{
    emit tasksChanged();
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
    if (m_stopRequested)
        return;

    setState(Reconnecting);
    if (!m_reconnectTimer.isActive()) {
        m_reconnectTimer.start(m_reconnectDelay);
        m_reconnectDelay = qMin(m_reconnectDelay * 2, MAX_RECONNECT_DELAY_MS);
    }
}

void WebSocketClient::resetReconnectDelay()
{
    m_reconnectDelay = INITIAL_RECONNECT_DELAY_MS;
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
