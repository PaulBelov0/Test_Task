#ifndef SUMMARY_TAB_H
#define SUMMARY_TAB_H

#include <QWidget>
#include <QTabWidget>
#include <QVBoxLayout>
#include <QListView>
#include <QPointer>
#include <QProgressBar>
#include <QListWidget>

#include "../widgets/expandable_widget.h"

////////////////////////////////////////////////////////////////
/// \brief The CurrentStageWgt class
///

class CurrentStageWgt : public QWidget
{
    Q_OBJECT
public:
    explicit CurrentStageWgt(QWidget* parent = nullptr);
signals:
    void onProgressBarFinished();

public slots:
    void changeCurrentStage(const QString& str);
    void changeProgressValue(long long val, long long max);
    void changeProgressBarMax(long long max);


private:
    QPointer<QLabel> m_currStage;
    QPointer<QProgressBar> m_progressBar;
    QPointer<QVBoxLayout> m_layout;
};

////////////////////////////////////////////////////////////////
/// \brief The FinishedStagesWgt class
///

class FinishedStagesWgt : public QWidget
{
    Q_OBJECT
public:
    explicit FinishedStagesWgt(QWidget* parent = nullptr);

public slots:
    void addFinishedStage(const QString& str);

private:
    QPointer<QLabel> m_label;
    QPointer<QListWidget> m_finishedStages;
    QPointer<QVBoxLayout> m_layout;
};

////////////////////////////////////////////////////////////////
/// \brief The StagesQueueWgt class
///

class StagesQueueWgt : public QWidget
{
    Q_OBJECT

public:
    explicit StagesQueueWgt(QWidget* parent = nullptr);
signals:
    void onQueueEmpty();

public slots:
    void nextStageRunning();

private:
    QPointer<QLabel> m_label;
    QPointer<QListWidget> m_queueList;
    QPointer<QVBoxLayout> m_layout;
};

////////////////////////////////////////////////////////////////
/// \brief The SummaryTab class
///

class SummaryTab : public QWidget
{
    Q_OBJECT
public:
    SummaryTab(QWidget* parent = nullptr);

signals:
    void onCurrentStage(const QString& stage);
    void saveFiles();

public slots:
    void changeCurrentStage(const QString& str);
    void progressChanged(long long val, long long max);
    void progressMaxChanged(long long max);

private:
    QPointer<QVBoxLayout> m_layout;

    QPointer<ExpandableWidget> m_currentStage;
    QPointer<ExpandableWidget> m_finishedStages;
    QPointer<ExpandableWidget> m_stagesInQueue;

    QPointer<CurrentStageWgt> m_currStageWgt;
    QPointer<FinishedStagesWgt> m_finishedStagesWgt;
    QPointer<StagesQueueWgt> m_stagesQueueWgt;

    QString m_currStage;
};

#endif //SUMMARY_TAB_H
