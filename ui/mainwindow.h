#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPointer>
#include <QTreeWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QVBoxLayout>

#include <memory>

#include "../archive_core/archiver_pipeline.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QString zipPath, LaunchType lType, QWidget *parent = nullptr);
    ~MainWindow();
private:
    QPointer<QWidget> m_mainWidget;
    QPointer<QGridLayout> m_mainLayout;
    QString m_zipPath;

    std::shared_ptr<ArchiverPipeline> m_archiverPipeline;
};
#endif // MAINWINDOW_H
