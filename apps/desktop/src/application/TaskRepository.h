#pragma once
#include <QVector>
#include <QStringList>
#include "domain/Task.h"

struct ArchiveTasksResult {
    int archived = 0;
    QString archivedAt;
    bool ok = true;
    QString error;
};

struct BatchCreateResult {
    QString phaseId;
    QString phaseName;
    QVector<Task> tasks;
    bool ok = true;
    QString error;
};

class TaskRepository {
public:
    virtual ~TaskRepository() = default;
    virtual QVector<Task> all() const = 0;
    virtual Task getById(int id) const = 0;
    virtual void update(const Task& task) = 0;
    virtual void save(const Task& task) = 0;
    virtual QVector<Task> archived() const = 0;

    virtual ArchiveTasksResult archiveCompletedTasks(const QString& phaseId = {}) = 0;
    virtual BatchCreateResult createBatch(const QString& phaseName, const QStringList& titles) = 0;
};
