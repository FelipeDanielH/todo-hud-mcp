#pragma once
#include <QVector>
#include <QJsonObject>
#include "domain/Task.h"

class TaskRepository {
public:
    virtual ~TaskRepository() = default;
    virtual QVector<Task> all() const = 0;
    virtual Task getById(int id) const = 0;
    virtual void update(const Task& task) = 0;
    virtual void save(const Task& task) = 0;
    virtual QVector<Task> archived() const = 0;

    virtual QJsonObject archiveCompletedTasks(const QString& /*phaseId*/ = {}) { return {}; }
    virtual QJsonObject createBatch(const QString& /*phaseName*/, const QStringList& /*titles*/) { return {}; }
};
