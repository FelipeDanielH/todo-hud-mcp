#pragma once
#include <QObject>

class TaskEventsClient : public QObject {
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

    explicit TaskEventsClient(QObject* parent = nullptr) : QObject(parent) {}
    ~TaskEventsClient() override = default;

    virtual State state() const = 0;
    virtual void connectToServer() = 0;
    virtual void disconnectFromServer() = 0;

signals:
    void stateChanged();
    void tasksChanged();
};
