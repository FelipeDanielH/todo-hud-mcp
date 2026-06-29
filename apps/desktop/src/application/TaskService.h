#pragma once
#include "application/TaskRepository.h"

class TaskService {
public:
    explicit TaskService(TaskRepository& repository);

    QVector<Task> allTasks() const;
    Task getTask(int id) const;
    void completeTask(int id);
    void reopenTask(int id);

private:
    TaskRepository& m_repository;
};
