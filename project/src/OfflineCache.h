#pragma once
#include <QJsonArray>

class OfflineCache
{
public:
    static void save(const QString& repo, const QJsonArray& data);
    static QJsonArray load(const QString& repo);
};
