#include <QEventLoop>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QTimer>
#include "ApiTaskRepository.h"

ApiTaskRepository::ApiTaskRepository(const QString& baseUrl)
    : m_baseUrl(baseUrl)
{
    if (!m_baseUrl.endsWith(QLatin1Char('/')))
        m_baseUrl.append(QLatin1Char('/'));
    checkConnection();
}

bool ApiTaskRepository::isConnected() const
{
    return m_connected;
}

QString ApiTaskRepository::baseUrl() const
{
    return m_baseUrl;
}

void ApiTaskRepository::checkConnection()
{
    const auto reply = doGet(QStringLiteral("tasks"));
    m_connected = !reply.isEmpty();
    qDebug().noquote() << QStringLiteral("[ApiTaskRepository] API %1 : %2")
        .arg(m_baseUrl, m_connected ? QStringLiteral("connected") : QStringLiteral("NOT REACHABLE - fallback will be used"));
}

bool ApiTaskRepository::waitForReply(QNetworkReply* reply, const QString& operation) const
{
    QEventLoop loop;
    QTimer timeout;
    bool timedOut = false;

    timeout.setSingleShot(true);
    QObject::connect(reply, &QNetworkReply::finished, &loop, &QEventLoop::quit);
    QObject::connect(&timeout, &QTimer::timeout, &loop, [&]() {
        timedOut = true;
        qWarning().noquote() << QStringLiteral("[ApiTaskRepository] %1 timed out after %2ms")
            .arg(operation)
            .arg(NETWORK_TIMEOUT_MS);
        reply->abort();
        loop.quit();
    });

    timeout.start(NETWORK_TIMEOUT_MS);
    loop.exec();

    if (timeout.isActive())
        timeout.stop();

    return !timedOut;
}

QJsonObject ApiTaskRepository::doGet(const QString& path) const
{
    QNetworkRequest req(QUrl(m_baseUrl + path));
    req.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));

    QNetworkReply* reply = m_manager.get(req);
    const bool completed = waitForReply(reply, QStringLiteral("GET %1").arg(path));

    QJsonObject result;
    if (completed && reply->error() == QNetworkReply::NoError) {
        const auto data = reply->readAll();
        const auto doc = QJsonDocument::fromJson(data);
        if (doc.isObject())
            result = doc.object();
        else if (doc.isArray())
            result[QStringLiteral("_array")] = doc.array();
    } else {
        qWarning().noquote() << QStringLiteral("[ApiTaskRepository] GET %1 failed: %2")
            .arg(path, reply->errorString());
    }
    reply->deleteLater();
    return result;
}

QJsonObject ApiTaskRepository::doPost(const QString& path, const QJsonObject& body)
{
    QNetworkRequest req(QUrl(m_baseUrl + path));
    req.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));

    const QByteArray payload = QJsonDocument(body).toJson(QJsonDocument::Compact);
    QNetworkReply* reply = m_manager.post(req, payload);
    const bool completed = waitForReply(reply, QStringLiteral("POST %1").arg(path));

    QJsonObject result;
    if (completed && reply->error() == QNetworkReply::NoError) {
        const auto data = reply->readAll();
        const auto doc = QJsonDocument::fromJson(data);
        if (doc.isObject())
            result = doc.object();
        else if (doc.isArray())
            result[QStringLiteral("_array")] = doc.array();
    } else {
        qWarning().noquote() << QStringLiteral("[ApiTaskRepository] POST %1 failed: %2")
            .arg(path, reply->errorString());
    }
    reply->deleteLater();
    return result;
}

QJsonObject ApiTaskRepository::doPatch(const QString& path, const QJsonObject& body)
{
    QNetworkRequest req(QUrl(m_baseUrl + path));
    req.setHeader(QNetworkRequest::ContentTypeHeader, QStringLiteral("application/json"));

    const QByteArray payload = QJsonDocument(body).toJson(QJsonDocument::Compact);
    QNetworkReply* reply = m_manager.sendCustomRequest(req, "PATCH", payload);
    const bool completed = waitForReply(reply, QStringLiteral("PATCH %1").arg(path));

    QJsonObject result;
    if (completed && reply->error() == QNetworkReply::NoError) {
        const auto data = reply->readAll();
        const auto doc = QJsonDocument::fromJson(data);
        if (doc.isObject())
            result = doc.object();
        else if (doc.isArray())
            result[QStringLiteral("_array")] = doc.array();
    } else {
        qWarning().noquote() << QStringLiteral("[ApiTaskRepository] PATCH %1 failed: %2")
            .arg(path, reply->errorString());
    }
    reply->deleteLater();
    return result;
}

