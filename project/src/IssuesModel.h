#pragma once
#include <QAbstractListModel>
#include <QJsonArray>
#include "GithubClient.h"

class IssuesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)
    Q_PROPERTY(int currentPage READ currentPage NOTIFY pageChanged)
    Q_PROPERTY(int totalPages READ totalPages NOTIFY pageChanged)
    Q_PROPERTY(int totalIssues READ totalIssues NOTIFY pageChanged)
    Q_PROPERTY(bool hasNextPage READ hasNextPage NOTIFY pageChanged)
    Q_PROPERTY(bool hasPreviousPage READ hasPreviousPage NOTIFY pageChanged)

public:
    enum Roles { TitleRole = Qt::UserRole + 1, UrlRole };

    explicit IssuesModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void load(const QString& repoInput);
    Q_INVOKABLE void loadPage(int page);
    Q_INVOKABLE void nextPage();
    Q_INVOKABLE void previousPage();
    Q_INVOKABLE void setToken(const QString& token);
    Q_INVOKABLE void clearCache();

    QString error() const;
    int currentPage() const { return m_currentPage; }
    int totalPages() const { return m_totalPages; }
    int totalIssues() const { return m_totalIssues; }
    bool hasNextPage() const { return m_currentPage < m_totalPages; }
    bool hasPreviousPage() const { return m_currentPage > 1; }

signals:
    void errorChanged();
    void pageChanged();
    void loadingChanged();

private:
    struct PageData {
        QList<QPair<QString, QString>> issues;
        bool loaded = false;
    };

    void saveToCache();
    void loadFromCache();
    void appendIssues(int page, const QJsonArray& array, int totalPagesFromHeader = 0);
    void setError(const QString& err);

    GithubClient m_client;
    QMap<int, PageData> m_pages;
    
    QString m_repo;
    QString m_error;
    int m_currentPage = 1;
    int m_totalPages = 0;
    int m_totalIssues = -1;
    const int m_perPage = 10;
};