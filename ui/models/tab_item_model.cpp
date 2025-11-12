#include "tab_item_model.h"

TabItemModel::TabItemModel(QObject* parent)
    : QAbstractItemModel(parent)
    , m_data(new QVector<QVector<QString>>())
    , m_headers(new QVector<QString>())
{}


TabItemModel::~TabItemModel()
{
    QAbstractItemModel::~QAbstractItemModel();
}

int TabItemModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_headers->size();
}

int TabItemModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return m_data->size();
}

QVariant TabItemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if (index.row() < m_data->size() && index.column() < m_data->at(index.row()).size())
            return m_data->at(index.row()).at(index.column());
    }

    return QVariant();
}

QModelIndex TabItemModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    return createIndex(row, column);
}

QModelIndex TabItemModel::parent(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return QModelIndex();
}

