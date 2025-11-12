#include "table_view_field.h"

TableViewField::TableViewField(QWidget* parent)
    : QWidget(parent)
{
    m_tableView = new QTableView(this);
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_tableView);
}
