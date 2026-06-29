#include "TaskModel.h"

TaskModel::TaskModel(QObject* parent)
    : QAbstractListModel(parent)
{
    m_tasks = {
        {QStringLiteral("Probar MCP desde ChatGPT"),         true},
        {QStringLiteral("Celebrar el primer deploy real"),   false},
        {QStringLiteral("Preparar siguiente mejora de Focus HUD"), false},
    };
}

int TaskModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : m_tasks.size();
}

QVariant TaskModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= m_tasks.size())
        return {};

    const auto& task = m_tasks.at(index.row());

    switch (role) {
    case Qt::DisplayRole:
    case TitleRole:
        return task.title;
    case StatusRole:
        return task.completed ? QStringLiteral("completada")
                              : QStringLiteral("pendiente");
    }
    return {};
}

QHash<int, QByteArray> TaskModel::roleNames() const
{
    return {
        {TitleRole,  "title"},
        {StatusRole, "status"},
    };
}

void TaskModel::setCompleted(int row, bool completed)
{
    if (row < 0 || row >= m_tasks.size())
        return;
    m_tasks[row].completed = completed;
    emit dataChanged(index(row), index(row), {StatusRole});
}

QString TaskModel::titleAt(int row) const
{
    if (row < 0 || row >= m_tasks.size())
        return {};
    return m_tasks.at(row).title;
}
