#include "OfflineCache.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QCoreApplication>
#include <QDateTime>


static QString cacheDir()
{
    QString base = QCoreApplication::applicationDirPath();
    QString path = base + "/offline_cache";

    QDir dir;
    if (!dir.exists(path))
        dir.mkpath(path);

    return path;
}

static QString cachePath(const QString& repo)
{
    QString sanitized = repo;
    sanitized.replace("/", "_");

    return cacheDir() + "/" + sanitized + ".json";
}

void OfflineCache::save(const QString& repo, const QJsonArray& issues)
{
    QJsonObject root;
    root["repo"] = repo;
    root["timestamp"] = QDateTime::currentSecsSinceEpoch();
    root["issues"] = issues;

    QFile file(cachePath(repo));
    if (file.open(QIODevice::WriteOnly))
        file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
}

QJsonArray OfflineCache::load(const QString& repo)
{
    QFile file(cachePath(repo));
    if (!file.open(QIODevice::ReadOnly))
        return {};

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    if (!doc.isObject())
        return {};

    return doc.object()["issues"].toArray();
}
