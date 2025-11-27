#ifndef ARCHIVER_PIPELINE_H
#define ARCHIVER_PIPELINE_H

#include <iostream>
#include <string>
#include <filesystem>
#include <tuple>

#include <QObject>
#include <QScopedPointer>

#include "archive_manager.h"

class ArchiverPipeline : public QObject
{
    Q_OBJECT
public:
    explicit ArchiverPipeline(QString zipPath, LaunchType lType, QObject* parent = nullptr);
    explicit ArchiverPipeline(int argc, char* argv[], LaunchType lType, QObject* parent = nullptr);

    void startProcessing();

signals:
    void onFileDirectoryWrong();
    void onSaveDirecroryWrong();
    void onFileReaded();
    void onFileSaved();
    void onAcceptibleFileDetected(const QString& str, quint64 fileSize, QDateTime timestamp);
    void onProcessingDone();
    void onSaveDirSet();

    //For GUI
    void onCurrentStageChanged(const QString& str);
    void onProgressChanged(long long val, long long max);
    void onProgressMaxChanged(long long max);
    void onErrorOccured(const QString& err);

public slots:
    void setPathToSave(const QString& path);
    bool saveFiles(const QString& path, const QStringList& selectedFiles);

private:
    bool checkPathToSave(const QString& path);
    bool isZipFile(const std::filesystem::path& path);
    bool checkPathToRead(const QString& path);

private:
    QScopedPointer<ArchiveManager> m_archive;
    QString m_pathSource;
    QString m_pathToSave;
};

#endif // ARCHIVER_PIPELINE_H
