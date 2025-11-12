#ifndef TREE_VIEW_FIELD_H
#define TREE_VIEW_FIELD_H

#include <QWidget>
#include <QPointer>
#include <QVBoxLayout>
#include <QTreeWidget>
#include <QTreeWidgetItem>

#include <list>
#include <string>

class TreeViewField : public QWidget
{
    Q_OBJECT
public:
    explicit TreeViewField(QString& path, QWidget *parent = nullptr);

private:
    void initTreeWidgetItems(QString& path);
    QString getArchiveName(QString& path);
private:
    QPointer<QTreeWidget> m_treeWidget;
};

#endif // TREE_VIEW_FIELD_H
