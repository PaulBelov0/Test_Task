#include "ui/mainwindow.h"
#include "archive_core/archiver_pipeline.h"

#include <QApplication>
#include <QIODevice>
#include <QJsonObject>
#include <QFile>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QFile file = QFile(":/config");
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Ошибка открытия файла:" << file.errorString();
    }

    QJsonDocument cfg = QJsonDocument::fromJson(file.readAll());

    QJsonObject root = cfg.object();
    QJsonObject params = root["launch_params"].toObject();

    bool guiEnabled = params["Gui"].toBool();
    bool cmdEnabled = params["Cmd"].toBool();
    bool testEnabled = params["Test"].toBool();

    if (guiEnabled)
    {
        QApplication a(argc, argv);
        MainWindow w;
        w.show();
        return a.exec();
    }
    else if (cmdEnabled)
    {
        QCoreApplication a(argc, argv);
        ArchiverPipeline arhc(argc, argv);

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
        MainWindow w;
        w.show();
        return a.exec();
    }
    return 0;
}
