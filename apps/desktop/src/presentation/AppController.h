#pragma once
#include <QObject>
#include "TaskListModel.h"
#include "FocusTimerController.h"

class TaskService;

class AppController : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString currentTaskTitle READ currentTaskTitle NOTIFY currentTaskChanged)
    Q_PROPERTY(bool hasActiveTask READ hasActiveTask NOTIFY currentTaskChanged)
    Q_PROPERTY(TaskListModel* taskListModel READ taskListModel CONSTANT)
    Q_PROPERTY(FocusTimerController* focusTimer READ focusTimer CONSTANT)

public:
    explicit AppController(TaskService* taskService,
                           TaskListModel* taskListModel,
                           FocusTimerController* focusTimer,
                           QObject* parent = nullptr);

    QString currentTaskTitle() const;
    bool hasActiveTask() const;
    TaskListModel* taskListModel() const;
    FocusTimerController* focusTimer() const;

    Q_INVOKABLE void selectTask(int id);
    Q_INVOKABLE void completeCurrentTask();
    Q_INVOKABLE void reopenTask(int id);

signals:
    void currentTaskChanged();

private:
    TaskService* m_taskService;
    TaskListModel* m_taskListModel;
    FocusTimerController* m_focusTimer;
    int m_currentTaskId = -1;
};
