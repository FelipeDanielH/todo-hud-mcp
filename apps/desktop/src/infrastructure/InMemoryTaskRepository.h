#pragma once
#include "application/TaskRepository.h"

class InMemoryTaskRepository : public TaskRepository {
public:
    InMemoryTaskRepository();

    QVector<Task> all() const override;
    Task getById(int id) const override;
    void update(const Task& task) override;

private:
    QVector<Task> m_tasks;
};
