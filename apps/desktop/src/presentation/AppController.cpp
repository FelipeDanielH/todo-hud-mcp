#include <QVector>
#include "AppController.h"
#include "application/TaskService.h"

AppController::AppController(TaskService& taskService,
                             TaskListModel& taskListModel,
                             FocusTimerController& focusTimer,
                             bool online,
                             QObject* parent)
    : QObject(parent)
    , m_taskService(taskService)
    , m_taskListModel(taskListModel)
    , m_focusTimer(focusTimer)
    , m_online(online)
{
    auto active = m_taskService.activeTasks();
    if (!active.isEmpty()) {
        m_currentTaskId = active.first().id;
    } else {
        auto all = m_taskService.allTasks();
        if (!all.isEmpty())
            m_currentTaskId = all.first().id;
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
    return m_online;
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

    auto active = m_taskService.activeTasks();
    m_currentTaskId = active.isEmpty() ? -1 : active.first().id;
    emit currentTaskChanged();
}

void AppController::reopenTask(int id)
{
    m_taskService.reopenTask(id);
    m_taskListModel.refresh();
    emit dataChanged();
}

void AppController::completeTask(int id)
{
    m_taskService.completeTask(id);
    m_taskListModel.refresh();
    emit dataChanged();
}

void AppController::createTask(const QString& title)
{
    if (title.trimmed().isEmpty())
        return;
    m_taskService.createTask(title.trimmed());
    m_taskListModel.refresh();
    emit dataChanged();
}

void AppController::archiveCompleted()
{
    m_taskService.archiveCompleted();
    m_taskListModel.refresh();
    emit dataChanged();
}
