#include "table_model.h"
#include <QFileInfo>
#include <QDateTime>
#include <algorithm>

TableModel::TableModel(QObject* parent)
    : QAbstractItemModel(parent)
{}

QModelIndex TableModel::index(int row, int column, const QModelIndex& parent) const
{
    return hasIndex(row, column, parent) ? createIndex(row, column) : QModelIndex();
}

QModelIndex TableModel::parent(const QModelIndex&) const { return QModelIndex(); }
int TableModel::rowCount(const QModelIndex&) const { return m_files.size(); }
int TableModel::columnCount(const QModelIndex&) const { return ColumnCount; }

QVariant TableModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= m_files.size())
        return QVariant();

    const QString& filePath = m_files[index.row()];
    const int col = index.column();

    switch (role) {
    case Qt::DisplayRole:
        switch (col) {
        case KeepColumn: return QVariant();
        case PathColumn: return fileDirectory(filePath);
        case NameColumn: return fileName(filePath);
        case SizeColumn: return formatSize(fileSize(filePath));
        case TimeColumn: return QDateTime(fileTimestamp(filePath));
        } break;

    case Qt::CheckStateRole:
        if (col == KeepColumn)
            return fileKeepState(filePath) ? Qt::Checked : Qt::Unchecked;
        break;

    case Qt::UserRole:
        switch (col) {
        case KeepColumn: return fileKeepState(filePath);
        case PathColumn: return filePath;
        case NameColumn: return fileName(filePath);
        case SizeColumn: return fileSize(filePath);
        case TimeColumn: return fileTimestamp(filePath);
        } break;
    }

    return QVariant();
}

bool TableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (index.isValid() && index.column() == KeepColumn && role == Qt::CheckStateRole) {
        const QString& filePath = m_files[index.row()];
        setFileKeepState(filePath, value.toInt() == Qt::Checked);
        emit dataChanged(index, index, {role});
        return true;
    }
    return false;
}

QVariant TableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
        static const QString headers[] = { "Сохранить", "Путь", "Имя", "Размер", "Время" };
        if (section < ColumnCount) return headers[section];
    }
    return QVariant();
}

Qt::ItemFlags TableModel::flags(const QModelIndex& index) const
{
    auto flags = QAbstractItemModel::flags(index);
    if (index.column() == KeepColumn) flags |= Qt::ItemIsUserCheckable;
    return flags;
}

void TableModel::addFileWithMetadata(const QString& filePath, quint64 fileSize, QDateTime timestamp)
{
    // Проверяем дубликаты
    if (m_files.contains(filePath)) return;

    beginInsertRows(QModelIndex(), m_files.size(), m_files.size());
    m_files.append(filePath);
    // m_keepFiles.insert(filePath);
    m_fileSizes.insert(filePath, fileSize);
    m_fileTimestamps.insert(filePath, timestamp);
    endInsertRows();
}

void TableModel::clearFiles()
{
    beginResetModel();
    m_files.clear();
    m_keepFiles.clear();
    endResetModel();
}

QStringList TableModel::getFilesToKeep() const
{
    QStringList result;
    for (const QString& filePath : m_files)
    {
        if (m_keepFiles.contains(filePath))
        {
            result.append(filePath);
        }
    }
    return result;
}



QString TableModel::fileName(const QString& filePath) const
{
    return QFileInfo(filePath).fileName();
}

QString TableModel::fileDirectory(const QString& filePath) const
{
    return QFileInfo(filePath).path();
}

qint64 TableModel::fileSize(const QString& filePath) const
{
    return m_fileSizes[filePath];
}

QDateTime TableModel::fileTimestamp(const QString& filePath) const
{
    return m_fileTimestamps[filePath];
}

bool TableModel::fileKeepState(const QString& filePath) const
{
    return m_keepFiles.contains(filePath);
}

void TableModel::setFileKeepState(const QString& filePath, bool keep)
{
    if (keep)
        m_keepFiles.insert(filePath);
    else
        m_keepFiles.remove(filePath);
}

QString TableModel::formatSize(qint64 bytes) const
{
    if (bytes == 0) return "0 B";
    if (bytes < 1024) return QString("%1 B").arg(bytes);
    else if (bytes < 1024*1024) return QString("%1 KB").arg(bytes/1024.0, 0, 'f', 1);
    else return QString("%1 MB").arg(bytes/(1024.0*1024.0), 0, 'f', 1);
}

void TableModel::sortByColumn(int column, Qt::SortOrder order)
{
    if (column < 0 || column >= ColumnCount) return;

    sort(column, order);
}

void TableModel::sort(int column, Qt::SortOrder order)
{
    beginResetModel();

    std::sort(m_files.begin(), m_files.end(), [this, column, order](const QString& a, const QString& b) {
        auto cmp = [this, column](const QString& a, const QString& b) {
            switch (column) {
            case KeepColumn: return fileKeepState(a) < fileKeepState(b);
            case PathColumn: return a < b;
            case NameColumn: return fileName(a) < fileName(b);
            case SizeColumn: return fileSize(a) < fileSize(b);
            case TimeColumn: return fileTimestamp(a) < fileTimestamp(b);
            default: return false;
            }
        };
        return order == Qt::AscendingOrder ? cmp(a, b) : cmp(b, a);
    });

    endResetModel();
}
