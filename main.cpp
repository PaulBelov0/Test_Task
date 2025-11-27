#include "ui/main_window.h"
#include "archive_core/archiver_pipeline.h"

#include <QApplication>
#include <QIODevice>
#include <QJsonObject>
#include <QFile>
#include <QCoreApplication>
#include <QFileDialog>

int main(int argc, char *argv[])
{
    QFile file = QFile(":/config");
    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Ошибка открытия файла:" << file.errorString();
    }

    QJsonDocument cfg = QJsonDocument::fromJson(file.readAll());

    QJsonObject root = cfg.object();
    QJsonObject params = root["launch_params"].toObject();

    bool cmdEnabled = params["Cmd"].toBool();
    bool testEnabled = params["Test"].toBool();

    if (cmdEnabled)
    {
        QCoreApplication a(argc, argv);
        ArchiverPipeline arhc(argc, argv, LaunchType::CLI);
        return a.exec();
    }
    else if (testEnabled)
    {
        QCoreApplication a(argc, argv);
        return a.exec();
    }
    else
    {
        QApplication a(argc, argv);
        MainWindow w(QFileDialog::getOpenFileName(nullptr, "Выберете .zip", QDir::homePath(), "Zip Files(*.zip)", nullptr), LaunchType::GUI, nullptr);
        w.show();
        QTimer::singleShot(100, &w, &MainWindow::startZipProcessing);
        return a.exec();
    }
    return 0;
}
