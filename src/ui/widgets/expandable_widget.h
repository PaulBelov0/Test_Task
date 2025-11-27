#ifndef EXPANDABLE_WIDGET_H
#define EXPANDABLE_WIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QPropertyAnimation>
#include <QPointer>
#include <QLabel>
#include <QTimer>

class ExpandableWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(int contentHeight READ getContentHeight WRITE setContentHeight)

public:
    explicit ExpandableWidget(const QString& title, const QString& additionalStr,  QWidget *parent = nullptr);

    void setContentWidget(QWidget *content);
    void setExpanded(bool expanded);
    bool isExpanded() const { return m_expanded; }

private slots:
    void toggleExpansion();
    void changeAdditionalStr(const QString& str);

signals:
    void onAdditionalStrChanged(const QString& str);
private:

    int getContentHeight() const;
    void setContentHeight(int height);
    void updateAnimation();

    QPointer<QPushButton> m_toggleButton;
    QPointer<QWidget> m_contentWidget;
    QPointer<QScrollArea> m_scrollArea;
    QPointer<QPropertyAnimation> m_animation;
    QPointer<QVBoxLayout> m_mainLayout;
    QString m_title;
    bool m_expanded;
    int m_contentHeight;
    QString m_additionalStr;
};


#endif // EXPANDABLE_WIDGET_H
