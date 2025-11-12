#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>
#include <QTreeView>
#include <QScopedPointer>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QTableView>
#include <QSplitter>

#include "ui/widgets/tree_view_field.h"
#include "ui/widgets/table_view_field.h"
#include "services/archiver_pipeline.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(ArchiverPipeline* archiverPipeline, QString& path, QWidget *parent = nullptr);

signals:
    void resizeEvent(QResizeEvent* ev);
};
#endif // MAIN_WINDOW_H
