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
    void onFileDetectedSuccessful();
    void onProcessingDone();
    void onSaveDirSet();

public slots:
    // void setPathToRead(const QString& path);
    void setPathToSave(const QString& path);
    void saveFile(const QString& path);

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
