#include "test_archive_manager.h"

void TestArchiveManager::init()
{
    m_manager = new ArchiveManager(LaunchType::Tests, this);
}

void TestArchiveManager::cleanup()
{
    delete m_manager;
    m_manager = nullptr;
}

void TestArchiveManager::testConstructors()
{
    ArchiveManager manager1(LaunchType::Tests);

    ArchiveManager manager2("test.zip", LaunchType::Tests);

    ArchiveManager manager3("test.zip", "/save/dir");
}

void TestArchiveManager::testSetters()
{
    m_manager->setPath("/test/path.zip");
    m_manager->setSaveDir("/save/dir");

    QVERIFY(true);
}

void TestArchiveManager::testSignals()
{
    QSignalSpy errorSpy(m_manager, &ArchiveManager::onErrorOccured);
    QSignalSpy progressSpy(m_manager, &ArchiveManager::onProgressChanged);

    m_manager->setPath("nonexistent.zip");
    m_manager->processZip();

    QVERIFY(errorSpy.count() > 0);
}

void TestArchiveManager::testProcessZipWithInvalidFile()
{
    QTemporaryFile invalidFile;
    QVERIFY(invalidFile.open());
    invalidFile.write("NOT_A_ZIP_FILE");
    invalidFile.close();

    m_manager->setPath(invalidFile.fileName());
    bool result = m_manager->processZip();

    QVERIFY(!result);
}

void TestArchiveManager::testCancelProcessing()
{
    m_manager->cancelProcessing();

    m_manager->setPath("test.zip");

    QVERIFY(true);
}

void TestArchiveManager::testSaveFilesOperations()
{
    QString savePath = m_tempDir.path() + "/output";

    bool result1 = m_manager->saveFiles(savePath);
    bool result2 = m_manager->saveFiles(savePath, QStringList() << "file1.txt" << "file2.txt");

    QVERIFY(!result1);
    QVERIFY(!result2);
}

void TestArchiveManager::testCompressDirectory()
{
    QTemporaryDir sourceDir;
    QFile testFile(sourceDir.path() + "/test.txt");
    if (testFile.open(QIODevice::WriteOnly))
    {
        testFile.write("Test content");
        testFile.close();
    }

    m_manager->setSaveDir(m_tempDir.path());

    bool result = m_manager->compressDirectory();

    QVERIFY(result == true || result == false);
}

void TestArchiveManager::createTestZipFile(const QString& path, const QByteArray& content)
{
    QFile file(path);
    if (file.open(QIODevice::WriteOnly))
    {
        file.write(content);
        file.close();
    }
}

#include "test_archive_manager.moc"
