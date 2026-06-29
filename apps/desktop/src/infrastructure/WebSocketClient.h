#pragma once
#include <QObject>
#include <QTcpSocket>
#include <QTimer>
#include <QUrl>
#include <QByteArray>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

class WebSocketClient : public QObject {
    Q_OBJECT
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
public:
    enum State {
        Disconnected = 0,
        Connecting,
        Connected,
        Reconnecting,
    };
    Q_ENUM(State)

    explicit WebSocketClient(const QUrl& wsUrl, QObject* parent = nullptr);
    ~WebSocketClient() override;

    State state() const;
    void connectToServer();
    void disconnectFromServer();

signals:
    void stateChanged();
    void tasksChanged();

private slots:
    void onConnected();
    void onDisconnected();
    void onReadyRead();
    void onError(QAbstractSocket::SocketError error);
    void attemptReconnect();
    void onDebounceTimeout();
    void onPingTimeout();

private:
    void setState(State newState);
    void scheduleReconnect();
    void sendHttpUpgrade();
    bool processHandshake();
    void processFrames();
    void sendPong(const QByteArray& payload);
    void sendClose();
    QString stateToString(State s) const;

    QTcpSocket m_socket;
    QUrl m_url;
    State m_state = Disconnected;
    int m_reconnectDelay = 1000;
    QTimer m_reconnectTimer;
    QTimer m_debounceTimer;
    QTimer m_pingTimer;

    QByteArray m_buffer;
    bool m_handshakeDone = false;
    QString m_expectedAccept;

    static constexpr int MAX_RECONNECT_DELAY_MS = 10000;
    static constexpr int DEBOUNCE_MS = 300;
    static constexpr int PING_TIMEOUT_MS = 30000;
};
