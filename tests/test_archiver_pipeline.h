#ifndef TESTARCHIVERPIPELINE_H
#define TESTARCHIVERPIPELINE_H

#include <QtTest>
#include <QTemporaryDir>
#include <QSignalSpy>
#include <QCoreApplication>

#include "../src/archive_core/archiver_pipeline.h"

class TestArchiverPipeline : public QObject
{
    Q_OBJECT

private slots:
    void init();

    void testConstructor();
    void testCLIConstructor();
    void testSetPathToSave();
    void testSaveFilesMethod();
    void testSignalEmissions();
    void testStartProcessing();

private:
    ArchiverPipeline* pipeline = nullptr;
    QTemporaryDir tempDir;
};

#endif // TESTARCHIVERPIPELINE_H
