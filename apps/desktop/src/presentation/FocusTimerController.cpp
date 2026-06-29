#include "FocusTimerController.h"

FocusTimerController::FocusTimerController(QObject* parent)
    : QObject(parent)
{
    connect(&m_timer, &QTimer::timeout, this, &FocusTimerController::tick);
    reset();
}

QString FocusTimerController::formattedTime() const
{
    return formatTime(m_remainingSeconds);
}

bool FocusTimerController::isRunning() const
{
    return m_timer.isActive();
}

void FocusTimerController::start()
{
    if (isRunning())
        return;

    if (m_remainingSeconds == 0)
        m_remainingSeconds = FOCUS_DURATION_SECONDS;

    m_timer.start(1000);
    emit isRunningChanged();
    emit formattedTimeChanged();
}

void FocusTimerController::stop()
{
    if (!isRunning())
        return;

    m_timer.stop();
    emit isRunningChanged();
}

void FocusTimerController::reset()
{
    const bool wasRunning = isRunning();
    if (wasRunning)
        m_timer.stop();

    m_remainingSeconds = FOCUS_DURATION_SECONDS;
    emit formattedTimeChanged();

    if (wasRunning)
        emit isRunningChanged();
}

void FocusTimerController::tick()
{
    if (m_remainingSeconds > 0) {
        --m_remainingSeconds;
        emit formattedTimeChanged();
    }

    if (m_remainingSeconds == 0) {
        m_timer.stop();
        emit isRunningChanged();
        emit focusCompleted();
    }
}

QString FocusTimerController::formatTime(int totalSeconds)
{
    int mins = totalSeconds / 60;
    int secs = totalSeconds % 60;
    return QStringLiteral("%1:%2")
        .arg(mins, 2, 10, QLatin1Char('0'))
        .arg(secs, 2, 10, QLatin1Char('0'));
}
