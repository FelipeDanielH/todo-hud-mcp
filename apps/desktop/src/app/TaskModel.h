#ifndef FOCUSHUD_TASK_MODEL_H
#define FOCUSHUD_TASK_MODEL_H

#include <QAbstractListModel>
#include <QString>
#include <QVector>

struct TaskData {
    QString title;
    bool completed = false;
};

class TaskModel : public QAbstractListModel {
    Q_OBJECT

public:
    enum Roles {
        TitleRole = Qt::UserRole + 1,
        StatusRole
    };

    explicit TaskModel(QObject* parent = nullptr);

    int rowCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;

    void setCompleted(int row, bool completed);
    QString titleAt(int row) const;

private:
    QVector<TaskData> m_tasks;
};

#endif
