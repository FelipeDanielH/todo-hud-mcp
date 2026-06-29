#include <QVector>
#include <QDebug>
#include "AppController.h"
#include "application/TaskService.h"
#include "application/TaskEventsClient.h"

AppController::AppController(TaskService& taskService,
                             TaskListModel& taskListModel,
                             FocusTimerController& focusTimer,
                             bool online,
                             TaskEventsClient* eventsClient,
                             QObject* parent)
    : QObject(parent)
    , m_taskService(taskService)
    , m_taskListModel(taskListModel)
    , m_focusTimer(focusTimer)
    , m_eventsClient(eventsClient)
    , m_online(online)
{
    syncCurrentTaskSelection();

    if (m_eventsClient) {
        connect(m_eventsClient, &TaskEventsClient::tasksChanged, this, &AppController::onWsTasksChanged);
        connect(m_eventsClient, &TaskEventsClient::stateChanged, this, &AppController::wsStateChanged);
        m_eventsClient->connectToServer();
    }
}

QString AppController::currentTaskTitle() const
{
    if (m_currentTaskId < 0)
        return {};
    return m_taskService.getTask(m_currentTaskId).title;
}

bool AppController::hasActiveTask() const
{
    return m_currentTaskId >= 0;
}

TaskListModel* AppController::taskListModel() const
{
    return &m_taskListModel;
}

FocusTimerController* AppController::focusTimer() const
{
    return &m_focusTimer;
}

int AppController::completedCount() const
{
    return m_taskService.completedCount();
}

bool AppController::isOnline() const
{
    return m_online || (m_eventsClient && m_eventsClient->state() == TaskEventsClient::Connected);
}

int AppController::wsState() const
{
    return m_eventsClient ? static_cast<int>(m_eventsClient->state()) : 0;
}

void AppController::onWsTasksChanged()
{
    qDebug().noquote() << QStringLiteral("[AppController] WebSocket event — refreshing tasks via GET /tasks");
    m_taskListModel.refresh();
    syncCurrentTaskSelection();
    emit dataChanged();
}

void AppController::forceRefresh()
{
    qDebug().noquote() << QStringLiteral("[AppController] Manual refresh triggered");
    m_taskListModel.refresh();
    syncCurrentTaskSelection();
    emit dataChanged();
}

void AppController::selectTask(int id)
{
    if (id < 0)
        return;
    if (id == m_currentTaskId)
        return;

    auto tasks = m_taskService.allTasks();
    for (const auto& t : tasks) {
        if (t.id == id) {
            m_currentTaskId = id;
            emit currentTaskChanged();
            return;
        }
    }
}

void AppController::completeCurrentTask()
{
    if (m_currentTaskId < 0)
        return;

    m_taskService.completeTask(m_currentTaskId);
    m_taskListModel.refresh();
    m_focusTimer.reset();
    emit dataChanged();

    syncCurrentTaskSelection();
}

void AppController::reopenTask(int id)
{
    m_taskService.reopenTask(id);
    m_taskListModel.refresh();
    syncCurrentTaskSelection();
    emit dataChanged();
}

void AppController::completeTask(int id)
{
    m_taskService.completeTask(id);
    m_taskListModel.refresh();
    syncCurrentTaskSelection();
    emit dataChanged();
}

void AppController::createTask(const QString& title)
{
    if (title.trimmed().isEmpty())
        return;
    m_taskService.createTask(title.trimmed());
    m_taskListModel.refresh();
    syncCurrentTaskSelection();
    emit dataChanged();
}

void AppController::archiveCompleted()
{
    m_taskService.archiveCompleted();
    m_taskListModel.refresh();
    syncCurrentTaskSelection();
    emit dataChanged();
}

void AppController::syncCurrentTaskSelection()
{
    const int previousTaskId = m_currentTaskId;
    const auto active = m_taskListModel.activeTasks();
    const auto completed = m_taskListModel.completedTasks();

    for (const auto& task : active) {
        if (task.id == m_currentTaskId) {
            return;
        }
    }

    if (!active.isEmpty()) {
        m_currentTaskId = active.first().id;
    } else if (!completed.isEmpty()) {
        m_currentTaskId = completed.first().id;
    } else {
        m_currentTaskId = -1;
    }

    if (previousTaskId != m_currentTaskId)
        emit currentTaskChanged();
}
