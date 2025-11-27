#include "summary_tab.h"

SummaryTab::SummaryTab(QWidget* parent)
    : QWidget(parent)
{
    m_layout = new QVBoxLayout(this);

    m_currStage = "";

    m_currentStage = new ExpandableWidget("Текущий этап", QString(), this);
    m_currStageWgt = new CurrentStageWgt(this);

    m_currentStage->setContentWidget(m_currStageWgt);

    m_finishedStages = new ExpandableWidget("Завершенные этапы", QString(), this);
    m_finishedStagesWgt = new FinishedStagesWgt(this);

    m_finishedStages->setContentWidget(m_finishedStagesWgt);

    m_stagesInQueue = new ExpandableWidget("Этапы в очереди", QString(), this);
    m_stagesQueueWgt = new StagesQueueWgt(this);

    m_stagesInQueue->setContentWidget(m_stagesQueueWgt);

    m_layout->addWidget(m_currentStage, Qt::AlignTop);
    m_layout->addWidget(m_finishedStages, Qt::AlignTop);
    m_layout->addWidget(m_stagesInQueue, Qt::AlignTop);

    m_currentStage->setExpanded(true);
    m_finishedStages->setExpanded(true);
    m_stagesInQueue->setExpanded(true);

    QPushButton* saveBtn = new QPushButton("Сохранить", this);

    saveBtn->setFixedWidth(width());
    m_layout->addWidget(saveBtn);

    connect(saveBtn, &QPushButton::clicked, this, &SummaryTab::saveFiles);

    connect(m_currStageWgt.get(), &CurrentStageWgt::onProgressBarFinished, [this]{
        m_finishedStagesWgt->addFinishedStage(m_currStage);
    });

    connect(m_stagesQueueWgt.get(), &StagesQueueWgt::onQueueEmpty, [this]{
        m_stagesInQueue->setExpanded(false);
        m_stagesInQueue->setDisabled(true);
    });
}

void SummaryTab::changeCurrentStage(const QString& str)
{
    m_currStage = str;
    m_currStageWgt->changeCurrentStage(m_currStage);
    m_stagesQueueWgt->nextStageRunning();
}

void SummaryTab::progressMaxChanged(long long max)
{
    if (max > INT_MAX)
    {
        m_currStageWgt->changeProgressBarMax(static_cast<int>(max / 100000));
    }
    else
        m_currStageWgt->changeProgressBarMax(max);
}

void SummaryTab::progressChanged(long long val, long long max)
{
    if (max > INT_MAX)
    {
        m_currStageWgt->changeProgressValue(static_cast<int>(val / 100000), static_cast<int>(max / 100000));
    }
    else
        m_currStageWgt->changeProgressValue(val, max);
}

CurrentStageWgt::CurrentStageWgt(QWidget* parent)
{
    m_layout = new QVBoxLayout(this);

    m_currStage = new QLabel("Текущая операция: ", this);

    m_progressBar = new QProgressBar(this);
    m_progressBar->setRange(0, 100);

    m_layout->addWidget(m_currStage);
    m_layout->addWidget(m_progressBar);

    connect(m_progressBar.get(), &QProgressBar::valueChanged, [this](int val){
        if (val == m_progressBar->maximum())
            emit onProgressBarFinished();
    });
}

void CurrentStageWgt::changeCurrentStage(const QString& str)
{
    if (str == "Завершено")
    {
        m_currStage->setText(str);
        return;
    }

    m_currStage->setText("Текущая операция: " + str);
}

void CurrentStageWgt::changeProgressBarMax(long long max)
{
    m_progressBar->setMaximum(max);
}

void CurrentStageWgt::changeProgressValue(long long val, long long max)
{
    m_progressBar->setMaximum(max);
    m_progressBar->setValue(val);
}

FinishedStagesWgt::FinishedStagesWgt(QWidget* parent)
{
    m_layout = new QVBoxLayout(this);

    m_label = new QLabel("Завершенные этапы: ", this);

    m_finishedStages = new QListWidget(this);
    m_finishedStages->setSelectionMode(QAbstractItemView::NoSelection);

    m_layout->addWidget(m_label);
    m_layout->addWidget(m_finishedStages);
}

void FinishedStagesWgt::addFinishedStage(const QString& str)
{
    if (str == "Завершено") return;
    m_finishedStages->addItem(str);
}

StagesQueueWgt::StagesQueueWgt(QWidget* parent)
{
    m_layout = new QVBoxLayout(this);

    m_label = new QLabel("Этапы в очереди: ", this);

    m_queueList = new QListWidget(this);
    m_queueList->setSelectionMode(QAbstractItemView::NoSelection);

    m_queueList->addItem("Сканирование архива");
    m_queueList->addItem("Сохранение файлов");
    m_queueList->addItem("Архивирование");

    m_layout->addWidget(m_label);
    m_layout->addWidget(m_queueList);
}

void StagesQueueWgt::nextStageRunning()
{
    if (m_queueList->count() > 0)
    {
        QListWidgetItem *firstItem = m_queueList->takeItem(0);
        delete firstItem;
    }
    else
    {
        emit onQueueEmpty();
    }
}
