#ifndef TERMINAL_CORE_H
#define TERMINAL_CORE_H

#ifdef __linux__
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#endif

#ifdef __WIN32
#include <windows.h>
#include <shlwapi.h>
#include <shlobj.h>
#include <io.h>
#endif

#include <iostream>
#include <string>
#include <cstdlib>
#include <thread>
#include <chrono>

#include <QObject>
#include <QDebug>
#include <QDir>
#include <QProcess>
#include <QCoreApplication>
#include <QTextStream>

#include "services/archiver_pipeline.h"

class TerminalCore : public QObject
{
    Q_OBJECT
public:
    explicit TerminalCore(ArchiverPipeline* archiverPipeline, QObject *parent = nullptr);
    void forceTerminal();

signals:
    void finished();
    void onReadPathReady(std::string&);
    void onSavePathReady(std::string&);
    void onPathWrong();

public slots:
    int start();

private slots:
    void setupPathToSave();
    void setupPathToRead();

private:
    QScopedPointer<QProcess> m_terminalProcess;
};

#endif // TERMINAL_CORE_H
