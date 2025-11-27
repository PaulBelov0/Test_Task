#include "unit_test_runner.h"
#include "test_archive_manager.h"
#include "test_table_model.h"
#include "test_archiver_pipeline.h"
#include <QTest>
#include <QDebug>

UnitTestRunner::UnitTestRunner()
{
}

bool UnitTestRunner::runAllTests()
{
    qDebug() << "=== STARTING UNIT TESTS ===";

    int failedCount = 0;

    failedCount += runArchiveManagerTests() ? 0 : 1;
    failedCount += runTableModelTests() ? 0 : 1;
    failedCount += runArchiverPipelineTests() ? 0 : 1;

    qDebug() << "=== UNIT TESTS COMPLETED ===";
    qDebug() << "Failed tests:" << failedCount;

    return failedCount == 0;
}

bool UnitTestRunner::runArchiveManagerTests()
{
    qDebug() << "Running ArchiveManager tests...";
    TestArchiveManager test;
    return QTest::qExec(&test) == 0;
}

bool UnitTestRunner::runTableModelTests()
{
    qDebug() << "Running TableModel tests...";
    TestTableModel test;
    return QTest::qExec(&test) == 0;
}

bool UnitTestRunner::runArchiverPipelineTests()
{
    qDebug() << "Running ArchiverPipeline tests...";
    TestArchiverPipeline test;
    return QTest::qExec(&test) == 0;
}
