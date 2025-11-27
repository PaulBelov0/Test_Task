#ifndef TABLE_MODEL_H
#define TABLE_MODEL_H

#include <QAbstractItemModel>
#include <QTimer>
#include <QStringList>
#include <QSet>

class TableModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    enum Column { KeepColumn, PathColumn, NameColumn, SizeColumn, TimeColumn, ColumnCount };

    explicit TableModel(QObject* parent = nullptr);

    // QAbstractItemModel interface start
    QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole) override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex& index) const override;
    void sort(int column, Qt::SortOrder order) override;
    //QAbstractItemModel interface end

    QStringList getFilesToKeep() const;
    QStringList getAllFiles() const { return m_files; }
    void sortByColumn(int column, Qt::SortOrder order = Qt::AscendingOrder);

    void setAllItemsChecked();

    bool isAllFilesChecked() { return m_isAllFilesSelected; };

public slots:
    void addFileWithMetadata(const QString& filePath, quint64 fileSize, QDateTime timestamp);
    void clearFiles();

private:
    QString fileName(const QString& filePath) const;
    QString fileDirectory(const QString& filePath) const;
    qint64 fileSize(const QString& filePath) const;
    QDateTime fileTimestamp(const QString& filePath) const;
    bool fileKeepState(const QString& filePath) const;
    void setFileKeepState(const QString& filePath, bool keep);
    QString formatSize(qint64 bytes) const;

private:
    QStringList m_files;
    QSet<QString> m_keepFiles;
    QHash<QString, quint64> m_fileSizes;      // filePath -> size
    QHash<QString, QDateTime> m_fileTimestamps;  // filePath -> timestamp

    bool m_isAllFilesSelected = false;
};

#endif // TABLE_MODEL_H
