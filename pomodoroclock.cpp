#include "pomodoroclock.h"

PomodoroClock::PomodoroClock(QObject *parent)
    : QObject(parent),
    m_timer(new CountdownTimer(this)),
    m_currentMode(Mode::Work),
    m_workSeconds(25*60),   // 默认 25 分钟工作
    m_breakSeconds(5*60)    // 默认 5 分钟休息
{
    connect(m_timer, &CountdownTimer::timeChanged, this, &PomodoroClock::timeChanged);
    connect(m_timer, &CountdownTimer::timeout, this, &PomodoroClock::handleTimeout);

    setupCurrentModeDuration();
}

void PomodoroClock::start()
{
    m_timer->start();
}

void PomodoroClock::pause()
{
    m_timer->pause();
}

void PomodoroClock::reset()
{
    m_currentMode = Mode::Work;
    setupCurrentModeDuration();
    emit modeChanged(m_currentMode);
}

PomodoroClock::Mode PomodoroClock::currentMode() const
{
    return m_currentMode;
}

void PomodoroClock::handleTimeout()
{
    emit sessionFinished(m_currentMode);
    switchMode();
}

void PomodoroClock::switchMode()
{
    if (m_currentMode == Mode::Work)
        m_currentMode = Mode::Break;
    else
        m_currentMode = Mode::Work;

    setupCurrentModeDuration();
    emit modeChanged(m_currentMode);
}

void PomodoroClock::setupCurrentModeDuration()
{
    if (m_currentMode == Mode::Work)
        m_timer->setDuration(m_workSeconds);
    else
        m_timer->setDuration(m_breakSeconds);
}
