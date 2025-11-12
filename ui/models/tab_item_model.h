#ifndef TAB_ITEM_MODEL_H
#define TAB_ITEM_MODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QTableView>
#include <QString>
#include <QScopedPointer>
#include <QVariant>

class TabItemModel : public QAbstractItemModel
{
public:
    explicit TabItemModel(QObject* parent = nullptr);
    ~TabItemModel() override;

    virtual int columnCount(const QModelIndex &index = QModelIndex()) const override;
    virtual int rowCount(const QModelIndex &index = QModelIndex()) const override;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    virtual QModelIndex parent(const QModelIndex &parent) const  override;

private:
    QScopedPointer<QVector<QVector<QString>>> m_data;
    QScopedPointer<QVector<QString>> m_headers;
};

#endif // TAB_ITEM_MODEL_H
