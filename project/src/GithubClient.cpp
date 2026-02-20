#include "GithubClient.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>
#include <QUrlQuery>
#include <QRegularExpression>

GithubClient::GithubClient(QObject *parent)
    : QObject(parent) {}

void GithubClient::setToken(const QString &token)
{
    m_token = token;
}

int GithubClient::parseTotalPages(const QByteArray& linkHeader)
{
    QRegularExpression lastRegex(R"(<[^>]*[?&]page=(\d+)>; rel="last")");
    QRegularExpressionMatch match = lastRegex.match(linkHeader);
    
    if (match.hasMatch()) {
        return match.captured(1).toInt();
    }
    
    QRegularExpression nextRegex(R"(<[^>]*[?&]page=(\d+)>; rel="next")");
    match = nextRegex.match(linkHeader);
    
    if (match.hasMatch()) {
        return -1;
    }
    
    return 1;
}

void GithubClient::fetchIssues(const QString& repo, int page, int perPage)
{
    QString url = QString("https://api.github.com/repos/%1/issues").arg(repo);
    
    QUrlQuery query;
    query.addQueryItem("page", QString::number(page));
    query.addQueryItem("per_page", QString::number(perPage));
    query.addQueryItem("state", "all");
    
    QUrl fullUrl(url);
    fullUrl.setQuery(query);
    
    QNetworkRequest request(fullUrl);
    request.setHeader(QNetworkRequest::UserAgentHeader, "QtApp");
    request.setRawHeader("Accept", "application/vnd.github.v3+json");

    if (!m_token.isEmpty())
        request.setRawHeader("Authorization", "Bearer " + m_token.toUtf8());

    auto reply = m_manager.get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply, page]() {

        int status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

        if (status == 404) {
            emit errorOccurred("Repository not found or private");
            reply->deleteLater();
            return;
        }

        if (status == 403) {
            emit errorOccurred("Access denied or rate limit exceeded");
            reply->deleteLater();
            return;
        }

        if (reply->error() != QNetworkReply::NoError) {
            emit errorOccurred(reply->errorString());
            reply->deleteLater();
            return;
        }

        QByteArray linkHeader = reply->rawHeader("Link");
        int totalPages = parseTotalPages(linkHeader);
        
        if (totalPages == -1) {
            totalPages = 0;
        }

        const auto data = reply->readAll();
        const auto doc = QJsonDocument::fromJson(data);

        if (!doc.isArray()) {
            emit errorOccurred("Invalid JSON");
            reply->deleteLater();
            return;
        }

        emit issuesReady(doc.array(), page, totalPages);
        reply->deleteLater();
    });
}