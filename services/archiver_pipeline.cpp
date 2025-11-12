#include "archiver_pipeline.h"

ArchiverPipeline::ArchiverPipeline(QObject *parent)
    : QObject{parent}
    , m_archive(new ArchiveManager(this))
{
    connect(this, &ArchiverPipeline::onSaveDirSet, this, &ArchiverPipeline::startProcessing);
    connect(m_archive.get(), &ArchiveManager::onProcessingFinished, this, &ArchiverPipeline::onProcessingDone);
}

void ArchiverPipeline::startProcessing()
{
    m_archive->processZip();
}

void ArchiverPipeline::setPathToRead(const std::string& path)
{
    bool isDone = checkPathToRead(path);

    if (isDone)
    {
        m_archive->setPath(path);
        emit onFileDetectedSuccessful();
    }
}

void ArchiverPipeline::setPathToSave(const std::string& path)
{
    if (!checkPathToSave(path))
        return;

    m_archive->setSaveDir(path);
    emit onSaveDirSet();
}

bool ArchiverPipeline::checkPathToSave(const std::string& path)
{
    std::error_code ec;
    std::filesystem::path pathStr(path);

    bool exists = std::filesystem::exists(pathStr, ec);
    if (ec || !exists)
    {
        std::cout << "Такой директории не существует!" << std::endl;
        emit onSaveDirecroryWrong();
        return false;
    }

    bool is_dir = std::filesystem::is_directory(pathStr, ec);
    if (ec || !is_dir)
    {
        std::cout << "Данный путь не является директорией!" << std::endl;
        emit onSaveDirecroryWrong();
        return false;
    }

    bool isEmpty = std::filesystem::is_empty(path, ec);
    if (!isEmpty)
    {
        std::cout << "Папка не является пустой!" << std::endl;
        emit onSaveDirecroryWrong();
        return false;
    }

    return true;
}

bool ArchiverPipeline::checkPathToRead(const std::string& path) {

    std::filesystem::path fsPath(path);

    bool exists = std::filesystem::exists(fsPath);
    bool isFile = exists && std::filesystem::is_regular_file(path);
    bool isZip = isFile && isZipFile(fsPath);

    if (!exists)
    {
        std::cout << "Директории не существует!" << std::endl;
        emit onFileDirectoryWrong();
        return false;
    }
    if (!isFile)
    {
        std::cout << "Не является файлом!" << std::endl;
        emit onFileDirectoryWrong();
        return false;
    }
    if (!isZip)
    {
        std::cout << "Не является .zip архивом!" << std::endl;
        emit onFileDirectoryWrong();
        return false;
    }
    return true;
}

bool ArchiverPipeline::isZipFile(const std::filesystem::path& path)
{
    if (!std::filesystem::is_regular_file(path)) return false;

    std::string ext = path.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(),
                   [](unsigned char c){ return std::tolower(c); });

    return ext == ".zip";
}
