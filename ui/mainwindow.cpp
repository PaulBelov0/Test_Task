#include "mainwindow.h"

MainWindow::MainWindow(QString zipPpath, LaunchType lType, QWidget *parent)
    : QMainWindow(parent)
{
    setMinimumSize(800, 600);

    m_zipPath = zipPpath;
    m_mainWidget = new QWidget(this);
    m_mainLayout = new QGridLayout(m_mainWidget);

    m_archiverPipeline = std::shared_ptr<ArchiverPipeline>(new ArchiverPipeline(m_zipPath, lType));
    m_archiverPipeline->startProcessing();

    setCentralWidget(m_mainWidget);
}

MainWindow::~MainWindow() {}
