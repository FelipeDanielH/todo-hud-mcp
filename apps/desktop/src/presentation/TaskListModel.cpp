#include "TaskListModel.h"
#include "application/TaskService.h"

TaskListModel::TaskListModel(TaskService& service, QObject* parent)
    : QAbstractListModel(parent)
    , m_service(service)
{
    refresh();
}

int TaskListModel::rowCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : m_tasks.size();
}

QVariant TaskListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= m_tasks.size())
        return {};

    const auto& task = m_tasks.at(index.row());

    switch (role) {
    case IdRole:           return task.id;
    case TitleRole:        return task.title;
    case CompletedRole:    return task.completed;
    case StatusTextRole:   return task.completed
                                ? QStringLiteral("completada")
                                : QStringLiteral("pendiente");
    }
    return {};
}

QHash<int, QByteArray> TaskListModel::roleNames() const
{
    return {
        {IdRole,         "taskId"},
        {TitleRole,      "title"},
        {CompletedRole,  "completed"},
        {StatusTextRole, "statusText"},
    };
}

void TaskListModel::refresh()
{
    beginResetModel();
    m_tasks = m_service.allTasks();
    endResetModel();
}
