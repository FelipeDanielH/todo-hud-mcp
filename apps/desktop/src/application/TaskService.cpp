#include <QDateTime>
#include <QJsonObject>
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
    task.completed = true;
    task.status = QStringLiteral("completed");
    task.completedAt = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
    m_repository.update(task);
}

void TaskService::reopenTask(int id)
{
    Task task = m_repository.getById(id);
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

QJsonObject TaskService::archiveCompleted(const QString& phaseId)
{
    QJsonObject result = m_repository.archiveCompletedTasks(phaseId);
    if (result.isEmpty()) {
        const auto now = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
        int count = 0;
        for (const auto& t : m_repository.all()) {
            if (t.status == QStringLiteral("completed")) {
                if (!phaseId.isEmpty() && t.phaseId != phaseId)
                    continue;
                Task archived = t;
                archived.status = QStringLiteral("archived");
                archived.archivedAt = now;
                m_repository.update(archived);
                ++count;
            }
        }
        result[QStringLiteral("archived")] = count;
        result[QStringLiteral("archivedAt")] = now;
    }
    return result;
}

QJsonObject TaskService::createBatch(const QString& phaseName, const QStringList& titles)
{
    QJsonObject result = m_repository.createBatch(phaseName, titles);
    if (result.isEmpty()) {
        const QString phaseId = QStringLiteral("local-") + QString::number(QDateTime::currentMSecsSinceEpoch());
        for (int i = 0; i < titles.size(); ++i) {
            Task task;
            task.id = m_nextId++;
            task.title = titles[i];
            task.completed = false;
            task.status = QStringLiteral("pending");
            task.phaseId = phaseId;
            task.phaseName = phaseName;
            task.sortOrder = i;
            m_repository.save(task);
        }
        result[QStringLiteral("phaseId")] = phaseId;
        result[QStringLiteral("phaseName")] = phaseName;
    }
    return result;
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


