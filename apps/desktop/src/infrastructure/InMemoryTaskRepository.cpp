#include <QDateTime>
#include <QJsonObject>
#include <QJsonArray>
#include "InMemoryTaskRepository.h"

InMemoryTaskRepository::InMemoryTaskRepository()
    : m_tasks{
        {1,  {}, QStringLiteral("Abrir consola AWS"),         false, QStringLiteral("pending"),    QStringLiteral("phase-lab281-1"), QStringLiteral("Lab 281 - Fase 1"), {}, {}, 0},
        {2,  {}, QStringLiteral("Ir a CloudWatch"),            false, QStringLiteral("pending"),    QStringLiteral("phase-lab281-1"), QStringLiteral("Lab 281 - Fase 1"), {}, {}, 1},
        {3,  {}, QStringLiteral("Crear alarma de métrica"),    false, QStringLiteral("pending"),    QStringLiteral("phase-lab281-1"), QStringLiteral("Lab 281 - Fase 1"), {}, {}, 2},
        {4,  {}, QStringLiteral("Validar métrica en dashboard"),true, QStringLiteral("completed"),  QStringLiteral("phase-lab281-1"), QStringLiteral("Lab 281 - Fase 1"), QStringLiteral("2026-06-29T10:30:00Z"), {}, 3},
        {5,  {}, QStringLiteral("Configurar notificación SNS"),true,  QStringLiteral("completed"),  QStringLiteral("phase-lab281-1"), QStringLiteral("Lab 281 - Fase 1"), QStringLiteral("2026-06-29T10:35:00Z"), {}, 4},
        {6,  {}, QStringLiteral("Informe - Introducción"),     false, QStringLiteral("pending"),    QStringLiteral("phase-informe"),  QStringLiteral("Informe - Desarrollo"), {}, {}, 0},
        {7,  {}, QStringLiteral("Informe - Metodología"),      false, QStringLiteral("pending"),    QStringLiteral("phase-informe"),  QStringLiteral("Informe - Desarrollo"), {}, {}, 1},
        {8,  {}, QStringLiteral("Preparar siguiente mejora"),  false, QStringLiteral("pending"),    {}, {}, {}, {}, 0},
    }
{
}

QVector<Task> InMemoryTaskRepository::all() const
{
    QVector<Task> result;
    for (const auto& t : m_tasks) {
        if (t.status != QStringLiteral("archived"))
            result.append(t);
    }
    return result;
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

void InMemoryTaskRepository::save(const Task& task)
{
    m_tasks.append(task);
}

QVector<Task> InMemoryTaskRepository::archived() const
{
    QVector<Task> result;
    for (const auto& t : m_tasks) {
        if (t.status == QStringLiteral("archived"))
            result.append(t);
    }
    return result;
}

QJsonObject InMemoryTaskRepository::archiveCompletedTasks(const QString& phaseId)
{
    const auto now = QDateTime::currentDateTimeUtc().toString(Qt::ISODate);
    int count = 0;
    for (auto& t : m_tasks) {
        if (t.status == QStringLiteral("completed")) {
            if (!phaseId.isEmpty() && t.phaseId != phaseId)
                continue;
            t.status = QStringLiteral("archived");
            t.archivedAt = now;
            ++count;
        }
    }
    QJsonObject result;
    result[QStringLiteral("archived")] = count;
    result[QStringLiteral("archivedAt")] = now;
    return result;
}

QJsonObject InMemoryTaskRepository::createBatch(const QString& phaseName, const QStringList& titles)
{
    const QString phaseId = QStringLiteral("local-") + QString::number(QDateTime::currentMSecsSinceEpoch());
    QJsonArray tasksArr;
    for (int i = 0; i < titles.size(); ++i) {
        Task task;
        task.id = m_tasks.size() + i + 1;
        task.title = titles[i];
        task.completed = false;
        task.status = QStringLiteral("pending");
        task.phaseId = phaseId;
        task.phaseName = phaseName;
        task.sortOrder = i;
        m_tasks.append(task);
        QJsonObject taskObj;
        taskObj[QStringLiteral("id")] = QString::number(task.id);
        taskObj[QStringLiteral("title")] = task.title;
        taskObj[QStringLiteral("status")] = task.status;
        tasksArr.append(taskObj);
    }
    QJsonObject result;
    result[QStringLiteral("phaseId")] = phaseId;
    result[QStringLiteral("phaseName")] = phaseName;
    result[QStringLiteral("tasks")] = tasksArr;
    return result;
}
