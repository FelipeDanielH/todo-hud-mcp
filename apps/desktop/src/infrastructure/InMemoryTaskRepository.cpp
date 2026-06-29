#include <QDateTime>
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

ArchiveTasksResult InMemoryTaskRepository::archiveCompletedTasks(const QString& phaseId)
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
    ArchiveTasksResult result;
    result.archived = count;
    result.archivedAt = now;
    return result;
}

BatchCreateResult InMemoryTaskRepository::createBatch(const QString& phaseName, const QStringList& titles)
{
    const QString phaseId = QStringLiteral("local-") + QString::number(QDateTime::currentMSecsSinceEpoch());
    int nextId = 1;
    for (const auto& existing : m_tasks) {
        if (existing.id >= nextId)
            nextId = existing.id + 1;
    }

    QVector<Task> created;
    for (int i = 0; i < titles.size(); ++i) {
        Task task;
        task.id = nextId++;
        task.title = titles[i];
        task.completed = false;
        task.status = QStringLiteral("pending");
        task.phaseId = phaseId;
        task.phaseName = phaseName;
        task.sortOrder = i;
        m_tasks.append(task);
        created.append(task);
    }
    BatchCreateResult result;
    result.phaseId = phaseId;
    result.phaseName = phaseName;
    result.tasks = created;
    return result;
}
