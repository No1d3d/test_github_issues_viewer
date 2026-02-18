#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonArray>

class GithubClient : public QObject
{
    Q_OBJECT
public:
    explicit GithubClient(QObject *parent = nullptr);

    void fetchIssues(const QString& repo, int page, int perPage = 20);
    void setToken(const QString& token);

signals:
    void issuesReady(const QJsonArray& issues);
    void errorOccurred(const QString& error);

private:
    QNetworkAccessManager m_manager;
    QString m_token;
};
