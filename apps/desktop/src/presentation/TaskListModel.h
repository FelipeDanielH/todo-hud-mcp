#pragma once
#include <QAbstractListModel>
#include <QVector>
#include "domain/Task.h"

class TaskService;

class TaskListModel : public QAbstractListModel {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(TaskListModel)

public:
    enum Roles {
        IdRole = Qt::UserRole + 1,
        TitleRole,
        CompletedRole,
        StatusTextRole,
    };

    explicit TaskListModel(TaskService& service, QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void refresh();

private:
    TaskService& m_service;
    QVector<Task> m_tasks;
};
