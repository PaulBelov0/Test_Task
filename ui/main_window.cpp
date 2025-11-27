#include "main_window.h"

MainWindow::MainWindow(const QString& zipPath, LaunchType lType, QWidget* parent)
    : QMainWindow(parent)
{
    setMinimumSize(800, 600);

    m_zipPath = zipPath;
    m_mainWidget = new QTabWidget(this);

    QTabBar* tabBar = m_mainWidget->tabBar();
    tabBar->setExpanding(true);
    tabBar->setMinimumHeight(40);

    SummaryTab* summaryTab = new SummaryTab(m_mainWidget);
    FilesTab* filesTab = new FilesTab(m_mainWidget);

    m_mainWidget->addTab(summaryTab, "Сводка");
    m_mainWidget->addTab(filesTab, "Файлы");

    m_archiverPipeline = std::shared_ptr<ArchiverPipeline>(new ArchiverPipeline(m_zipPath, lType));

    connect(m_archiverPipeline.get(), &ArchiverPipeline::onAcceptibleFileDetected, [filesTab, this](const QString& str, quint64 fileSize, QDateTime timestamp) {
        filesTab->addAcceptibleFile(str, fileSize, timestamp);
        });

    connect(summaryTab, &SummaryTab::saveFiles, [this, filesTab]{
        m_archiverPipeline->saveFiles(
            QFileDialog::getExistingDirectory(nullptr, "Выберете папку", QDir::homePath(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks)
            , filesTab->getTableView()->tableModel()->getFilesToKeep()
            );
    });

    connect(m_archiverPipeline.get(), &ArchiverPipeline::onCurrentStageChanged, [summaryTab](const QString& str){
        summaryTab->changeCurrentStage(str);
    });

    connect(m_archiverPipeline.get(), &ArchiverPipeline::onProgressMaxChanged, [summaryTab](long long max){
        summaryTab->progressMaxChanged(max);
    });

    connect(m_archiverPipeline.get(), &ArchiverPipeline::onProgressChanged, [summaryTab](long long val, long long max){
        summaryTab->progressChanged(val, max);
    });

    connect(m_archiverPipeline.get(), &ArchiverPipeline::onErrorOccured, [this](const QString& err){
        QMessageBox* msg = new QMessageBox(this);
        msg->setMinimumSize(300, 180);
        msg->setWindowTitle("Ошибка!");
        msg->setText(err);
        msg->exec();
        this->close();
    });

    setCentralWidget(m_mainWidget);
}

void MainWindow::startZipProcessing()
{
    show();
    m_archiverPipeline->startProcessing();
}
