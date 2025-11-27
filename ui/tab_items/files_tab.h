#ifndef FILES_TAB_H
#define FILES_TAB_H

#include <QWidget>
#include <QTableView>
#include <QVBoxLayout>
#include <QPointer>
#include <QHeaderView>
#include <QDateTime>

#include "../models/table_model.h"
#include "../table_view.h"

class FilesTab : public QWidget
{
    Q_OBJECT
public:
    explicit FilesTab(QWidget *parent = nullptr);

    TableView* getTableView() { return m_tabWgt; }

public slots:
    void addAcceptibleFile(const QString& str, quint64 fileSize, QDateTime timestamp);
private:
    QPointer<TableView> m_tabWgt;
};

#endif // FILES_TAB_H
