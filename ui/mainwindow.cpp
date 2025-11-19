#include "mainwindow.h"

MainWindow::MainWindow(const QString& zipPath, LaunchType lType, QWidget* parent)
    : QMainWindow(parent)
{
    setMinimumSize(800, 600);

    m_zipPath = zipPath;
    m_mainWidget = new QTabWidget(this);

    SummaryTab* summaryTab = new SummaryTab(m_mainWidget);
    FilesTab* filesTab = new FilesTab(m_mainWidget);

    m_mainWidget->addTab(summaryTab, "Сводка");
    m_mainWidget->addTab(filesTab, "Файлы");

    m_archiverPipeline = std::shared_ptr<ArchiverPipeline>(new ArchiverPipeline(m_zipPath, lType));
    m_archiverPipeline->startProcessing();

    setCentralWidget(m_mainWidget);
}
