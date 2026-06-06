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
    emit runningStateChanged(m_timer->isRunning());
}

void PomodoroClock::pause()
{
    m_timer->pause();
    emit runningStateChanged(false);
}

void PomodoroClock::reset()
{
    m_timer->pause();
    m_currentMode = Mode::Work;
    setupCurrentModeDuration();
    emit modeChanged(m_currentMode);
    emit runningStateChanged(false);
}

void PomodoroClock::addMinutesToCurrent(int minutes)
{
    // 将按钮添加的分钟数同步到当前阶段配置和正在显示的倒计时
    if (minutes <= 0)
        return;

    int seconds = minutes * 60;
    if (m_currentMode == Mode::Work)
        m_workSeconds += seconds;
    else
        m_breakSeconds += seconds;

    m_timer->addSeconds(seconds);
}

PomodoroClock::Mode PomodoroClock::currentMode() const
{
    return m_currentMode;
}

PomodoroClock::Mode PomodoroClock::nextMode() const
{
    if (m_currentMode == Mode::Work)
        return Mode::Break;
    return Mode::Work;
}

int PomodoroClock::durationMinutesForMode(Mode mode) const
{
    // 界面显示使用分钟，业务内部仍然用秒保存，避免倒计时计算混乱
    if (mode == Mode::Work)
        return m_workSeconds / 60;
    return m_breakSeconds / 60;
}

bool PomodoroClock::isRunning() const
{
    return m_timer->isRunning();
}

void PomodoroClock::handleTimeout()
{
    emit sessionFinished(m_currentMode);
    switchMode();
    start(); // 当前阶段结束后，自动开始下一阶段倒计时
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
