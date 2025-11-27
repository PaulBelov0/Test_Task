#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

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
#include <QFileDialog>
#include <QMessageBox>
#include <QIcon>

#include <memory>

#include "../archive_core/archiver_pipeline.h"
#include "tab_items/files_tab.h"
#include "tab_items/summary_tab.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(const QString& zipPath, LaunchType lType, QWidget* parent = nullptr);
    void startZipProcessing();

private:
    QPointer<QTabWidget> m_mainWidget;
    QString m_zipPath;

    std::shared_ptr<ArchiverPipeline> m_archiverPipeline;
};
#endif // MAIN_WINDOW_H
