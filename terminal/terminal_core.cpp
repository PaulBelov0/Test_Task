#include "terminal_core.h"

TerminalCore::TerminalCore(ArchiverPipeline* archiverPipeline, QObject *parent)
    : QObject(parent)
    , m_terminalProcess(new QProcess(this))
{
    connect(this, &TerminalCore::onReadPathReady, archiverPipeline, &ArchiverPipeline::setPathToRead);
    connect(this, &TerminalCore::onSavePathReady, archiverPipeline, &ArchiverPipeline::setPathToSave);

    connect(archiverPipeline, &ArchiverPipeline::onFileDirectoryWrong, this, &TerminalCore::setupPathToRead);
    connect(archiverPipeline, &ArchiverPipeline::onSaveDirecroryWrong, this, &TerminalCore::setupPathToSave);

    connect(archiverPipeline, &ArchiverPipeline::onProcessingDone, this, &TerminalCore::setupPathToSave);
}

int TerminalCore::start()
{
    forceTerminal();

    setupPathToRead();

    setupPathToSave();

    emit finished();
    return 0;
}

void TerminalCore::setupPathToSave()
{
    std::string path;
    std::cout << "Введите путь к пустой папке для сохранения файлов:    ";
    std::getline(std::cin, path);
    std::cout << "User input save:   " << path << std::endl;

    emit onSavePathReady(path);
}

void TerminalCore::setupPathToRead()
{
    std::string path;
    std::cout << "Введите путь к файлу '*.zip':    ";
    std::getline(std::cin, path);
    std::cout << "User input file:   " << path << std::endl;

    emit onReadPathReady(path);
}

void TerminalCore::forceTerminal()
{
#ifdef __WIN32__
    AllocConsole();

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    freopen("CONIN$", "r", stdin);
    freopen("CONOUT$", "w", stdout);
    freopen("CONOUT$", "w", stderr);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    qSetMessagePattern("%{file}(%{line}): %{message}");
#else
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
#endif

#elif __linux__
    if (!isatty(fileno(stdout)))
    {
        QString terminal = "x-terminal-emulator";

        QString command = QString("\"%1\"; bash -i").arg(QCoreApplication::applicationFilePath());

        m_terminalProcess->startDetached(terminal, {"-e", "bash", "-c", command});
    }
#endif
}
