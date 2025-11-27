#ifndef FILE_DATA_H
#define FILE_DATA_H

#include <QString>
#include <QFileInfo>

struct FileData {
    bool keep = true;           // Чекбокс
    QString path;               // Полный путь
    QString name;               // Имя файла
    qint64 size = 0;           // Размер
    qint64 timestamp = 0;      // Таймстамп (Unix time)

    // Конструктор для удобства
    FileData(const QString& filePath = "", bool keepFile = true)
        : keep(keepFile), path(filePath)
    {
        QFileInfo info(filePath);
        name = info.fileName();
        size = info.size();
        timestamp = info.lastModified().toSecsSinceEpoch();
    }

    bool operator==(const FileData& other) const {
        return path == other.path;
    }
};

#endif // FILE_DATA_H
