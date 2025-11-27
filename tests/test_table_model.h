#ifndef TESTTABLEMODEL_H
#define TESTTABLEMODEL_H

#include <QtTest>
#include <QSignalSpy>

#include "../src/ui/models/table_model.h"

class TestTableModel : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    void testAddFile();
    void testRowCount();
    void testColumnCount();
    void testDataDisplay();
    void testCheckboxOperations();
    void testGetFilesToKeep();
    void testClearFiles();

private:
    TableModel* model = nullptr;
};

#endif // TESTTABLEMODEL_H
