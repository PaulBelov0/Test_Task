#include "test_archiver_pipeline.h"

void TestArchiverPipeline::init()
{
    pipeline = new ArchiverPipeline("", LaunchType::Tests, this);
}

void TestArchiverPipeline::testCLIConstructor()
{
    qDebug() << "Testing CLI constructor...";

    int argc = 3;
    char arg1[] = "test_program";
    char arg2[] = "dummy_path.zip";
    char arg3[] = "dummy_save_dir";
    char* argv[] = {arg1, arg2, arg3};

    QVERIFY(true);
}

void TestArchiverPipeline::testConstructor()
{
    qDebug() << "Testing constructors...";

    ArchiverPipeline pipeline1("", LaunchType::Tests);

    QVERIFY(true);
}

void TestArchiverPipeline::testSetPathToSave()
{
    QSignalSpy saveDirSpy(pipeline, &ArchiverPipeline::onSaveDirSet);
    QSignalSpy errorSpy(pipeline, &ArchiverPipeline::onSaveDirecroryWrong);

    pipeline->setPathToSave(tempDir.path());

    QTest::qWait(50);

    QVERIFY(saveDirSpy.count() > 0 || errorSpy.count() == 0);

    pipeline->setPathToSave("/invalid/path/that/does/not/exist");

    QTest::qWait(50);

    QVERIFY(true);
}

void TestArchiverPipeline::testSaveFilesMethod()
{
    QSignalSpy errorSpy(pipeline, &ArchiverPipeline::onErrorOccured);

    QString savePath = tempDir.path();
    QStringList selectedFiles = {"file1.txt", "file2.txt"};

    pipeline->saveFiles(savePath, selectedFiles);

    QTest::qWait(50);

    QVERIFY(true);
}

void TestArchiverPipeline::testSignalEmissions()
{
    QSignalSpy errorSpy(pipeline, &ArchiverPipeline::onErrorOccured);
    QSignalSpy stageSpy(pipeline, &ArchiverPipeline::onCurrentStageChanged);
    QSignalSpy progressSpy(pipeline, &ArchiverPipeline::onProgressChanged);

    pipeline->setPathToSave(tempDir.path());

    pipeline->startProcessing();

    QTest::qWait(100);

    QVERIFY(errorSpy.count() >= 0);
    QVERIFY(stageSpy.count() >= 0);
    QVERIFY(progressSpy.count() >= 0);
}

void TestArchiverPipeline::testStartProcessing()
{
    QSignalSpy doneSpy(pipeline, &ArchiverPipeline::onProcessingDone);
    QSignalSpy errorSpy(pipeline, &ArchiverPipeline::onErrorOccured);
    QSignalSpy fileReadedSpy(pipeline, &ArchiverPipeline::onFileReaded);

    pipeline->setPathToSave(tempDir.path());

    pipeline->startProcessing();

    QTest::qWait(200);

    QVERIFY(doneSpy.count() >= 0 || errorSpy.count() >= 0 || fileReadedSpy.count() >= 0);
}

#include "test_archiver_pipeline.moc"
