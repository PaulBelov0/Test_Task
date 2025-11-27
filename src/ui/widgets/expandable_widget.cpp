#include "expandable_widget.h"

ExpandableWidget::ExpandableWidget(const QString& title, const QString& additionalStr, QWidget *parent)
    : QWidget(parent)
    , m_title(title)
    , m_expanded(false)
    , m_contentHeight(0)
    , m_additionalStr(additionalStr)
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);
    m_mainLayout->setSpacing(0);

    m_toggleButton = new QPushButton();

    m_toggleButton->setText("► " + m_title + " " + additionalStr);

    m_toggleButton->setStyleSheet(R"(
        QPushButton {
            text-align: left;
            padding: 8px;
            font-weight: bold;
        }
        )");
    m_toggleButton->setCheckable(true);

    m_scrollArea = new QScrollArea(this);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setFrameShape(QFrame::NoFrame);
    m_scrollArea->setMaximumHeight(0);
    m_scrollArea->setMinimumHeight(0);

    m_contentWidget = new QWidget(this);
    m_scrollArea->setWidget(m_contentWidget);

    m_animation = new QPropertyAnimation(m_scrollArea, "maximumHeight");
    m_animation->setDuration(300);
    m_animation->setEasingCurve(QEasingCurve::InOutQuad);

    m_mainLayout->addWidget(m_toggleButton);
    m_mainLayout->addWidget(m_scrollArea);

    connect(m_toggleButton, &QPushButton::toggled, this, &ExpandableWidget::toggleExpansion);
}

void ExpandableWidget::setContentWidget(QWidget *content)
{
    QPointer<QVBoxLayout> contentLayout = new QVBoxLayout(m_contentWidget);
    contentLayout->setContentsMargins(10, 5, 10, 5);
    contentLayout->addWidget(content);

    QTimer::singleShot(0, this, [this]() {
        m_contentHeight = m_contentWidget->sizeHint().height();
        updateAnimation();
    });
}

void ExpandableWidget::setExpanded(bool expanded)
{
    m_expanded = expanded;
    m_toggleButton->setChecked(expanded);
    updateAnimation();
}

void ExpandableWidget::toggleExpansion()
{
    m_expanded = m_toggleButton->isChecked();
    m_toggleButton->setText(m_expanded ? "▼ " + m_title : "► " + m_title);
    updateAnimation();
}

void ExpandableWidget::updateAnimation()
{
    if (m_expanded)
    {
        m_animation->setStartValue(0);
        m_animation->setEndValue(m_contentHeight);
        m_toggleButton->setText("▼ " + m_title);
    }
    else
    {
        m_animation->setStartValue(m_contentHeight);
        m_animation->setEndValue(0);
        m_toggleButton->setText("► " + m_title);
    }
    m_animation->start();
}

int ExpandableWidget::getContentHeight() const
{
    return m_contentHeight;
}

void ExpandableWidget::setContentHeight(int height)
{
    m_contentHeight = height;
}

void ExpandableWidget::changeAdditionalStr(const QString& str)
{
    m_additionalStr = str;
    emit onAdditionalStrChanged(str);
}
