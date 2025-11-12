#include "tree_view_field.h"

TreeViewField::TreeViewField(QString& path, QWidget *parent)
    : QWidget{parent}
{
    m_treeWidget = new QTreeWidget(this);
    m_treeWidget->setHeaderHidden(true);

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_treeWidget);

    initTreeWidgetItems(path);
}

QString TreeViewField::getArchiveName(QString& path)
{
    std::list<QChar> charName;
    QString result = "";

    for (qsizetype i = path.length() - 1; i >= 0; --i)
    {
        if (path.at(i) != '/')
            charName.push_front(path.at(i));
        else
            break;
    }

    for (std::list<QChar>::iterator it = charName.begin(); it != charName.end(); ++it)
        result += it->mirroredChar();

    return result;
}

void TreeViewField::initTreeWidgetItems(QString& path)
{
    QTreeWidgetItem* archiveItem = new QTreeWidgetItem(m_treeWidget);
    archiveItem->setText(0, "Архив: " + getArchiveName(path));
    archiveItem->setText(1, "Поиск начат.");
    archiveItem->setExpanded(true);

    QTreeWidgetItem* sourcesItem = new QTreeWidgetItem(archiveItem);
    sourcesItem->setText(0, "Источники");

    QTreeWidgetItem* disksItem = new QTreeWidgetItem(archiveItem);
    disksItem->setText(0, "Диски и разделы");

    QTreeWidgetItem* resultItem = new QTreeWidgetItem(m_treeWidget);
    resultItem->setText(0, "Результаты поиска");
    resultItem->setExpanded(true);

    QTreeWidgetItem* fileInfoItem = new QTreeWidgetItem(resultItem);
    fileInfoItem->setFirstColumnSpanned(true);
    fileInfoItem->setText(0, "Файлы");
    fileInfoItem->setText(1, "Описание дочернего элемента 2");

    m_treeWidget->addTopLevelItem(archiveItem);
}
