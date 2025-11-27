#include "table_view.h"

TableView::TableView(QWidget* parent)
    : QTableView(parent)
{
    setSortingEnabled(true);
    sortByColumn(2, Qt::AscendingOrder);
    sortByColumn(3, Qt::AscendingOrder);
    sortByColumn(4, Qt::AscendingOrder);
}

TableModel* TableView::tableModel()
{
    return dynamic_cast<TableModel*>(QTableView::model());
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
