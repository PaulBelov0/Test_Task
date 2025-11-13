#include "archive_manager.h"
#include <QtEndian>


quint16 ArchiveManager::readInt16(const QByteArray& data, int offset) {
    if (offset + 2 > data.size()) return 0;

    return qFromLittleEndian<quint16>(reinterpret_cast<const uchar*>(data.constData()) + offset);
}
ArchiveManager::ArchiveManager(QString path, QObject* parent)
    : QObject(parent), m_path(std::move(path))
{
    m_targetWord = "secret";
}

ArchiveManager::ArchiveManager(QObject* parent)
    : QObject(parent)
{
    m_targetWord = "secret";
}

ArchiveManager::ArchiveManager(QString filePath, QString saveDir, QObject *parent)
    : QObject(parent), m_path(std::move(filePath)), m_saveDir(std::move(saveDir))
{
    m_targetWord = "secret";
    emit onSaveDirectorySet();
}


void ArchiveManager::setPath(const QString& path) { m_path = path; }
void ArchiveManager::setSaveDir(const QString& dir) { m_saveDir = dir; emit onSaveDirectorySet(); }

bool ArchiveManager::processZip()
{
    const uint32_t ZIP_LOCAL_FILE_SIGNATURE = 0x04034B50; // PK\x03\x04
    const quint16 COMPRESSION_STORED = 0;

    QFile file(m_path);
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "Cannot open file:" << m_path;
        return false;
    }

    bool found = false;
    qint64 currentPos = 0;
    qint64 fileSize = file.size();

    // Буфер для чтения заголовка
    QByteArray headerBuffer(sizeof(ZipLocalFileHeader), 0);

    while (currentPos < fileSize)
    {
        file.seek(currentPos);

        qint64 bytesRead = file.read(headerBuffer.data(), sizeof(ZipLocalFileHeader));
        if (bytesRead != sizeof(ZipLocalFileHeader)) {
            break;
        }

        const ZipLocalFileHeader* header =
            reinterpret_cast<const ZipLocalFileHeader*>(headerBuffer.constData());

        if (header->signature != ZIP_LOCAL_FILE_SIGNATURE)
        {
            qDebug() << "Signature not found at expected position, searching linearly...";
            QByteArray searchChunk;
            const int CHUNK_SIZE = 4096; // Размер буфера для поиска сигнатуры

            file.seek(currentPos);

            bool signatureFound = false;
            while (file.bytesAvailable() > 0) {
                searchChunk = file.read(CHUNK_SIZE);
                int relativeIndex = searchChunk.indexOf("PK\x03\x04");
                if (relativeIndex != -1) {
                    currentPos += relativeIndex;
                    signatureFound = true;
                    qDebug() << "Found signature at new position:" << currentPos;
                    break;
                }
                currentPos += searchChunk.size();
            }

            if (!signatureFound) break;

            continue;
        }


        uint16_t compressionMethod = qFromLittleEndian(header->compression);
        uint16_t fileNameLength = qFromLittleEndian(header->fileNameLength);
        uint16_t extraFieldLength = qFromLittleEndian(header->extraFieldLength);
        uint32_t compressedSize = qFromLittleEndian(header->compressedSize);

        const qint64 fileNameOffset = currentPos + sizeof(ZipLocalFileHeader);
        const qint64 extraFieldOffset = fileNameOffset + fileNameLength;
        const qint64 dataOffset = extraFieldOffset + extraFieldLength;

        file.seek(fileNameOffset);
        QString filename;
        if (fileNameLength > 0) {
            QByteArray nameBytes = file.read(fileNameLength);
            filename = QString::fromUtf8(nameBytes);
        }

        // Проверяем метод сжатия и обрабатываем данные
        if (compressionMethod != COMPRESSION_STORED) {
            qWarning() << "Skipping compressed file (OOM protection):" << filename;
            currentPos = dataOffset + compressedSize;
            continue;
        }

        file.seek(dataOffset);
        QByteArray fileContent;

        if (compressedSize > 0) {
            fileContent = file.read(compressedSize);
        }


        if (!fileContent.isEmpty() && fileContent.contains(m_targetWord.toUtf8()))
        {
            qDebug() << "File" << filename << "contains" << m_targetWord;
            found = true;
            saveFile(filename, fileContent);
            emit fileFound(filename);
        }
        else if (!fileContent.isEmpty())
        {
            qDebug() << "File" << filename << "doesn't contain" << m_targetWord;
        }

        currentPos = dataOffset + compressedSize;
    }

    file.close();
    if (!found) {
        qDebug() << "Target word not found in any file of archive:" << m_path;
    }
    emit finished();
    return found;
}

bool ArchiveManager::saveFile(const QString& filename, const QByteArray& content)
{
    if (m_saveDir.isEmpty()) return false;
    QDir dir(m_saveDir);
    if (!dir.exists() && !dir.mkpath(".")) return false;

    QString safeName = filename;
    safeName.replace(QRegularExpression("[\\\\/:*?\"<>|]"), "_");
    QString fullPath = dir.filePath(safeName);

    QFile outFile(fullPath);
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        qWarning() << "Cannot open file for writing:" << fullPath;
        return false;
    }

    if (outFile.write(content) == -1)
    {
        qWarning() << "Failed to write content to file:" << fullPath;
        outFile.remove();
        return false;
    }

    outFile.flush();
    qDebug() << "File saved to:" << fullPath;
    return true;
}
