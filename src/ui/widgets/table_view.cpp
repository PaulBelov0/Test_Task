#include "table_view.h"

TableView::TableView(QWidget* parent)
    : QTableView(parent)
{
    setSortingEnabled(true);
    sortByColumn(2, Qt::AscendingOrder);
    sortByColumn(3, Qt::AscendingOrder);
    sortByColumn(4, Qt::AscendingOrder);

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QTableView::customContextMenuRequested,
            [this](const QPoint &pos) {
                QModelIndex index = indexAt(pos);
                if (index.isValid()) {
                    qDebug() << "Right click on row:" << index.row();

                    QMenu menu;
                    menu.addAction("Выбрать все", [this, index]() {
                        tableModel()->setAllItemsChecked();
                    });
                    menu.exec(viewport()->mapToGlobal(pos));
                }
            });
}

TableModel* TableView::tableModel()
{
    return dynamic_cast<TableModel*>(model());
}

void TableView::onDoubleClicked(const QModelIndex& index)
{
    if (TableModel* model = tableModel())
    {
        QVariant filePath = model->data(model->index(index.row(), TableModel::PathColumn), Qt::EditRole);
        if (filePath.isValid())
        {
            emit fileDoubleClicked(filePath.toString());
        }
    }
}


