#include "files_tab.h"

FilesTab::FilesTab(QWidget *parent)
    : QWidget{parent}
{
    QVBoxLayout* layout = new QVBoxLayout(this);

    m_tabWgt = new TableView(this);
    TableModel* tableModel = new TableModel(m_tabWgt);
    m_tabWgt->setModel(tableModel);

    m_tabWgt->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    layout->addWidget(m_tabWgt);
}

void FilesTab::addAcceptibleFile(const QString& str, quint64 fileSize, QDateTime timestamp)
{
    m_tabWgt->tableModel()->addFileWithMetadata(str, fileSize, timestamp);
    m_tabWgt->update();
}
