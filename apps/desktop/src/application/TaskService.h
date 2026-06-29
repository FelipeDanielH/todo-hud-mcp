#pragma once
#include <QVector>
#include "domain/Task.h"
#include "application/TaskRepository.h"

class TaskService {
public:
    explicit TaskService(TaskRepository& repository);

    QVector<Task> allTasks() const;
    QVector<Task> activeTasks() const;
    QVector<Task> completedTasks() const;
    QVector<Task> archivedTasks() const;
    Task getTask(int id) const;
    void completeTask(int id);
    void reopenTask(int id);
    void createTask(const QString& title, const QString& phaseId = {}, const QString& phaseName = {});
    ArchiveTasksResult archiveCompleted(const QString& phaseId = {});
    BatchCreateResult createBatch(const QString& phaseName, const QStringList& titles);
    int completedCount() const;


private:
    TaskRepository& m_repository;
    int m_nextId = 100;
};
