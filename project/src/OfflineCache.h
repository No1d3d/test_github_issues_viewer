#pragma once
#include <QJsonArray>
#include <QJsonObject>
#include <QString>

class OfflineCache
{
public:
    static void setCacheDir(const QString& path);
    static void save(const QString& repo, const QJsonObject& data);
    static QJsonObject load(const QString& repo);
    static void clear(const QString& repo);
    
private:
    static QString m_cacheDir;
    static QString getCachePath(const QString& repo);
};