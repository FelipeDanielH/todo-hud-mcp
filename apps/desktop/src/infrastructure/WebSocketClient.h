#pragma once
#include <QAbstractSocket>
#include <QTimer>
#include <QUrl>
#include <QtWebSockets/QWebSocket>
#include <QtWebSockets/QWebSocketProtocol>
#include "application/TaskEventsClient.h"

class WebSocketClient : public TaskEventsClient {
    Q_OBJECT

public:
    explicit WebSocketClient(const QUrl& wsUrl, QObject* parent = nullptr);
    ~WebSocketClient() override;

    State state() const override;
    void connectToServer() override;
    void disconnectFromServer() override;

private slots:
    void onConnected();
    void onDisconnected();
    void onTextMessageReceived(const QString& message);
    void onError(QAbstractSocket::SocketError error);
    void attemptReconnect();
    void onDebounceTimeout();

private:
    void setState(State newState);
    void scheduleReconnect();
    void resetReconnectDelay();
    QString stateToString(State s) const;

    QWebSocket m_socket;
    QUrl m_url;
    State m_state = Disconnected;
    bool m_stopRequested = false;
    int m_reconnectDelay = INITIAL_RECONNECT_DELAY_MS;
    QTimer m_reconnectTimer;
    QTimer m_debounceTimer;

    static constexpr int INITIAL_RECONNECT_DELAY_MS = 1000;
    static constexpr int MAX_RECONNECT_DELAY_MS = 10000;
    static constexpr int DEBOUNCE_MS = 300;
};
