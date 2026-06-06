#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "pomodoroclock.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateTimeDisplay(int secondsLeft); // 更新时间显示
    void updateModeDisplay(PomodoroClock::Mode mode); // 更新模式显示
    void showSessionFinished(PomodoroClock::Mode mode); // 弹窗提示

private:
    QString formatTime(int seconds) const; // 格式化 mm:ss

private:
    Ui::MainWindow *ui;
    PomodoroClock *m_clock;
};

#endif
