#pragma once
#include <QString>
#include <QVector>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include "application/TaskRepository.h"

class QNetworkReply;

class ApiTaskRepository : public TaskRepository {
public:
    explicit ApiTaskRepository(const QString& baseUrl = QStringLiteral("http://localhost:3000"));

    bool isConnected() const;
    QString baseUrl() const;

    QVector<Task> all() const override;
    Task getById(int id) const override;
    void update(const Task& task) override;
    void save(const Task& task) override;
    QVector<Task> archived() const override;

    ArchiveTasksResult archiveCompletedTasks(const QString& phaseId = {}) override;
    BatchCreateResult createBatch(const QString& phaseName, const QStringList& titles) override;

private:
    Task parseTask(const QJsonObject& obj, int rowId) const;
    int findIdByApiId(const QString& apiId) const;
    void checkConnection();
    bool waitForReply(QNetworkReply* reply, const QString& operation) const;

    QJsonObject doGet(const QString& path) const;
    QJsonObject doPost(const QString& path, const QJsonObject& body);
    QJsonObject doPatch(const QString& path, const QJsonObject& body);

    mutable QNetworkAccessManager m_manager;
    QString m_baseUrl;
    mutable bool m_connected = false;

    static constexpr int NETWORK_TIMEOUT_MS = 5000;
};
