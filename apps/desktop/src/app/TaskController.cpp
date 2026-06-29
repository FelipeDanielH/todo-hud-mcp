#include "TaskController.h"
#include <QDebug>

TaskController::TaskController(TaskModel* model, QObject* parent)
    : QObject(parent)
    , m_model(model)
    , m_timer(new QTimer(this))
{
    connect(m_timer, &QTimer::timeout, this, &TaskController::updateTimer);

    m_currentTaskIndex = 1;
}

QString TaskController::formattedTime() const
{
    return formatTime(m_remainingSeconds);
}

QString TaskController::currentTaskTitle() const
{
    if (m_currentTaskIndex < 0)
        return {};
    return m_model->titleAt(m_currentTaskIndex);
}

bool TaskController::isRunning() const
{
    return m_timer->isActive();
}

bool TaskController::hasActiveTask() const
{
    return m_currentTaskIndex >= 0
        && m_currentTaskIndex < m_model->rowCount({});
}

TaskModel* TaskController::taskModel() const
{
    return m_model;
}

void TaskController::startFocus()
{
    if (!hasActiveTask())
        return;

    if (!isRunning()) {
        m_remainingSeconds = FOCUS_DURATION;
        m_timer->start(1000);
        emit isRunningChanged();
        emit formattedTimeChanged();
    }
}

void TaskController::stopFocus()
{
    if (isRunning()) {
        m_timer->stop();
        emit isRunningChanged();
    }
}

void TaskController::completeCurrentTask()
{
    if (!hasActiveTask())
        return;

    stopFocus();
    m_model->setCompleted(m_currentTaskIndex, true);
    m_remainingSeconds = 0;
    emit formattedTimeChanged();
    emit currentTaskChanged();
}

void TaskController::reopenTask(int index)
{
    if (index < 0 || index >= m_model->rowCount({}))
        return;
    m_model->setCompleted(index, false);
}

void TaskController::selectTask(int index)
{
    if (index < 0 || index >= m_model->rowCount({}))
        return;

    stopFocus();
    m_currentTaskIndex = index;
    m_remainingSeconds = 0;
    emit formattedTimeChanged();
    emit currentTaskChanged();
}

void TaskController::updateTimer()
{
    if (m_remainingSeconds > 0) {
        --m_remainingSeconds;
        emit formattedTimeChanged();
    }

    if (m_remainingSeconds == 0) {
        stopFocus();
    }
}

QString TaskController::formatTime(int totalSeconds)
{
    int mins = totalSeconds / 60;
    int secs = totalSeconds % 60;
    return QStringLiteral("%1:%2")
        .arg(mins, 2, 10, QLatin1Char('0'))
        .arg(secs, 2, 10, QLatin1Char('0'));
}
