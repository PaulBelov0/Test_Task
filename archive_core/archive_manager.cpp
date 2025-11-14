#include "archive_manager.h"
#include <QtEndian>

ArchiveManager::ArchiveManager(QString path, LaunchType lType, QObject* parent)
    : QObject(parent), m_path(std::move(path))
{
    m_targetWord = "secret";
    m_launch = lType;
#if _WIN64
    m_folderName = "\\extracted";
#else
    m_folderName = "/extracted";
#endif
}

ArchiveManager::ArchiveManager(LaunchType lType, QObject* parent)
    : QObject(parent)
{
    m_targetWord = "secret";
#if _WIN64
    m_folderName = "\\extracted";
#else
    m_folderName = "/extracted";
#endif
}

ArchiveManager::ArchiveManager(QString filePath, QString saveDir, QObject *parent)
    : QObject(parent), m_path(std::move(filePath)), m_saveDir(std::move(saveDir))
{
    m_targetWord = "secret";
#if _WIN64
    m_folderName = "\\extracted";
#else
    m_folderName = "/extracted";
#endif
}


void ArchiveManager::setPath(const QString& path) { m_path = path; }
void ArchiveManager::setSaveDir(const QString& dir)
{
    m_saveDir = dir;
    emit onSaveDirectorySet();
}

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
            QByteArray searchChunk;
            const int CHUNK_SIZE = 4096; // Размер буфера для поиска сигнатуры

            file.seek(currentPos);

            bool signatureFound = false;
            while (file.bytesAvailable() > 0)
            {
                searchChunk = file.read(CHUNK_SIZE);
                int relativeIndex = searchChunk.indexOf("PK\x03\x04");
                if (relativeIndex != -1)
                {
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

        file.seek(dataOffset);
        QByteArray fileContent;

        if (compressedSize > 0) {
            fileContent = file.read(compressedSize);
        }


        if (!fileContent.isEmpty() && fileContent.contains(m_targetWord.toUtf8()))
        {
            // qDebug() << "File" << filename << "contains" << m_targetWord;
            found = true;
            saveFile(filename, fileContent);//todo
            // emit fileFound(filename);
        }

        currentPos = dataOffset + compressedSize;
    }
    qDebug() << "emit finished()";
    emit finished();
    file.close();
    return found;
}

bool ArchiveManager::saveFile(const QString& filename, const QByteArray& content)
{
    if ((m_saveDir + m_folderName).isEmpty())
        return false;

    QDir dir(m_saveDir + m_folderName);
    if (!dir.exists() && !dir.mkpath("."))
        return false;

    QString safeName = filename;
    safeName.replace(QRegularExpression("[\\\\/:*?\"<>|]"), "_");
    QString fullPath = dir.filePath(safeName);

    QFile outFile(fullPath);
    if (!outFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        return false;
    }

    if (outFile.write(content) == -1)
    {
        outFile.remove();
        return false;
    }

    outFile.flush();
    return true;
}

bool ArchiveManager::saveFile(const QString&  saveDirPath)
{
    if (m_acceptFilesData.isEmpty()) {
        qDebug() << "No files to archive.";
        return false;
    }

    QTemporaryDir tempDir(m_saveDir + m_folderName);
    if (!tempDir.isValid()) {
        qWarning() << "Failed to create temporary directory.";
        return false;
    }

    QDir saveDir(m_saveDir + m_folderName);
    QString outputPath = saveDir.filePath("extracted.zip");

    if (!saveFilteredFilesToTemp(tempDir)) {
        return false;
    }

    if (!compressTempDirWithSystemZip()) {
        return false;
    }

    return true;
}

bool ArchiveManager::saveFilteredFilesToTemp(QTemporaryDir& tempDir)
{
    if (m_acceptFilesData.isEmpty()) {
        qDebug() << "No files to save to temp directory.";
        return false;
    }

    QFile inFile(m_path); // Путь к оригинальному ZIP-архиву
    if (!inFile.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open source zip file for reading data:" << m_path;
        return false;
    }

    if (!tempDir.isValid()) {
        qWarning() << "Cannot create temporary directory.";
        return false;
    }

    qDebug() << "Saving filtered files to temporary directory:" << tempDir.path();

    for (const ZipEntry& entry : m_acceptFilesData)
    {
        QString fullPath = tempDir.filePath(entry.filename);
        QFileInfo fileInfo(fullPath);
        QDir().mkpath(fileInfo.path());

        QFile outFile(fullPath);
        if (!outFile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            qWarning() << "Cannot open temp output file for writing:" << fullPath;
            continue;
        }

        inFile.seek(entry.dataOffset);
        QByteArray content = inFile.read(entry.size);
        outFile.write(content);
        outFile.close();
    }

    inFile.close();
    return true;
}


bool ArchiveManager::compressTempDirWithSystemZip()
{
    QString program = "zip";
    QString command;
    QString outputZip = m_saveDir + m_folderName + ".zip";

#ifdef _WIN32
    command = "powershell -Command \"Compress-Archive -Path '" + m_saveDir + m_folderName + "/*' -DestinationPath '" + outputZip + "' -Force\" > nul 2>&1";
    qDebug() << "Windows command:" << command;
#else
    command = "zip -r \"" + outputZip + "\" \"" + m_saveDir + m_folderName + "\" > /dev/null 2>&1";
    qDebug() << "Linux command:" << command;
#endif

    qDebug() << "result: " << system(command.toStdString().c_str());
}
