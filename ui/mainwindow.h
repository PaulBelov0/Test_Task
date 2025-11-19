#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPointer>
#include <QTreeWidget>
#include <QPushButton>
#include <QTableWidget>
#include <QTabWidget>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QListView>

#include <memory>

#include "../archive_core/archiver_pipeline.h"
#include "tab_items/files_tab.h"
#include "tab_items/summary_tab.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QString& zipPath, LaunchType lType, QWidget* parent = nullptr);

private:
    QPointer<QTabWidget> m_mainWidget;
    QString m_zipPath;

    std::shared_ptr<ArchiverPipeline> m_archiverPipeline;
};
#endif // MAINWINDOW_H
