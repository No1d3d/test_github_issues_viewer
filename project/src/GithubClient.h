#pragma once
#include <QObject>
#include <QNetworkAccessManager>
#include <QJsonArray>

class GithubClient : public QObject
{
    Q_OBJECT
public:
    explicit GithubClient(QObject *parent = nullptr);

    void fetchIssues(const QString& repo, int page, int perPage = 10);
    void setToken(const QString& token);

signals:
    void issuesReady(const QJsonArray& issues, int page, int totalPages);
    void errorOccurred(const QString& error);

private:
    int parseTotalPages(const QByteArray& linkHeader);
    
    QNetworkAccessManager m_manager;
    QString m_token;
};