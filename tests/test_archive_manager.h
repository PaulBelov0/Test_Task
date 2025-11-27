#ifndef TESTARCHIVEMANAGER_H
#define TESTARCHIVEMANAGER_H

#include <QtTest>
#include <QTemporaryFile>
#include <QTemporaryDir>
#include <QSignalSpy>

#include "../src/archive_core/archive_manager.h"

class TestArchiveManager : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void testConstructors();
    void testSetters();
    void testSignals();
    void testProcessZipWithInvalidFile();
    void testCancelProcessing();
    void testSaveFilesOperations();
    void testCompressDirectory();

private:
    ArchiveManager* m_manager = nullptr;
    QTemporaryDir m_tempDir;

    void createTestZipFile(const QString& path, const QByteArray& content);
};

#endif // TESTARCHIVEMANAGER_H
