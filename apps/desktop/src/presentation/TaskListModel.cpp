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
    return parent.isValid() ? 0 : m_activeTasks.size() + m_completedTasks.size() + 1;
}

QVariant TaskListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= rowCount())
        return {};

    const int row = index.row();
    const int activeCount = m_activeTasks.size();

    // Section header for completed
    if (row == activeCount && m_completedTasks.size() > 0) {
        switch (role) {
        case TitleRole:     return QStringLiteral("COMPLETED %1").arg(m_completedTasks.size());
        case StatusRole:    return QStringLiteral("section-completed");
        case IdRole:        return -1;
        case CompletedRole: return false;
        case PhaseIdRole:   return QString{};
        case PhaseNameRole: return QString{};
        case SortOrderRole: return 0;
        }
        return {};
    }

    const Task* task = nullptr;
    if (row < activeCount) {
        task = &m_activeTasks.at(row);
    } else if (row > activeCount) {
        task = &m_completedTasks.at(row - activeCount - 1);
    }

    if (!task) return {};

    switch (role) {
    case IdRole:           return task->id;
    case TitleRole:        return task->title;
    case CompletedRole:    return task->completed;
    case StatusRole:       return task->status;
    case PhaseIdRole:      return task->phaseId;
    case PhaseNameRole:    return task->phaseName;
    case SortOrderRole:    return task->sortOrder;
    }
    return {};
}

QHash<int, QByteArray> TaskListModel::roleNames() const
{
    return {
        {IdRole,         "taskId"},
        {TitleRole,      "title"},
        {CompletedRole,  "completed"},
        {StatusRole,     "status"},
        {PhaseIdRole,    "phaseId"},
        {PhaseNameRole,  "phaseName"},
        {SortOrderRole,  "sortOrder"},
    };
}

void TaskListModel::refresh()
{
    beginResetModel();
    m_activeTasks = m_service.activeTasks();
    m_completedTasks = m_service.completedTasks();
    endResetModel();
}

QVector<Task> TaskListModel::activeTasks() const
{
    return m_activeTasks;
}

QVector<Task> TaskListModel::completedTasks() const
{
    return m_completedTasks;
}

int TaskListModel::completedCount() const
{
    return m_completedTasks.size();
}
