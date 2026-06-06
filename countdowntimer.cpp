#include "countdowntimer.h"

CountdownTimer::CountdownTimer(QObject *parent)
    : QObject(parent),
    m_timer(new QTimer(this)),
    m_totalSeconds(0),
    m_remainingSeconds(0)
{
    // 每秒调用 onTick
    connect(m_timer, &QTimer::timeout, this, &CountdownTimer::onTick);
}

void CountdownTimer::setDuration(int seconds)
{
    m_totalSeconds = seconds;
    m_remainingSeconds = seconds;
    emit timeChanged(m_remainingSeconds);
}

void CountdownTimer::start()
{
    if (m_remainingSeconds > 0)
        m_timer->start(1000); // 每 1000ms 触发一次
}

void CountdownTimer::pause()
{
    m_timer->stop();
}

void CountdownTimer::reset()
{
    m_timer->stop();
    m_remainingSeconds = m_totalSeconds;
    emit timeChanged(m_remainingSeconds);
}

int CountdownTimer::remainingSeconds() const
{
    return m_remainingSeconds;
}

void CountdownTimer::onTick()
{
    if (m_remainingSeconds > 0)
    {
        m_remainingSeconds--;
        emit timeChanged(m_remainingSeconds);
    }

    if (m_remainingSeconds <= 0)
    {
        m_timer->stop();
        emit timeout(); // 倒计时结束信号
    }
}
