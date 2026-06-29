#ifndef FOCUSHUD_TASK_CONTROLLER_H
#define FOCUSHUD_TASK_CONTROLLER_H

#include <QObject>
#include <QTimer>
#include "TaskModel.h"

class TaskController : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString formattedTime READ formattedTime NOTIFY formattedTimeChanged)
    Q_PROPERTY(QString currentTaskTitle READ currentTaskTitle NOTIFY currentTaskChanged)
    Q_PROPERTY(bool isRunning READ isRunning NOTIFY isRunningChanged)
    Q_PROPERTY(bool hasActiveTask READ hasActiveTask NOTIFY currentTaskChanged)

public:
    explicit TaskController(TaskModel* model, QObject* parent = nullptr);

    QString formattedTime() const;
    QString currentTaskTitle() const;
    bool isRunning() const;
    bool hasActiveTask() const;
    TaskModel* taskModel() const;

    Q_INVOKABLE void startFocus();
    Q_INVOKABLE void stopFocus();
    Q_INVOKABLE void completeCurrentTask();
    Q_INVOKABLE void reopenTask(int index);
    Q_INVOKABLE void selectTask(int index);

signals:
    void formattedTimeChanged();
    void currentTaskChanged();
    void isRunningChanged();

private:
    void updateTimer();
    static QString formatTime(int totalSeconds);

    TaskModel* m_model;
    QTimer* m_timer;
    int m_remainingSeconds = 0;
    int m_currentTaskIndex = -1;
    static constexpr int FOCUS_DURATION = 25 * 60;
};

#endif
