#ifndef ARCHIVE_MANAGER_H
#define ARCHIVE_MANAGER_H

#include <string>

#include <QObject>
#include <QFile>
#include <QString>
#include <QDataStream>
#include <QList>
#include <QByteArray>
#include <QDebug>
#include <QDir>
#include <QRegularExpression>
#include <QCoreApplication>

class ArchiveManager : public QObject
{
    Q_OBJECT

public:
    explicit ArchiveManager(QString path, QObject* parent = nullptr);

    explicit ArchiveManager(QObject* parent = nullptr);

    ArchiveManager(QString filePath, QString saveDir, QObject *parent = nullptr);

    void setPath(const QString& path);
    void setSaveDir(const QString& dir);

    /**
     * @brief Основной метод обработки ZIP-архива.
     * @return true, если целевое слово найдено хотя бы в одном файле.
     */
    bool processZip();

signals:
    void onSaveDirectorySet();
    void fileFound(const QString& filename);
    void finished();

private:
    /**
     * @brief Вспомогательная функция для безопасного чтения 16-битного
     * целого числа в формате Little Endian из QByteArray.
     */
    quint16 readInt16(const QByteArray& data, int offset);

    /**
     * @brief Сохраняет содержимое файла в указанную директорию.
     * Реализует базовую очистку имени файла для безопасности.
     */
    bool saveFile(const QString& filename, const QByteArray& content);

    QString m_targetWord;
    QString m_path;
    QString m_saveDir;
};


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
#pragma pack(pop)

struct ZipEntry
{
    QString filename;
    bool isCompressed;
    quint32 compressedSize;
    quint32 uncompressedSize;
    quint32 localHeaderOffset;
};

#endif // ARCHIVE_MANAGER_H
