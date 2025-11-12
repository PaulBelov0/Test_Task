#ifndef TABLE_VIEW_FIELD_H
#define TABLE_VIEW_FIELD_H

#include <QWidget>
#include <QPointer>
#include <QTableView>
#include <QVBoxLayout>

class TableViewField : public QWidget
{
    Q_OBJECT
public:
    TableViewField(QWidget* parent = nullptr);

private:
    QPointer<QTableView> m_tableView;
};

#endif // TABLE_VIEW_FIELD_H