Task ApiTaskRepository::parseTask(const QJsonObject& obj, int rowId) const
{
    Task t;
    t.id = rowId;
    t.apiId = obj[QStringLiteral("id")].toString();
    t.title = obj[QStringLiteral("title")].toString();
    t.completed = obj[QStringLiteral("completed")].toBool();
    t.status = obj[QStringLiteral("status")].toString();
    t.phaseId = obj[QStringLiteral("phaseId")].toString();
    t.phaseName = obj[QStringLiteral("phaseName")].toString();
    t.completedAt = obj[QStringLiteral("completedAt")].toString();
    t.archivedAt = obj[QStringLiteral("archivedAt")].toString();
    t.sortOrder = obj[QStringLiteral("sortOrder")].toInt();
    return t;
}

int ApiTaskRepository::findIdByApiId(const QString& apiId) const
{
    const auto tasks = all();
    for (const auto& t : tasks) {
        if (t.apiId == apiId)
            return t.id;
    }
    return -1;
}

QVector<Task> ApiTaskRepository::all() const
{
    QVector<Task> result;
    const auto json = doGet(QStringLiteral("tasks"));
    const auto arr = json[QStringLiteral("_array")].toArray();
    for (int i = 0; i < arr.size(); ++i) {
        result.append(parseTask(arr[i].toObject(), i));
    }
    return result;
}

Task ApiTaskRepository::getById(int id) const
{
    const auto tasks = all();
    for (const auto& t : tasks) {
        if (t.id == id)
            return t;
    }
    return {};
}

void ApiTaskRepository::update(const Task& task)
{
    const QString path = QStringLiteral("tasks/%1").arg(task.apiId.isEmpty() ? QString::number(task.id) : task.apiId);
    QJsonObject body;
    body[QStringLiteral("completed")] = task.completed;
    body[QStringLiteral("status")] = task.status;
    if (!task.completedAt.isEmpty())
        body[QStringLiteral("completedAt")] = task.completedAt;

    if (task.status == QStringLiteral("completed") || task.completed) {
        doPatch(path + QStringLiteral("/complete"), {});
    } else if (task.status == QStringLiteral("pending") && !task.completed) {
        doPatch(path + QStringLiteral("/reopen"), {});
    } else {
        doPatch(path, body);
    }
}

void ApiTaskRepository::save(const Task& task)
{
    QJsonObject body;
    body[QStringLiteral("title")] = task.title;
    if (!task.phaseId.isEmpty())
        body[QStringLiteral("phaseId")] = task.phaseId;
    if (!task.phaseName.isEmpty())
        body[QStringLiteral("phaseName")] = task.phaseName;
    doPost(QStringLiteral("tasks"), body);
}

QVector<Task> ApiTaskRepository::archived() const
{
    QVector<Task> result;
    const auto json = doGet(QStringLiteral("tasks/history"));
    const auto arr = json[QStringLiteral("_array")].toArray();
    int rowId = 0;
    for (const auto& phaseVal : arr) {
        const auto phase = phaseVal.toObject();
        const auto tasksArr = phase[QStringLiteral("tasks")].toArray();
        for (const auto& taskVal : tasksArr) {
            result.append(parseTask(taskVal.toObject(), rowId++));
        }
    }
    return result;
}

ArchiveTasksResult ApiTaskRepository::archiveCompletedTasks(const QString& phaseId)
{
    QJsonObject body;
    if (!phaseId.isEmpty())
        body[QStringLiteral("phaseId")] = phaseId;
    const auto json = doPost(QStringLiteral("tasks/archive"), body);

    ArchiveTasksResult result;
    if (json.isEmpty()) {
        result.ok = false;
        result.error = QStringLiteral("Archive request failed");
        return result;
    }

    result.archived = json[QStringLiteral("archived")].toInt();
    result.archivedAt = json[QStringLiteral("archivedAt")].toString();
    return result;
}

BatchCreateResult ApiTaskRepository::createBatch(const QString& phaseName, const QStringList& titles)
{
    QJsonObject body;
    body[QStringLiteral("phaseName")] = phaseName;
    QJsonArray arr;
    for (const auto& t : titles)
        arr.append(t);
    body[QStringLiteral("tasks")] = arr;

    const auto json = doPost(QStringLiteral("tasks/batch"), body);

    BatchCreateResult result;
    if (json.isEmpty()) {
        result.ok = false;
        result.error = QStringLiteral("Batch creation request failed");
        return result;
    }

    result.phaseId = json[QStringLiteral("phaseId")].toString();
    result.phaseName = json[QStringLiteral("phaseName")].toString();
    const auto tasks = json[QStringLiteral("tasks")].toArray();
    for (int i = 0; i < tasks.size(); ++i)
        result.tasks.append(parseTask(tasks[i].toObject(), i));
    return result;
}
