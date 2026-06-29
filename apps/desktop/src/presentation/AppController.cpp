#include "AppController.h"
#include "application/TaskService.h"

AppController::AppController(TaskService& taskService,
                             TaskListModel& taskListModel,
                             FocusTimerController& focusTimer,
                             QObject* parent)
    : QObject(parent)
    , m_taskService(taskService)
    , m_taskListModel(taskListModel)
    , m_focusTimer(focusTimer)
{
    if (m_taskListModel.rowCount({}) > 0) {
        auto tasks = m_taskService.allTasks();
        for (const auto& t : tasks) {
            if (!t.completed) {
                m_currentTaskId = t.id;
                break;
            }
        }
    }
    if (m_currentTaskId < 0 && m_taskListModel.rowCount({}) > 0)
        m_currentTaskId = m_taskService.allTasks().first().id;
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

void AppController::selectTask(int id)
{
    if (id < 0)
        return;

    auto tasks = m_taskService.allTasks();
    bool found = false;
    for (const auto& t : tasks) {
        if (t.id == id) {
            found = true;
            break;
        }
    }
    if (!found)
        return;

    if (id == m_currentTaskId)
        return;

    m_currentTaskId = id;
    emit currentTaskChanged();
}

void AppController::completeCurrentTask()
{
    if (m_currentTaskId < 0)
        return;

    m_taskService.completeTask(m_currentTaskId);
    m_taskListModel.refresh();
    m_focusTimer.reset();

    auto tasks = m_taskService.allTasks();
    m_currentTaskId = -1;
    for (const auto& t : tasks) {
        if (!t.completed) {
            m_currentTaskId = t.id;
            break;
        }
    }
    emit currentTaskChanged();
}

void AppController::reopenTask(int id)
{
    m_taskService.reopenTask(id);
    m_taskListModel.refresh();
}
