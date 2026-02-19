#pragma once
#include <QAbstractListModel>
#include <QJsonArray>
#include "GithubClient.h"

class IssuesModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(QString error READ error NOTIFY errorChanged)

public:
    enum Roles { TitleRole = Qt::UserRole + 1, UrlRole };

    explicit IssuesModel(QObject *parent = nullptr);

    int rowCount(const QModelIndex &) const override;
    QVariant data(const QModelIndex &, int role) const override;
    QHash<int, QByteArray> roleNames() const override;

    Q_INVOKABLE void load(const QString& repoInput);
    Q_INVOKABLE void loadNextPage();
    Q_INVOKABLE void setToken(const QString& token);

    QString error() const;

signals:
    void errorChanged();

private:
    void saveToCache();
    void appendIssues(const QJsonArray& array);
    void setError(const QString& err);

    GithubClient m_client;
    QList<QPair<QString, QString>> m_data;

    QString m_repo;
    QString m_error;
    int m_page = 1;
};
