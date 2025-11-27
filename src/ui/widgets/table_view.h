#ifndef TABLE_VIEW_H
#define TABLE_VIEW_H

#include <QTableView>
#include <QMouseEvent>
#include <QMenu>

#include "../models/table_model.h"

class TableView : public QTableView
{
    Q_OBJECT
public:
    explicit TableView(QWidget* parent = nullptr);

    TableModel* tableModel();

private slots:
    void onDoubleClicked(const QModelIndex& index);

signals:
    void fileDoubleClicked(const QString& filePath);
    void mousePressEvent(QMouseEvent* event);
};

#endif // TABLE_VIEW_H
