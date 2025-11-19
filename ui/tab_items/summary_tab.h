#ifndef SUMMARY_TAB_H
#define SUMMARY_TAB_H

#include <QWidget>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QListView>
#include <QPointer>

#include "../expandable_widget.h"

class SummaryTab : public QWidget
{
    Q_OBJECT
public:
    SummaryTab(QWidget* parent = nullptr);

signals:
    void onCurrentStage(const QString& stage);

private:
    QPointer<QVBoxLayout> m_layout;
};

#endif //SUMMARY_TAB_H
