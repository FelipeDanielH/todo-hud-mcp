#pragma once
#include <QObject>
#include <QTimer>

class FocusTimerController : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(FocusTimerController)
    Q_PROPERTY(QString formattedTime READ formattedTime NOTIFY formattedTimeChanged)
    Q_PROPERTY(bool isRunning READ isRunning NOTIFY isRunningChanged)

public:
    explicit FocusTimerController(QObject* parent = nullptr);

    QString formattedTime() const;
    bool isRunning() const;

    Q_INVOKABLE void start();
    Q_INVOKABLE void stop();
    Q_INVOKABLE void reset();

signals:
    void formattedTimeChanged();
    void isRunningChanged();
    void focusCompleted();

private:
    void tick();

    QTimer m_timer;
    int m_remainingSeconds = 0;

    static constexpr int FOCUS_DURATION_SECONDS = 25 * 60;

    static QString formatTime(int totalSeconds);
};
