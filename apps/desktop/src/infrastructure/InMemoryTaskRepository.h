#pragma once
#include "application/TaskRepository.h"

class InMemoryTaskRepository : public TaskRepository {
public:
    InMemoryTaskRepository();

    QVector<Task> all() const override;
    Task getById(int id) const override;
    void update(const Task& task) override;
    void save(const Task& task) override;
    QVector<Task> archived() const override;
    ArchiveTasksResult archiveCompletedTasks(const QString& phaseId = {}) override;
    BatchCreateResult createBatch(const QString& phaseName, const QStringList& titles) override;

private:
    QVector<Task> m_tasks;
};
