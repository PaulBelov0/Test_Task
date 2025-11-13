#include "archive_manager.h"

ArchiveManager::ArchiveManager(QString& path, QObject* parent)
    : QObject(parent)
{
    m_targetWord = "secret";
    m_path = path;
    m_filesCount = 0;
    m_clusterCounter = 0;
}

ArchiveManager::ArchiveManager(QObject* parent)
    : QObject(parent)
{
    m_targetWord = "secret";
    m_filesCount = 0;
    m_clusterCounter = 0;
}

ArchiveManager::ArchiveManager(QString& filePath, QString& saveDir, QObject *parent)
{
    m_saveDir = saveDir;
    m_targetWord = "secret";
    m_path = filePath;
    m_filesCount = 0;
    m_clusterCounter = 0;
    emit onSaveDirectorySet();
}

void ArchiveManager::setPath(const std::string& path)
{
    m_path = QString::fromStdString(path);
}
void ArchiveManager::setSaveDir(const std::string& dir)
{
    m_saveDir = QString::fromStdString(dir);
    emit onSaveDirectorySet();
}

void ArchiveManager::filesCounter()
{
    const int FILENAME_LENGTH_OFFSET = 26;
    const int FILENAME_OFFSET = 30;

    QFile file(m_path);
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "Cannot open file:" << m_path;
        return;
    }

    const QByteArray zipSignature = "PK\x03\x04";
    QByteArray data = file.readAll();
    file.close();

    int pos = 0;

    while ((pos = data.indexOf(zipSignature, pos)) != -1)
    {
        int start = pos + zipSignature.size();
        int nextHeaderPos = data.indexOf(zipSignature, start);

        if (nextHeaderPos == -1)
        {
            nextHeaderPos = data.size();
        }


        QString filename = "unknown";
        if (pos + FILENAME_OFFSET < data.size())
        {
            quint16 fileNameLength = *reinterpret_cast<const quint16*>(
                data.constData() + pos + FILENAME_LENGTH_OFFSET);

            if (pos + FILENAME_OFFSET + fileNameLength <= data.size())
            {
                filename = QString::fromLatin1(
                    data.constData() + pos + FILENAME_OFFSET,
                    fileNameLength);
            }
        }
    }
}

bool ArchiveManager::processZip()
{
    // Constants from official ZIP specifications
    const int FILENAME_LENGTH_OFFSET = 26;  // 2 bytes
    const int FILENAME_OFFSET = 30;        // Start of filename

    QFile file(m_path);
    if (!file.open(QIODevice::ReadOnly))
    {
        qWarning() << "Cannot open file:" << m_path;
        return false;
    }

    const QByteArray zipSignature = "PK\x03\x04";
    QByteArray data = file.readAll();
    file.close();

    int pos = 0;
    bool found = false;

    while ((pos = data.indexOf(zipSignature, pos)) != -1)
    {
        int start = pos + zipSignature.size();
        int nextHeaderPos = data.indexOf(zipSignature, start);

        if (nextHeaderPos == -1)
        {
            nextHeaderPos = data.size();
        }

        QString filename = "unknown";
        if (pos + FILENAME_OFFSET < data.size())
        {
            quint16 fileNameLength = *reinterpret_cast<const quint16*>(
                data.constData() + pos + FILENAME_LENGTH_OFFSET);

            if (pos + FILENAME_OFFSET + fileNameLength <= data.size())
            {
                filename = QString::fromLatin1(
                    data.constData() + pos + FILENAME_OFFSET,
                    fileNameLength);
            }
        }

        QByteArray chunk = data.mid(start, nextHeaderPos - start);

        if (chunk.contains(m_targetWord.toUtf8()))
        {
            qDebug() << "✅ File" << filename << "contains" << m_targetWord;
            found = true;
            saveFile(filename, chunk);
        }
        else
        {
            qDebug() << "❌ File" << filename << "doesn't contain" << m_targetWord;
        }

        pos = nextHeaderPos;
    }

    if (!found)
    {
        qDebug() << "Target word not found in any file of archive:" << m_path;
    }

    return found;
}

bool ArchiveManager::saveFile(const QString& filename, const QByteArray& content)
{
    QDir dir(m_saveDir);

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
