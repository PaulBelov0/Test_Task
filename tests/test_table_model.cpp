#include "test_table_model.h"

void TestTableModel::init()
{
    model = new TableModel(this);
}

void TestTableModel::cleanup()
{
    delete model;
    model = nullptr;
}

void TestTableModel::testAddFile()
{
    int initialRowCount = model->rowCount();

    model->addFileWithMetadata("/test/file.txt", 1024, QDateTime::currentDateTime());

    QCOMPARE(model->rowCount(), initialRowCount + 1);
}

void TestTableModel::testRowCount()
{
    QCOMPARE(model->rowCount(), 0);

    model->addFileWithMetadata("/test/file1.txt", 1024, QDateTime::currentDateTime());
    QCOMPARE(model->rowCount(), 1);

    model->addFileWithMetadata("/test/file2.txt", 2048, QDateTime::currentDateTime());
    QCOMPARE(model->rowCount(), 2);
}

void TestTableModel::testColumnCount()
{
    QCOMPARE(model->columnCount(), 5);
}

void TestTableModel::testDataDisplay()
{
    model->addFileWithMetadata("/test/path/file.txt", 1024, QDateTime::currentDateTime());

    QModelIndex nameIndex = model->index(0, TableModel::NameColumn);
    QVariant nameData = model->data(nameIndex, Qt::DisplayRole);

    QCOMPARE(nameData.toString(), "file.txt");
}

void TestTableModel::testCheckboxOperations()
{
    model->addFileWithMetadata("/test/file.txt", 1024, QDateTime::currentDateTime());

    QModelIndex checkboxIndex = model->index(0, TableModel::KeepColumn);

    bool setResult = model->setData(checkboxIndex, Qt::Checked, Qt::CheckStateRole);
    QVERIFY(setResult);

    QVariant checkState = model->data(checkboxIndex, Qt::CheckStateRole);
    QCOMPARE(checkState.toInt(), static_cast<int>(Qt::Checked));
}

void TestTableModel::testGetFilesToKeep()
{
    model->addFileWithMetadata("/test/file1.txt", 1024, QDateTime::currentDateTime());
    model->addFileWithMetadata("/test/file2.txt", 2048, QDateTime::currentDateTime());

    QModelIndex checkboxIndex = model->index(0, TableModel::KeepColumn);
    model->setData(checkboxIndex, Qt::Checked, Qt::CheckStateRole);

    QStringList filesToKeep = model->getFilesToKeep();
    QCOMPARE(filesToKeep.size(), 1);
    QCOMPARE(filesToKeep[0], QString("/test/file1.txt"));
}

void TestTableModel::testClearFiles()
{
    model->addFileWithMetadata("/test/file1.txt", 1024, QDateTime::currentDateTime());
    model->addFileWithMetadata("/test/file2.txt", 2048, QDateTime::currentDateTime());

    QCOMPARE(model->rowCount(), 2);

    model->clearFiles();

    QCOMPARE(model->rowCount(), 0);
}

#include "test_table_model.moc"
