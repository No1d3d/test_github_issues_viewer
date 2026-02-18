#include "OfflineCache.h"
#include <QStandardPaths>
#include <QDir>
#include <QFile>
#include <QJsonDocument>

static QString cachePath(const QString& repo)
{
    QString base = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(base);
    QString sanitized = repo;
    sanitized.replace("/", "_");
    return base + "/" + sanitized + ".json";
}

void OfflineCache::save(const QString& repo, const QJsonArray& data)
{
    QFile file(cachePath(repo));
    if (file.open(QIODevice::WriteOnly))
        file.write(QJsonDocument(data).toJson());
}

QJsonArray OfflineCache::load(const QString& repo)
{
    QFile file(cachePath(repo));
    if (!file.open(QIODevice::ReadOnly))
        return {};

    return QJsonDocument::fromJson(file.readAll()).array();
}
