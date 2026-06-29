#pragma once
#include <QObject>
#include <QtQmlIntegration/qqmlintegration.h>
#include "TaskListModel.h"
#include "FocusTimerController.h"

class TaskService;
class TaskEventsClient;

struct TaskListModelQmlForeign {
    Q_GADGET
    QML_FOREIGN(TaskListModel)
    QML_ANONYMOUS
};

struct FocusTimerControllerQmlForeign {
    Q_GADGET
    QML_FOREIGN(FocusTimerController)
    QML_ANONYMOUS
};

class AppController : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(AppController)
    QML_ELEMENT
    QML_UNCREATABLE("AppController is provided by the application")
    Q_PROPERTY(QString currentTaskTitle READ currentTaskTitle NOTIFY currentTaskChanged)
    Q_PROPERTY(bool hasActiveTask READ hasActiveTask NOTIFY currentTaskChanged)
    Q_PROPERTY(TaskListModel* taskListModel READ taskListModel CONSTANT)
    Q_PROPERTY(FocusTimerController* focusTimer READ focusTimer CONSTANT)
    Q_PROPERTY(int completedCount READ completedCount NOTIFY dataChanged)
    Q_PROPERTY(bool online READ isOnline NOTIFY wsStateChanged)
    Q_PROPERTY(int wsState READ wsState NOTIFY wsStateChanged)

public:
    explicit AppController(TaskService& taskService,
                           TaskListModel& taskListModel,
                           FocusTimerController& focusTimer,
                           bool online,
                           TaskEventsClient* eventsClient = nullptr,
                           QObject* parent = nullptr);

    QString currentTaskTitle() const;
    bool hasActiveTask() const;
    TaskListModel* taskListModel() const;
    FocusTimerController* focusTimer() const;
    int completedCount() const;
    bool isOnline() const;
    int wsState() const;

    Q_INVOKABLE void selectTask(int id);
    Q_INVOKABLE void completeCurrentTask();
    Q_INVOKABLE void reopenTask(int id);
    Q_INVOKABLE void completeTask(int id);
    Q_INVOKABLE void createTask(const QString& title);
    Q_INVOKABLE void archiveCompleted();
    Q_INVOKABLE void forceRefresh();

signals:
    void currentTaskChanged();
    void dataChanged();
    void wsStateChanged();

private:
    void onWsTasksChanged();
    void syncCurrentTaskSelection();

    TaskService& m_taskService;
    TaskListModel& m_taskListModel;
    FocusTimerController& m_focusTimer;
    TaskEventsClient* m_eventsClient = nullptr;
    int m_currentTaskId = -1;
    bool m_online = false;
};
