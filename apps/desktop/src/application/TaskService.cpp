#include <QDateTime>
#include "TaskService.h"

TaskService::TaskService(TaskRepository& repository)
    : m_repository(repository)
{
    for (const auto& t : m_repository.all()) {
        if (t.id >= m_nextId)
            m_nextId = t.id + 1;
    }
}

QVector<Task> TaskService::allTasks() const
{
    return m_repository.all();
}

QVector<Task> TaskService::activeTasks() const
{
    QVector<Task> result;
    for (const auto& t : m_repository.all()) {
        if (t.status == QStringLiteral("pending"))
            result.append(t);
    }
    return result;
}

QVector<Task> TaskService::completedTasks() const
{
    QVector<Task> result;
    for (const auto& t : m_repository.all()) {
        if (t.status == QStringLiteral("completed"))
            result.append(t);
    }
    return result;
}

QVector<Task> TaskService::archivedTasks() const
{
    return m_repository.archived();
}

Task TaskService::getTask(int id) const
{
    return m_repository.getById(id);
}

void TaskService::completeTask(int id)
{
    Task task = m_repository.getById(id);
    if (task.id < 0)
        return;

    task.completed = true;
    task.status = QStringLiteral("completed");
    task.completedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
    m_repository.update(task);
}

void TaskService::reopenTask(int id)
{
    Task task = m_repository.getById(id);
    if (task.id < 0)
        return;

    task.completed = false;
    task.status = QStringLiteral("pending");
    task.completedAt.clear();
    m_repository.update(task);
}

void TaskService::createTask(const QString& title, const QString& phaseId, const QString& phaseName)
{
    Task task;
    task.id = m_nextId++;
    task.title = title;
    task.completed = false;
    task.status = QStringLiteral("pending");
    task.phaseId = phaseId;
    task.phaseName = phaseName;
    task.sortOrder = 0;
    m_repository.save(task);
}

ArchiveTasksResult TaskService::archiveCompleted(const QString& phaseId)
{
    return m_repository.archiveCompletedTasks(phaseId);
}

BatchCreateResult TaskService::createBatch(const QString& phaseName, const QStringList& titles)
{
    return m_repository.createBatch(phaseName, titles);
}

int TaskService::completedCount() const
{
    int count = 0;
    for (const auto& t : m_repository.all()) {
        if (t.status == QStringLiteral("completed"))
            ++count;
    }
    return count;
}
