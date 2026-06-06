#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QRect>
#include "pomodoroclock.h"

class QResizeEvent;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void resizeEvent(QResizeEvent *event) override; // 窗口大小变化时重新适配界面

private slots:
    void updateTimeDisplay(int secondsLeft); // 更新时间显示
    void updateModeDisplay(PomodoroClock::Mode mode); // 更新模式显示
    void showSessionFinished(PomodoroClock::Mode mode); // 弹窗提示
    void addFiveMinutes(); // 给当前倒计时增加 5 分钟
    void addQueueItem(); // 添加队列项
    void removeSelectedQueueItem(); // 删除选中的队列项
    void clearQueueItems(); // 清空队列
    void queueChanged(); // 队列内容或顺序变化

private:
    QString formatTime(int seconds) const; // 格式化 mm:ss
    QString modeText(PomodoroClock::Mode mode) const; // 模式文字
    void updateNextStageDisplay(); // 更新下一个阶段提示
    void appendQueueItem(PomodoroClock::Mode mode, int minutes); // 将阶段加入队列
    QRect scaledRect(const QRect &baseRect) const; // 按窗口比例缩放控件位置
    void scaleUiToWindow(); // 根据当前窗口大小调整控件

private:
    Ui::MainWindow *ui;
    PomodoroClock *m_clock;

    PomodoroClock::Mode m_lastQueueMode; // 记录上次添加的阶段类型
    int m_lastQueueWorkMinutes; // 记录上次添加的工作时长
    int m_lastQueueBreakMinutes; // 记录上次添加的休息时长
};

#endif
