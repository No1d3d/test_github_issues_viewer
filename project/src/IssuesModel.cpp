#include "IssuesModel.h"
#include "OfflineCache.h"
#include "RepoParser.h"
#include <QJsonObject>
#include <QDebug>
#include <QModelIndex>
#include <QDir>

IssuesModel::IssuesModel(QObject *parent) : QAbstractListModel(parent)
{
    connect(&m_client, &GithubClient::issuesReady, this, 
            [this](const QJsonArray& issues, int page, int totalPagesFromHeader) {
                appendIssues(page, issues, totalPagesFromHeader);
            });
    
    connect(&m_client, &GithubClient::errorOccurred, this, 
            [this](const QString &err){
                setError(err);
                loadFromCache();
            });
    
    QString buildCacheDir = QDir::current().absolutePath() + "/offline_cache";
    OfflineCache::setCacheDir(buildCacheDir);
}

int IssuesModel::rowCount(const QModelIndex &parent) const 
{ 
    if (parent.isValid())
        return 0;
        
    if (m_pages.contains(m_currentPage))
        return m_pages[m_currentPage].issues.size();
    return 0;
}

QVariant IssuesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || !m_pages.contains(m_currentPage))
        return {};

    const auto &item = m_pages[m_currentPage].issues[index.row()];
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
    m_pages.clear();
    m_currentPage = 1;
    m_totalPages = 0;
    m_totalIssues = -1;
    endResetModel();

    m_repo = RepoParser::extract(repoInput);

    if (m_repo.isEmpty()) { 
        setError("Invalid repository"); 
        return; 
    }

    setError("");
    
    loadFromCache();
    
    m_client.fetchIssues(m_repo, m_currentPage, m_perPage);
}

void IssuesModel::loadPage(int page)
{
    if (page < 1 || (m_totalPages > 0 && page > m_totalPages))
        return;

    if (page == m_currentPage)
        return;

    beginResetModel();
    m_currentPage = page;
    endResetModel();

    if (!m_pages.contains(page) || !m_pages[page].loaded) {
        m_client.fetchIssues(m_repo, page, m_perPage);
    }

    emit pageChanged();
}

void IssuesModel::nextPage()
{
    if (hasNextPage())
        loadPage(m_currentPage + 1);
}

void IssuesModel::previousPage()
{
    if (hasPreviousPage())
        loadPage(m_currentPage - 1);
}

void IssuesModel::saveToCache()
{
    if (m_repo.isEmpty())
        return;

    QJsonObject cacheData;
    cacheData["repo"] = m_repo;
    cacheData["totalPages"] = m_totalPages;
    cacheData["totalIssues"] = m_totalIssues;
    cacheData["currentPage"] = m_currentPage;
    
    QJsonObject pages;
    for (auto it = m_pages.begin(); it != m_pages.end(); ++it) {
        if (it.value().loaded) {
            QJsonArray pageArray;
            for (const auto &item : it.value().issues) {
                QJsonObject obj;
                obj["title"] = item.first;
                obj["html_url"] = item.second;
                pageArray.append(obj);
            }
            pages[QString::number(it.key())] = pageArray;
        }
    }
    cacheData["pages"] = pages;

    OfflineCache::save(m_repo, cacheData);
}

void IssuesModel::loadFromCache()
{
    if (m_repo.isEmpty())
        return;

    QJsonObject cacheData = OfflineCache::load(m_repo);
    if (cacheData.isEmpty())
        return;

    m_totalPages = cacheData["totalPages"].toInt();
    m_totalIssues = cacheData["totalIssues"].toInt(-1);
    
    QJsonObject pages = cacheData["pages"].toObject();
    for (auto it = pages.begin(); it != pages.end(); ++it) {
        int page = it.key().toInt();
        QJsonArray pageArray = it.value().toArray();
        
        PageData pageData;
        pageData.loaded = true;
        
        for (const auto &val : pageArray) {
            QJsonObject obj = val.toObject();
            pageData.issues.append({
                obj["title"].toString(),
                obj["html_url"].toString()
            });
        }
        
        m_pages[page] = pageData;
    }

    emit pageChanged();
    
    if (m_pages.contains(m_currentPage)) {
        QModelIndex topLeft = createIndex(0, 0);
        QModelIndex bottomRight = createIndex(rowCount() - 1, 0);
        emit dataChanged(topLeft, bottomRight);
    }
}

void IssuesModel::appendIssues(int page, const QJsonArray &array, int totalPagesFromHeader)
{
    if (totalPagesFromHeader > 0) {
        m_totalPages = totalPagesFromHeader;
        
        if (m_totalPages > 0) {
            m_totalIssues = (m_totalPages - 1) * m_perPage + array.size();
        }
    } else {
        if (array.isEmpty()) {
            if (page == 1) {
                m_totalPages = 1;
                m_totalIssues = 0;
            } else {
                m_totalPages = page - 1;
                int total = 0;
                for (int i = 1; i <= m_totalPages; i++) {
                    if (m_pages.contains(i)) {
                        total += m_pages[i].issues.size();
                    }
                }
                m_totalIssues = total;
            }
        } else if (array.size() < m_perPage) {
            m_totalPages = page;
            
            int total = 0;
            for (int i = 1; i < page; i++) {
                if (m_pages.contains(i)) {
                    total += m_pages[i].issues.size();
                }
            }
            total += array.size();
            m_totalIssues = total;
        } else if (page > m_totalPages) {
            m_totalPages = qMax(m_totalPages, page + 1);
        }
    }

    PageData pageData;
    pageData.loaded = true;

    for (const auto &val : array) {
        QJsonObject obj = val.toObject();
        pageData.issues.append({
            obj["title"].toString(),
            obj["html_url"].toString()
        });
    }

    m_pages[page] = pageData;

    if (page == m_currentPage) {
        beginResetModel();
        endResetModel();
    }

    saveToCache();
    emit pageChanged();
}

void IssuesModel::setToken(const QString &token) 
{ 
    m_client.setToken(token); 
}

void IssuesModel::clearCache()
{
    OfflineCache::clear(m_repo);
    load(m_repo);
}

QString IssuesModel::error() const 
{ 
    return m_error; 
}

void IssuesModel::setError(const QString &err)
{
    if (m_error == err) return;
    m_error = err;
    emit errorChanged();
}