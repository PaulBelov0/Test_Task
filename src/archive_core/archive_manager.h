#ifndef ARCHIVE_MANAGER_H
#define ARCHIVE_MANAGER_H

#include <string>
#include <cstdlib>
#include <chrono>

#include <QObject>
#include <QFile>
#include <QString>
#include <QDataStream>
#include <QList>
#include <QByteArray>
#include <QDebug>
#include <QDir>
#include <QStorageInfo>
#include <QRegularExpression>
#include <QCoreApplication>
#include <QApplication>
#include <QTemporaryDir>
#include <QProcess>
#include <QtEndian>
#include <qminmax.h>
#include <QRegularExpression>
#include <QStorageInfo>
#include <QMessageBox>
#include <QThread>
#include <QThreadPool>
#include <QtConcurrent/QtConcurrent>
#include <QAtomicInt>

#pragma pack(push, 1)  // Выравнивание 1 байт
struct ZipLocalFileHeader {
    uint32_t signature;       // 0x04034B50
    uint16_t version;
    uint16_t flags;
    uint16_t compression;
    uint16_t modTime;
    uint16_t modDate;
    uint32_t crc32;
    uint32_t compressedSize;
    uint32_t uncompressedSize;
    uint16_t fileNameLength;
    uint16_t extraFieldLength;
};

struct ZipEntry
{
    QString filename;
    quint32 size;
    qint64 dataOffset;
    uint32_t crc32;
};
#pragma pack(pop)


enum class LaunchType
{
    GUI,
    CLI,
    Tests
};

class ArchiveManager : public QObject
{
    Q_OBJECT

public:
    ArchiveManager(QString path, LaunchType lType, QObject* parent = nullptr);

    explicit ArchiveManager(LaunchType lType, QObject* parent = nullptr);

    ArchiveManager(QString filePath, QString saveDir, QObject *parent = nullptr);

    void setPath(const QString& path);
    void setSaveDir(const QString& dir);

    bool processZip();

signals:
    void onSaveDirectorySet();
    void fileFound(const QString& filename);

// Signals for GUI
    void onProgressChanged(int, qsizetype);
    void onAcceptibleFileAdded(const QString& str, quint64 fileSize, QDateTime timestamp);
    void onCurrentStageChanged(const QString& str);
    void onStageFinished(const QString& str);
    void onProgressGuiChanged(long long val, long long max);
    void onProgressMaxValueChanged(long long max);

    void onErrorOccured(const QString& err);

public slots:
    bool saveFiles(const QString&  saveDirPath, const QStringList& selectedFiles);
    bool saveFiles(const QString&  saveDirPath);
    void cancelProcessing() { m_shouldStop = true; }

    bool compressDirectory();
private:
    bool saveFilteredFilesToTemp(QTemporaryDir& dir, const QStringList& selectedFiles);
    bool saveFilteredFilesDirectToZip(const QStringList& selectedFiles, const QString& outputPath);
    inline QDateTime convertDosDateTime(uint16_t modDate, uint16_t modTime);

    bool m_shouldStop = false;

    QString m_targetWord;
    QString m_path;
    QString m_saveDir;
    QString m_folderName;
    QString m_compressingPath;

    LaunchType m_launch;
    QFile m_file;
    QMap<QString, std::pair<quint64, quint64>> m_fileMap; // filename | position, size
};

#endif // ARCHIVE_MANAGER_H
