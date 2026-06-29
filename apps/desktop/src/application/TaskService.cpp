#include "TaskService.h"

TaskService::TaskService(TaskRepository* repository)
    : m_repository(repository)
{
}

QVector<Task> TaskService::allTasks() const
{
    return m_repository->all();
}

Task TaskService::getTask(int id) const
{
    return m_repository->getById(id);
}

void TaskService::completeTask(int id)
{
    Task task = m_repository->getById(id);
    task.completed = true;
    m_repository->update(task);
}

void TaskService::reopenTask(int id)
{
    Task task = m_repository->getById(id);
    task.completed = false;
    m_repository->update(task);
}
