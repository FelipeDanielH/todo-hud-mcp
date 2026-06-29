#include "InMemoryTaskRepository.h"

InMemoryTaskRepository::InMemoryTaskRepository()
    : m_tasks{
        {1, QStringLiteral("Probar MCP desde ChatGPT"), true},
        {2, QStringLiteral("Celebrar el primer deploy real"), false},
        {3, QStringLiteral("Preparar siguiente mejora de Focus HUD"), false},
    }
{
}

QVector<Task> InMemoryTaskRepository::all() const
{
    return m_tasks;
}

Task InMemoryTaskRepository::getById(int id) const
{
    for (const auto& t : m_tasks) {
        if (t.id == id)
            return t;
    }
    return {};
}

void InMemoryTaskRepository::update(const Task& task)
{
    for (auto& t : m_tasks) {
        if (t.id == task.id) {
            t = task;
            return;
        }
    }
}
