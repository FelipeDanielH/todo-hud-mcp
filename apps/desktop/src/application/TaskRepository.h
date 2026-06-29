#pragma once
#include <QVector>
#include "domain/Task.h"

class TaskRepository {
public:
    virtual ~TaskRepository() = default;
    virtual QVector<Task> all() const = 0;
    virtual Task getById(int id) const = 0;
    virtual void update(const Task& task) = 0;
};
