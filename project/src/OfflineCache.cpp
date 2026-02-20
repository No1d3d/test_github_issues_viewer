#include "OfflineCache.h"
#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QCoreApplication>
#include <QStandardPaths>

QString OfflineCache::m_cacheDir;

void OfflineCache::setCacheDir(const QString& path)
{
    m_cacheDir = path;
    QDir dir;
    if (!dir.exists(m_cacheDir))
        dir.mkpath(m_cacheDir);
}

QString OfflineCache::getCachePath(const QString& repo)
{
    if (m_cacheDir.isEmpty()) {
        m_cacheDir = QDir::current().absolutePath() + "/cache";
        QDir().mkpath(m_cacheDir);
    }
    
    QString sanitized = repo;
    sanitized.replace("/", "_");
    sanitized.replace(":", "_");
    sanitized.replace(".", "_");
    sanitized.replace("\\", "_");

    return m_cacheDir + "/" + sanitized + ".json";
}

void OfflineCache::save(const QString& repo, const QJsonObject& data)
{
    QFile file(getCachePath(repo));
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(data);
        file.write(doc.toJson(QJsonDocument::Indented));
        file.close();
    }
}

QJsonObject OfflineCache::load(const QString& repo)
{
    QFile file(getCachePath(repo));
    if (!file.open(QIODevice::ReadOnly))
        return {};

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    
    if (!doc.isObject())
        return {};

    return doc.object();
}

void OfflineCache::clear(const QString& repo)
{
    QFile file(getCachePath(repo));
    if (file.exists())
        file.remove();
}