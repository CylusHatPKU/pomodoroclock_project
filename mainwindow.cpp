#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_clock(new PomodoroClock(this))
{
    ui->setupUi(this);

    // 按钮事件连接到 PomodoroClock
    connect(ui->startButton, &QPushButton::clicked, m_clock, &PomodoroClock::start);
    connect(ui->pauseButton, &QPushButton::clicked, m_clock, &PomodoroClock::pause);
    connect(ui->resetButton, &QPushButton::clicked, m_clock, &PomodoroClock::reset);

    // 信号槽连接
    connect(m_clock, &PomodoroClock::timeChanged, this, &MainWindow::updateTimeDisplay);
    connect(m_clock, &PomodoroClock::modeChanged, this, &MainWindow::updateModeDisplay);
    connect(m_clock, &PomodoroClock::sessionFinished, this, &MainWindow::showSessionFinished);

    // 初始化显示
    updateModeDisplay(PomodoroClock::Mode::Work);
    updateTimeDisplay(25*60);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::updateTimeDisplay(int secondsLeft)
{
    ui->timeLabel->setText(formatTime(secondsLeft));
}

void MainWindow::updateModeDisplay(PomodoroClock::Mode mode)
{
    if (mode == PomodoroClock::Mode::Work)
        ui->modeLabel->setText("工作时间");
    else
        ui->modeLabel->setText("休息时间");
}

void MainWindow::showSessionFinished(PomodoroClock::Mode mode)
{
    if (mode == PomodoroClock::Mode::Work)
        QMessageBox::information(this, "番茄钟", "工作时间结束，休息一下吧！");
    else
        QMessageBox::information(this, "番茄钟", "休息结束，准备开始工作！");
}

QString MainWindow::formatTime(int seconds) const
{
    int minutes = seconds / 60;
    int secs = seconds % 60;
    return QString("%1:%2").arg(minutes,2,10,QChar('0')).arg(secs,2,10,QChar('0'));
}
