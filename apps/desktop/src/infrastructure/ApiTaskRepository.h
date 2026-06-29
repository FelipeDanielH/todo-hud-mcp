// @tech-debt ApiTaskRepository uses synchronous (blocking) HTTP calls via QEventLoop.
// Future improvement: migrate to async QNetworkReply + signal/slot or
// QtConcurrent to avoid blocking the UI thread during GET /tasks refresh from WebSocket events.

#pragma once
#include <QString>
#include <QVector>
#include <QJsonObject>
#include <QJsonArray>
#include <QNetworkAccessManager>
#include "application/TaskRepository.h"

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

    QJsonObject archiveCompleted(const QString& phaseId = {});
    QJsonObject createBatch(const QString& phaseName, const QStringList& titles);

private:
    Task parseTask(const QJsonObject& obj, int rowId) const;
    int findIdByApiId(const QString& apiId) const;
    void checkConnection();

    QJsonObject doGet(const QString& path) const;
    QJsonObject doPost(const QString& path, const QJsonObject& body);
    QJsonObject doPatch(const QString& path, const QJsonObject& body);

    mutable QNetworkAccessManager m_manager;
    QString m_baseUrl;
    mutable bool m_connected = false;
};
