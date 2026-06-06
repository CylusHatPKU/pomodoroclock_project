#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QAbstractItemView>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QAbstractItemModel>
#include <QResizeEvent>
#include <QSpinBox>
#include <QtGlobal>
#include <QVBoxLayout>

namespace {
const int QueueModeRole = Qt::UserRole;       // 队列项保存工作/休息类型
const int QueueMinutesRole = Qt::UserRole + 1; // 队列项保存持续分钟数
const int BaseWidth = 758;   // 设计界面的基准宽度
const int BaseHeight = 628;  // 设计界面的基准中心区域高度，不包含状态栏
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_clock(new PomodoroClock(this)),
    m_lastQueueMode(PomodoroClock::Mode::Work),
    m_lastQueueWorkMinutes(25),
    m_lastQueueBreakMinutes(5)
{
    ui->setupUi(this);
    setMinimumSize(620, 520);

    // 按钮事件连接到 PomodoroClock
    connect(ui->startButton, &QPushButton::clicked, m_clock, &PomodoroClock::start);
    connect(ui->pauseButton, &QPushButton::clicked, m_clock, &PomodoroClock::pause);
    connect(ui->resetButton, &QPushButton::clicked, m_clock, &PomodoroClock::reset);
    connect(ui->addMinutesButton, &QPushButton::clicked, this, &MainWindow::addFiveMinutes);
    connect(ui->exitButton, &QPushButton::clicked, this, &MainWindow::close);
    connect(ui->addQueueButton, &QPushButton::clicked, this, &MainWindow::addQueueItem);
    connect(ui->removeQueueButton, &QPushButton::clicked, this, &MainWindow::removeSelectedQueueItem);
    connect(ui->clearQueueButton, &QPushButton::clicked, this, &MainWindow::clearQueueItems);

    // 队列项允许拖动排序，用户可以直接调整工作/休息阶段的执行顺序
    ui->queueListWidget->setDragDropMode(QAbstractItemView::InternalMove);
    ui->queueListWidget->setDefaultDropAction(Qt::MoveAction);
    ui->queueListWidget->setDragDropOverwriteMode(false);
    ui->queueListWidget->setDropIndicatorShown(true);
    connect(ui->queueListWidget->model(), &QAbstractItemModel::rowsMoved,
            this, &MainWindow::queueChanged);

    // 信号槽连接
    connect(m_clock, &PomodoroClock::timeChanged, this, &MainWindow::updateTimeDisplay);
    connect(m_clock, &PomodoroClock::modeChanged, this, &MainWindow::updateModeDisplay);
    connect(m_clock, &PomodoroClock::sessionFinished, this, &MainWindow::showSessionFinished);

    // 初始化显示
    updateModeDisplay(PomodoroClock::Mode::Work);
    updateNextStageDisplay();
    updateTimeDisplay(25*60);
    scaleUiToWindow();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    scaleUiToWindow();
}

void MainWindow::updateTimeDisplay(int secondsLeft)
{
    ui->timeLabel->setText(formatTime(secondsLeft));
}

void MainWindow::updateModeDisplay(PomodoroClock::Mode mode)
{
    ui->modeLabel->setText(modeText(mode));
    updateNextStageDisplay();
}

void MainWindow::showSessionFinished(PomodoroClock::Mode mode)
{
    if (mode == PomodoroClock::Mode::Work)
        QMessageBox::information(this, "番茄钟", "工作时间结束，休息一下吧！");
    else
        QMessageBox::information(this, "番茄钟", "休息结束，准备开始工作！");
}

void MainWindow::addFiveMinutes()
{
    // 替代原来的工作/休息时长设置，直接给当前阶段增加固定 5 分钟
    m_clock->addMinutesToCurrent(5);
    updateNextStageDisplay();
}

void MainWindow::addQueueItem()
{
    QDialog dialog(this);
    dialog.setWindowTitle("添加队列项");

    QComboBox *modeComboBox = new QComboBox(&dialog);
    modeComboBox->addItem("工作时间", static_cast<int>(PomodoroClock::Mode::Work));
    modeComboBox->addItem("休息时间", static_cast<int>(PomodoroClock::Mode::Break));
    modeComboBox->setCurrentIndex(m_lastQueueMode == PomodoroClock::Mode::Work ? 0 : 1);

    QSpinBox *minutesSpinBox = new QSpinBox(&dialog);
    minutesSpinBox->setRange(1, 120);
    minutesSpinBox->setSuffix(" 分钟");
    minutesSpinBox->setValue(m_lastQueueMode == PomodoroClock::Mode::Work
                                 ? m_lastQueueWorkMinutes
                                 : m_lastQueueBreakMinutes);

    // 切换工作/休息类型时，自动显示该类型上一次添加过的分钟数
    connect(modeComboBox, qOverload<int>(&QComboBox::currentIndexChanged), &dialog, [this, modeComboBox, minutesSpinBox]() {
        PomodoroClock::Mode selectedMode =
            static_cast<PomodoroClock::Mode>(modeComboBox->currentData().toInt());

        if (selectedMode == PomodoroClock::Mode::Work)
            minutesSpinBox->setValue(m_lastQueueWorkMinutes);
        else
            minutesSpinBox->setValue(m_lastQueueBreakMinutes);
    });

    QFormLayout *formLayout = new QFormLayout;
    formLayout->addRow("阶段类型：", modeComboBox);
    formLayout->addRow("持续时间：", minutesSpinBox);

    QDialogButtonBox *buttonBox =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    connect(buttonBox, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    QVBoxLayout *dialogLayout = new QVBoxLayout(&dialog);
    dialogLayout->addLayout(formLayout);
    dialogLayout->addWidget(buttonBox);

    if (dialog.exec() != QDialog::Accepted)
        return;

    PomodoroClock::Mode selectedMode =
        static_cast<PomodoroClock::Mode>(modeComboBox->currentData().toInt());
    int minutes = minutesSpinBox->value();

    // 保存上一次添加的数值，下一次打开添加窗口时可以直接复用
    m_lastQueueMode = selectedMode;
    if (selectedMode == PomodoroClock::Mode::Work)
        m_lastQueueWorkMinutes = minutes;
    else
        m_lastQueueBreakMinutes = minutes;

    appendQueueItem(selectedMode, minutes);
    queueChanged();
}

void MainWindow::removeSelectedQueueItem()
{
    // 删除当前选中的阶段；没有选中时不做处理
    int currentRow = ui->queueListWidget->currentRow();
    if (currentRow < 0)
        return;

    delete ui->queueListWidget->takeItem(currentRow);
    queueChanged();
}

void MainWindow::clearQueueItems()
{
    ui->queueListWidget->clear();
    queueChanged();
}

void MainWindow::queueChanged()
{
    updateNextStageDisplay();
}

QString MainWindow::formatTime(int seconds) const
{
    int minutes = seconds / 60;
    int secs = seconds % 60;
    return QString("%1:%2").arg(minutes,2,10,QChar('0')).arg(secs,2,10,QChar('0'));
}

QString MainWindow::modeText(PomodoroClock::Mode mode) const
{
    if (mode == PomodoroClock::Mode::Work)
        return "工作时间";
    return "休息时间";
}

void MainWindow::updateNextStageDisplay()
{
    if (ui->queueListWidget->count() == 0) {
        ui->nextStageNameLabel->setText("无");
        ui->nextStageDurationLabel->setText("-分钟");
        return;
    }

    QListWidgetItem *firstItem = ui->queueListWidget->item(0);
    PomodoroClock::Mode nextMode =
        static_cast<PomodoroClock::Mode>(firstItem->data(QueueModeRole).toInt());
    int nextMinutes = firstItem->data(QueueMinutesRole).toInt();

    // 小窗口显示队首阶段，队列为空时显示“无”
    ui->nextStageNameLabel->setText(modeText(nextMode));
    ui->nextStageDurationLabel->setText(QString("%1 分钟").arg(nextMinutes));
}

void MainWindow::appendQueueItem(PomodoroClock::Mode mode, int minutes)
{
    QListWidgetItem *item = new QListWidgetItem(QString("%1 - %2 分钟").arg(modeText(mode)).arg(minutes));

    // 使用 UserRole 保存真实数据，显示文字改变时也不会丢失阶段类型和分钟数
    item->setData(QueueModeRole, static_cast<int>(mode));
    item->setData(QueueMinutesRole, minutes);
    // 队列项只允许被拖动，不把 item 本身作为 drop 目标，避免拖到队首时覆盖/吞掉项目
    Qt::ItemFlags flags = item->flags();
    flags |= Qt::ItemIsDragEnabled;
    flags &= ~Qt::ItemIsDropEnabled;
    item->setFlags(flags);

    ui->queueListWidget->addItem(item);
}

QRect MainWindow::scaledRect(const QRect &baseRect) const
{
    // 使用当前 centralwidget 尺寸和设计尺寸计算比例，让控件随窗口缩放
    double widthScale = ui->centralwidget->width() / static_cast<double>(BaseWidth);
    double heightScale = ui->centralwidget->height() / static_cast<double>(BaseHeight);

    return QRect(qRound(baseRect.x() * widthScale),
                 qRound(baseRect.y() * heightScale),
                 qRound(baseRect.width() * widthScale),
                 qRound(baseRect.height() * heightScale));
}

void MainWindow::scaleUiToWindow()
{
    // 这里的 QRect 是 .ui 文件中的原始位置；窗口变化时统一按比例缩放
    ui->appTitleLabel->setGeometry(scaledRect(QRect(259, 18, 240, 44)));

    ui->queueTitleLabel->setGeometry(scaledRect(QRect(20, 24, 81, 24)));
    ui->addQueueButton->setGeometry(scaledRect(QRect(20, 260, 54, 28)));
    ui->removeQueueButton->setGeometry(scaledRect(QRect(20, 300, 54, 28)));
    ui->clearQueueButton->setGeometry(scaledRect(QRect(20, 340, 54, 28)));
    ui->queueListWidget->setGeometry(scaledRect(QRect(20, 56, 181, 191)));

    ui->modeLabel->setGeometry(scaledRect(QRect(290, 85, 191, 61)));
    ui->timeLabel->setGeometry(scaledRect(QRect(230, 175, 311, 121)));

    ui->nextStagePanel->setGeometry(scaledRect(QRect(560, 190, 151, 91)));
    ui->nextStageTitleLabel->setGeometry(scaledRect(QRect(575, 199, 121, 23)));
    ui->nextStageNameLabel->setGeometry(scaledRect(QRect(575, 226, 121, 27)));
    ui->nextStageDurationLabel->setGeometry(scaledRect(QRect(575, 255, 121, 25)));

    ui->addMinutesButton->setGeometry(scaledRect(QRect(315, 321, 151, 41)));
    ui->startButton->setGeometry(scaledRect(QRect(120, 420, 151, 61)));
    ui->pauseButton->setGeometry(scaledRect(QRect(340, 420, 151, 61)));
    ui->resetButton->setGeometry(scaledRect(QRect(550, 420, 151, 61)));
    ui->exitButton->setGeometry(scaledRect(QRect(670, 570, 71, 30)));
}
