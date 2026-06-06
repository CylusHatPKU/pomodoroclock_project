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
    void addMinutesToCurrent(int minutes); // 给当前阶段增加时间

    Mode currentMode() const;
    Mode nextMode() const; // 获取下一个阶段
    int durationMinutesForMode(Mode mode) const; // 获取某个阶段的持续分钟数
    bool isRunning() const; // 判断番茄钟是否正在计时

signals:
    void timeChanged(int secondsLeft);      // 剩余时间更新
    void modeChanged(PomodoroClock::Mode mode); // 模式切换
    void sessionFinished(PomodoroClock::Mode finishedMode); // 本阶段结束
    void runningStateChanged(bool running); // 计时运行状态变化

private slots:
    void handleTimeout(); // Timer 超时

private:
    void switchMode();          // 切换模式
    void setupCurrentModeDuration(); // 设置当前模式的倒计时

private:
    CountdownTimer *m_timer;
    Mode m_currentMode;

    int m_workSeconds;  // 工作阶段总秒数
    int m_breakSeconds; // 休息阶段总秒数
};

#endif
