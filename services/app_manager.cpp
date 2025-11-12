#include "app_manager.h"

AppManager::AppManager(QObject* parent)
    : QObject(parent)
{
    QFile file("config.json");
    if (!file.open(QIODevice::ReadOnly)) {
        qFatal() << "Ошибка открытия файла:" << file.errorString();
    }

    m_cfg = QSharedPointer<QJsonDocument>(new QJsonDocument(QJsonDocument::fromJson(file.readAll())));
    m_launchConfig = getConfiguration();

    m_archiverPipeline = QSharedPointer<ArchiverPipeline>::create(this);

    connect(this, &AppManager::onUserNullPath, [this]{setPath();});
}

int AppManager::start(int argc, char* argv[])
{
    if (m_launchConfig == LaunchConfig::ERR)
        return -1;

    if (m_launchConfig == LaunchConfig::Gui)
    {
        QApplication a(argc, argv);
        setPath();

        MainWindow w(m_archiverPipeline.get(), m_path);
        w.show();

        return a.exec();
    }
    else if (m_launchConfig == LaunchConfig::Cmd)
    {
        QCoreApplication a(argc, argv);

        TerminalCore* terminalCore = new TerminalCore(m_archiverPipeline.get(), &a);

        connect(terminalCore, &TerminalCore::finished, &a, &QCoreApplication::quit);

        QTimer::singleShot(0, terminalCore, &TerminalCore::start);

        return a.exec();
    }
    else
        return 0;
}

void AppManager::setPath()
{
    QString pathToFile = "";

#ifdef __linux__

    char* username = getenv("USER");
    if (!username)
    {
        username = getenv("USERNAME");
    }

    pathToFile = QFileDialog::getOpenFileName(nullptr, "Select file", "/home/" + QString(username), "*.zip");

#elif __WIN32
    PWSTR pszPath = nullptr;
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_Documents, 0, NULL, &pszPath);

    if (SUCCEEDED(hr))
    {
        QString documentsPath = QString::fromWCharArray(pszPath);
        CoTaskMemFree(pszPath);

        pathToFile = QFileDialog::getOpenFileName(nullptr, "Select file", documentsPath, "*.zip");
        qDebug() << pathToFile;
    }
    else
    {
        qWarning() << "Failed to get Documents folder path. HRESULT:" << hr;
        pathToFile = QFileDialog::getOpenFileName(nullptr, "Select file", "C:\\", "*.zip");
        qDebug() << pathToFile;
    }
#else

    QMessageBox msg;
    msg.setText("Error! Unsupported System");
    msg.exec();
    return 0;

#endif


    if (pathToFile == "")
    {
        emit onUserNullPath();
        return;
    }
    m_path = pathToFile;
}

LaunchConfig AppManager::getConfiguration()
{
    const QString cdgFile = "config.json";

    QFile file(cdgFile);
    if (!file.open(QIODevice::ReadOnly)) {
        qCritical() << "File open Error:" << file.errorString();
    }

    QJsonObject root = m_cfg->object();
    QJsonObject params = root["launch_params"].toObject();

    bool guiEnabled = params["Gui"].toBool();

    if (guiEnabled)
        return LaunchConfig::Gui;

    bool cmdEnabled = params["Cmd"].toBool();

    if (cmdEnabled)
        return LaunchConfig::Cmd;

    bool testEnabled = params["Test"].toBool();

    if (testEnabled)
        return LaunchConfig::Tests;

    return LaunchConfig::ERR;
}
