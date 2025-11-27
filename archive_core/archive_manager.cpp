#include "archive_manager.h"
namespace {
// DOS date: [year:7][month:4][day:5]
constexpr uint16_t DAY_BITS = 5;
constexpr uint16_t MONTH_BITS = 4;
constexpr uint16_t YEAR_BASE = 1980;

// Byte masks
constexpr uint16_t DAY_MASK = (1 << DAY_BITS) - 1;
constexpr uint16_t MONTH_MASK = (1 << MONTH_BITS) - 1;
constexpr uint16_t MINUTE_MASK = (1 << 6) - 1;
constexpr uint16_t HOUR_MASK = (1 << 5) - 1;
constexpr uint16_t SECOND_MASK = (1 << 5) - 1;
}

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


void ArchiveManager::setPath(const QString& path)
{
    m_path = path;
}
void ArchiveManager::setSaveDir(const QString& dir)
{
    m_saveDir = dir;
    emit onSaveDirectorySet();
}

bool ArchiveManager::processZip()
{
    emit onCurrentStageChanged("Сканирование");
    const uint32_t ZIP_LOCAL_FILE_SIGNATURE = 0x04034B50;
    const quint16 COMPRESSION_STORED = 0;

    m_file.setFileName(m_path);
    if (!m_file.open(QIODevice::ReadOnly))
    {
        qWarning() << "Cannot open file:" << m_path;
        return false;
    }

    bool found = false;
    qint64 currentPos = 0;
    qint64 fileSize = m_file.size();
    emit onProgressMaxValueChanged(fileSize);

    QByteArray headerBuffer(sizeof(ZipLocalFileHeader), 0);

    while (currentPos < fileSize) {
        m_file.seek(currentPos);

        qint64 bytesRead = m_file.read(headerBuffer.data(), sizeof(ZipLocalFileHeader));
        if (bytesRead != sizeof(ZipLocalFileHeader))
        {
            break;
        }

        const ZipLocalFileHeader* header = reinterpret_cast<const ZipLocalFileHeader*>(headerBuffer.constData());

        if (header->signature != ZIP_LOCAL_FILE_SIGNATURE)
        {
            QByteArray searchChunk;
            const int CHUNK_SIZE = 4096;

            m_file.seek(currentPos);
            bool signatureFound = false;

            while (m_file.bytesAvailable() > 0)
            {
                searchChunk = m_file.read(CHUNK_SIZE);
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

        uint16_t fileNameLength = qFromLittleEndian(header->fileNameLength);
        uint16_t extraFieldLength = qFromLittleEndian(header->extraFieldLength);
        uint32_t compressedSize = qFromLittleEndian(header->compressedSize);

        const qint64 fileNameOffset = currentPos + sizeof(ZipLocalFileHeader);
        const qint64 extraFieldOffset = fileNameOffset + fileNameLength;
        const qint64 dataOffset = extraFieldOffset + extraFieldLength;

        m_file.seek(fileNameOffset);
        QString filename;
        if (fileNameLength > 0)
        {
            QByteArray nameBytes = m_file.read(fileNameLength);
            filename = QString::fromUtf8(nameBytes);
        }

        m_file.seek(dataOffset);
        QByteArray fileContent;

        if (compressedSize > 0)
        {
            fileContent = m_file.read(compressedSize);
        }

        if (!fileContent.isEmpty() && fileContent.contains(m_targetWord.toUtf8()))
        {
            m_fileMap.insert(filename, std::pair<quint64, quint64>(dataOffset, header->compressedSize));
            emit onAcceptibleFileAdded(filename, header->uncompressedSize, convertDosDateTime(header->modDate, header->modTime));
            found = true;
        }

        currentPos = dataOffset + compressedSize;
        emit onProgressGuiChanged(dataOffset + header->compressedSize, fileSize);

        if (m_launch != LaunchType::CLI || m_launch != LaunchType::Tests)
            QApplication::processEvents();
    }

    emit onProgressGuiChanged(fileSize, fileSize);
    m_file.close();
    return found;
}

QDateTime ArchiveManager::convertDosDateTime(uint16_t modDate, uint16_t modTime)
{
    const auto day = modDate & DAY_MASK;
    const auto month = (modDate >> DAY_BITS) & MONTH_MASK;
    const auto year = (modDate >> (DAY_BITS + MONTH_BITS)) + YEAR_BASE;

    const auto second = (modTime & SECOND_MASK) * 2;
    const auto minute = (modTime >> DAY_BITS) & MINUTE_MASK;
    const auto hour = (modTime >> (DAY_BITS + 6)) & HOUR_MASK;

    return QDateTime(QDate(year, month, day), QTime(hour, minute, second));
}


bool ArchiveManager::saveFiles(const QString& saveDirPath, const QStringList& selectedFiles)
{
    if (m_fileMap.isEmpty() || selectedFiles.isEmpty())
    {
        qDebug() << "No files to archive.";
        return false;
    }

    m_saveDir = QDir::toNativeSeparators(saveDirPath);

    QTemporaryDir tempDir(m_saveDir + m_folderName);
    if (!tempDir.isValid())
    {
        qWarning() << "Failed to create temporary directory.";
        return false;
    }


    int savingFilesSize = 0;

    for (int i = 0; i < selectedFiles.size(); ++i)
    {
        if (m_fileMap.contains(selectedFiles.at(i)))
            savingFilesSize += m_fileMap[selectedFiles.at(i)].second * 1.6;
    }

    if (savingFilesSize > QStorageInfo(saveDirPath).bytesAvailable())
    {
        qDebug() << QStorageInfo(saveDirPath).bytesAvailable();
        emit onErrorOccured("Недостаточно места на диске для сохранения");
    }

    QString tempPath = tempDir.path();
    qDebug() << "Using temp directory:" << tempPath;

#ifdef _WIN32
    tempPath.replace('/', '\\');
#endif

    if (!saveFilteredFilesToTemp(tempDir, selectedFiles))
    {
        return false;
    }

    m_compressingPath = tempPath;

    if (!compressDirectory())
    {
        return false;
    }

    return true;
}

bool ArchiveManager::saveFiles(const QString& saveDirPath)
{
    if (m_fileMap.isEmpty())
    {
        qDebug() << "No files to archive.";
        return false;
    }

    m_saveDir = QDir::toNativeSeparators(saveDirPath);

    QTemporaryDir tempDir(m_saveDir + m_folderName);
    if (!tempDir.isValid())
    {
        qWarning() << "Failed to create temporary directory.";
        return false;
    }


    int savingFilesSize = 0;

    for (auto it : m_fileMap)
    {
        savingFilesSize += (it.second * 1.6);
    }

    if (savingFilesSize > QStorageInfo(tempDir.path()).bytesAvailable())
    {
        return false;
    }

    QString tempPath = tempDir.path();
    qDebug() << "Using temp directory:" << tempPath;

#ifdef _WIN32
    tempPath.replace('/', '\\');
#endif

    QStringList selectedFiles;

    int totalFiles = m_fileMap.size();
    int currentFile = 0;

    for (auto it = m_fileMap.constBegin(); it != m_fileMap.constEnd(); ++it)
    {
        selectedFiles.append(it.key());
        currentFile++;
        emit onProgressGuiChanged(currentFile, totalFiles);
    }

    if (!saveFilteredFilesToTemp(tempDir, selectedFiles))
    {
        return false;
    }

    m_compressingPath = tempPath;

    if (!compressDirectory())
    {
        return false;
    }

    return true;
}

bool ArchiveManager::saveFilteredFilesToTemp(QTemporaryDir& tempDir, const QStringList& selectedFiles)
{
    emit onCurrentStageChanged("Сохранение");

    if (m_fileMap.isEmpty() || selectedFiles.isEmpty())
    {
        return false;
    }

    int savingFilesSize = 0;

    QFile inFile(m_path);
    if (!inFile.open(QIODevice::ReadOnly))
    {
        return false;
    }

    qDebug() << "Saving" << selectedFiles.size() << "files to temp directory...";

    int savedCount = 0;
    const int REPORT_INTERVAL = 100;

    emit onProgressMaxValueChanged(selectedFiles.size());

    QSet<QString> createdDirs;

    for (int i = 0; i < selectedFiles.size(); ++i)
    {
        const QString& filename = selectedFiles[i];

        if (!m_fileMap.contains(filename))
        {
            continue;
        }

        std::pair<quint64, quint64> positionAndSize = m_fileMap[filename];

        QString fullPath = tempDir.filePath(filename);
        QFileInfo fileInfo(fullPath);
        QString dirPath = fileInfo.path();

        if (!createdDirs.contains(dirPath))
        {
            QDir().mkpath(dirPath);
            createdDirs.insert(dirPath);
        }

        QFile outFile(fullPath);
        if (outFile.open(QIODevice::WriteOnly))
        {
            inFile.seek(positionAndSize.first);
            QByteArray content = inFile.read(positionAndSize.second);

            if (outFile.write(content) == content.size())
            {
                savedCount++;
            }
            outFile.close();
        }

        if (i % REPORT_INTERVAL == 0)
        {
            qDebug() << "Progress:" << i << "/" << selectedFiles.size() << "files saved";
            emit onProgressGuiChanged(i, selectedFiles.size());
            QApplication::processEvents();
        }
    }

    inFile.close();
    qDebug() << "Successfully saved" << savedCount << "files";
    emit onProgressGuiChanged(selectedFiles.size(), selectedFiles.size());
    return savedCount > 0;
}

bool ArchiveManager::compressDirectory()
{
    emit onCurrentStageChanged("Архивирование");

    QString sourcePath = m_compressingPath;
    QString destPath = QDir::toNativeSeparators(m_saveDir + m_folderName + ".zip");

    qDebug() << "Fast compression starting...";

    QProcess process;

#ifdef _WIN32
    process.setProgram("powershell");
    QString command = QString(
                          "$startTime = Get-Date\n"
                          "$allFiles = @(Get-ChildItem '%1' -Recurse -File)\n"
                          "Write-Host 'TOTAL_FILES:' $allFiles.Count\n"
                          "Write-Host 'Files to compress:' $allFiles.Count\n"
                          "\n"
                          "if ($allFiles.Count -gt 0) {\n"
                          "    $null = New-Item -Path '%2' -ItemType File -Force\n"
                          "    \n"
                          "    for ($i = 0; $i -lt $allFiles.Count; $i += 1000) {\n"
                          "        $batch = $allFiles[$i..[Math]::Min($i+999, $allFiles.Count-1)]\n"
                          "        Compress-Archive -Path $batch.FullName -DestinationPath '%2' -Update -CompressionLevel Fastest\n"
                          "        $current = [Math]::Min($i + 1000, $allFiles.Count)\n"
                          "        $percent = [int](($current / $allFiles.Count) * 100)\n"
                          "        Write-Host 'PROGRESS:' $current $allFiles.Count $percent\n"
                          "    }\n"
                          "    \n"
                          "    $endTime = Get-Date\n"
                          "    $duration = $endTime - $startTime\n"
                          "    Write-Host \"Compression completed in $($duration.TotalSeconds) seconds\"\n"
                          "    Write-Host 'COMPLETE'\n"
                          "    exit 0\n"
                          "} else {\n"
                          "    Write-Error 'No files found'\n"
                          "    exit 1\n"
                          "}"
                          ).arg(sourcePath).arg(destPath);

    process.setArguments({"-Command", command});

#else
    process.setProgram("zip");

    QProcess findProcess;
    findProcess.setProgram("find");
    findProcess.setArguments({sourcePath, "-type", "f"});
    findProcess.start();
    findProcess.waitForFinished();

    QString fileList = findProcess.readAllStandardOutput();
    int totalFiles = fileList.count('\n') + (fileList.isEmpty() ? 0 : 1);

    qDebug() << "Total files found:" << totalFiles;
    emit onProgressGuiChanged(0, totalFiles);

    QTemporaryFile tempFile;
    if (tempFile.open())
    {
        QTextStream stream(&tempFile);
        stream << fileList;
        tempFile.close();
    }

    process.setArguments({"-r", "-1", destPath, ".", "-i@" + tempFile.fileName()});

#endif

    process.start();

    int totalFiles = 0;
    int processedFiles = 0;

    connect(&process, &QProcess::readyReadStandardOutput, [&process, this, &totalFiles, &processedFiles]() {
        QString output = process.readAllStandardOutput();

        QTextStream stream(&output);
        while (!stream.atEnd()) {
            QString line = stream.readLine();

#ifdef _WIN32
            if (line.contains("TOTAL_FILES:"))
            {
                totalFiles = line.section("TOTAL_FILES:", 1).trimmed().section(' ', 0, 0).toInt();
                emit onProgressGuiChanged(0, totalFiles);
            }
            else if (line.contains("PROGRESS:"))
            {
                QStringList parts = line.section("PROGRESS:", 1).trimmed().split(' ', Qt::SkipEmptyParts);
                if (parts.size() >= 2) {
                    processedFiles = parts[0].toInt();
                    int total = parts[1].toInt();
                    emit onProgressGuiChanged(processedFiles, total);
                }
            }
            else if (line.contains("COMPLETE"))
            {
                emit onProgressGuiChanged(totalFiles, totalFiles);
                emit onCurrentStageChanged("Завершено");
            }
#else
            if (line.contains("adding:") || line.contains("updating:"))
            {
                processedFiles++;
                if (processedFiles % 100 == 0 && totalFiles > 0)
                {
                    emit onProgressGuiChanged(processedFiles, totalFiles);
                }
            }

            // Ищем завершение
            if (line.contains("zip warning") || line.contains("zip error")) {
                qWarning() << "Zip warning/error:" << line;
            }
#endif
        }
    });

    connect(&process, &QProcess::readyReadStandardError, [&process, this]() {
        QString error = process.readAllStandardError();

        if (!error.trimmed().isEmpty()) {
            emit onErrorOccured(error);
        }
    });

    connect(&process, QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [this, &process, totalFiles](int exitCode, QProcess::ExitStatus exitStatus) {
                if (exitStatus == QProcess::NormalExit && exitCode == 0) {
                    emit onProgressGuiChanged(totalFiles, totalFiles);
                    emit onCurrentStageChanged("Завершено");
                }
                else
                {
                    emit onCurrentStageChanged("Ошибка");
                    emit onErrorOccured(QString("Код ошибки: %1").arg(exitCode));
                }
            });

    if (!process.waitForFinished(120000))
    {
        process.kill();
        emit onErrorOccured("Таймаут операции архивирования");
        return false;
    }

    if (process.exitCode() != 0)
    {
        emit onCurrentStageChanged("Ошибка");
        emit onErrorOccured(QString("Код ошибки: %1").arg(process.exitCode()));
        return false;
    }

    return true;
}

bool ArchiveManager::checkDiskSpaceWithCompression(const QString& savePath, double compressionRatio)
{
    QStorageInfo storage(savePath);
    if (!storage.isValid()) return false;

    quint64 totalOriginalSize = 0;
    for (auto it = m_fileMap.constBegin(); it != m_fileMap.constEnd(); ++it)
    {
        totalOriginalSize += it.value().first;
    }

    double avgCompressionRatio = 0;
    int count = 0;

    for (auto it = m_fileMap.constBegin(); it != m_fileMap.constEnd(); ++it)
    {
        if (it.value().first > 0)
        {
            double ratio = static_cast<double>(it.value().second) / it.value().first;
            avgCompressionRatio += ratio;
            count++;
        }
    }

    if (count > 0)
    {
        avgCompressionRatio /= count;
        compressionRatio = avgCompressionRatio;
    }

    quint64 estimatedNewArchiveSize = static_cast<quint64>(totalOriginalSize * compressionRatio);
    quint64 tempFilesSize = totalOriginalSize;

    const quint64 safetyBuffer = 150 * 1024 * 1024;
    quint64 requiredSpace = tempFilesSize + estimatedNewArchiveSize + safetyBuffer;

    return storage.bytesAvailable() >= requiredSpace;
}
