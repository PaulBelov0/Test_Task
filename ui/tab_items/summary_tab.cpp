#include "summary_tab.h"

SummaryTab::SummaryTab(QWidget* parent)
    : QWidget(parent)
{
    m_layout = new QVBoxLayout(this);

    ExpandableWidget* currentStage = new ExpandableWidget("Текущий этап", QString(), this);
    m_layout->addWidget(currentStage);

    ExpandableWidget* finishedStages = new ExpandableWidget("Завершенные этапы", QString(), this);
    m_layout->addWidget(finishedStages);

    ExpandableWidget* stagesInQueue = new ExpandableWidget("Этапы в очереди", QString(), this);
    m_layout->addWidget(stagesInQueue);
}
