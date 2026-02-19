#include "IssuesModel.h"
#include "OfflineCache.h"
#include "RepoParser.h"
#include <QJsonObject>

IssuesModel::IssuesModel(QObject *parent) : QAbstractListModel(parent)
{
    connect(&m_client, &GithubClient::issuesReady, this, &IssuesModel::appendIssues);
    connect(&m_client, &GithubClient::errorOccurred, this, [this](const QString &err){
        setError(err);
        auto cached = OfflineCache::load(m_repo);
        if (!cached.isEmpty())
            appendIssues(cached);
    });
}

int IssuesModel::rowCount(const QModelIndex &) const { return m_data.size(); }

QVariant IssuesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) return {};

    const auto &item = m_data[index.row()];
    if (role == TitleRole) return item.first;
    if (role == UrlRole) return item.second;
    return {};
}

QHash<int, QByteArray> IssuesModel::roleNames() const
{
    return { {TitleRole, "title"}, {UrlRole, "url"} };
}

void IssuesModel::load(const QString &repoInput)
{
    beginResetModel();
    m_data.clear();
    endResetModel();

    m_repo = RepoParser::extract(repoInput);

    if (m_repo.isEmpty()) { setError("Invalid repository"); return; }

    m_page = 1;
    setError("");

    auto cached = OfflineCache::load(m_repo);
    if (!cached.isEmpty()) appendIssues(cached);

    m_client.fetchIssues(m_repo, m_page);
}

void IssuesModel::loadNextPage()
{
    ++m_page;
    m_client.fetchIssues(m_repo, m_page);
}

void IssuesModel::saveToCache()
{
    QJsonArray array;

    for (const auto &item : m_data) {
        QJsonObject obj;
        obj["title"] = item.first;
        obj["html_url"] = item.second;
        array.append(obj);
    }

    OfflineCache::save(m_repo, array);
}

void IssuesModel::appendIssues(const QJsonArray &array)
{
    if (array.isEmpty())
        return;

    beginInsertRows(QModelIndex(), m_data.size(), m_data.size() + array.size() - 1);

    for (const auto &val : array) {
        QJsonObject obj = val.toObject();

        QString title = obj["title"].toString();
        QString url   = obj["html_url"].toString();

        m_data.append({ title, url });
    }

    endInsertRows();

    saveToCache();
}

void IssuesModel::setToken(const QString &token) { m_client.setToken(token); }

QString IssuesModel::error() const { return m_error; }

void IssuesModel::setError(const QString &err)
{
    if (m_error == err) return;
    m_error = err;
    emit errorChanged();
}
