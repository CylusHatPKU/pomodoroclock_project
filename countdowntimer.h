#ifndef COUNTDOWNTIMER_H
#define COUNTDOWNTIMER_H

#include <QObject>
#include <QTimer>

/*
 * CountdownTimer: 纯倒计时类
 * 不关心工作/休息模式
 * 提供 start/pause/reset，发射时间更新和结束信号
 */
class CountdownTimer : public QObject
{
    Q_OBJECT
public:
    explicit CountdownTimer(QObject *parent = nullptr);

    void setDuration(int seconds);  // 设置总时间
    void start();                   // 开始倒计时
    void pause();                   // 暂停
    void reset();                   // 重置为总时间

    int remainingSeconds() const;   // 获取剩余秒数

signals:
    void timeChanged(int secondsLeft); // 每秒触发
    void timeout();                     // 倒计时结束

private slots:
    void onTick(); // 内部定时器触发槽

private:
    QTimer *m_timer;       // Qt 内置定时器
    int m_totalSeconds;    // 总时间
    int m_remainingSeconds; // 剩余时间
};

#endif
