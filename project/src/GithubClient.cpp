#include "GithubClient.h"
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QJsonDocument>

GithubClient::GithubClient(QObject *parent)
    : QObject(parent) {}

void GithubClient::setToken(const QString &token)
{
    m_token = token;
}

void GithubClient::fetchIssues(const QString& repo, int page, int perPage)
{
    const QString url = QString(
        "https://api.github.com/repos/%1/issues?page=%2&per_page=%3"
    ).arg(repo).arg(page).arg(perPage);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, "QtApp");

    if (!m_token.isEmpty())
        request.setRawHeader("Authorization", "Bearer " + m_token.toUtf8());

    auto reply = m_manager.get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {

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

        const auto data = reply->readAll();
        const auto doc = QJsonDocument::fromJson(data);

        if (!doc.isArray()) {
            emit errorOccurred("Invalid JSON");
            reply->deleteLater();
            return;
        }

        emit issuesReady(doc.array());
        reply->deleteLater();
    });
}
