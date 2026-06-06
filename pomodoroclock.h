#ifndef POMODOROCLOCK_H
#define POMODOROCLOCK_H

#include <QObject>
#include "countdowntimer.h"

/*
 * PomodoroClock: 番茄钟业务逻辑类
 * 使用 CountdownTimer 完成工作/休息模式切换
 */
class PomodoroClock : public QObject
{
    Q_OBJECT
public:
    enum class Mode { Work, Break };

    explicit PomodoroClock(QObject *parent = nullptr);

    void start();
    void pause();
    void reset();

    Mode currentMode() const;

signals:
    void timeChanged(int secondsLeft);      // 剩余时间更新
    void modeChanged(PomodoroClock::Mode mode); // 模式切换
    void sessionFinished(PomodoroClock::Mode finishedMode); // 本阶段结束

private slots:
    void handleTimeout(); // Timer 超时

private:
    void switchMode();          // 切换模式
    void setupCurrentModeDuration(); // 设置当前模式的倒计时

private:
    CountdownTimer *m_timer;
    Mode m_currentMode;

    int m_workSeconds;
    int m_breakSeconds;
};

#endif
