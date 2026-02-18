#pragma once
#include <QString>
#include <QUrl>

class RepoParser
{
public:
    static QString extract(const QString& input)
    {
        QString text = input.trimmed();

        if (text.startsWith("http")) {
            QUrl url(text);
            if (url.host() != "github.com")
                return {};

            QString path = url.path();
            if (path.startsWith("/"))
                path.remove(0,1);

            auto parts = path.split("/");
            if (parts.size() < 2)
                return {};

            return parts[0] + "/" + parts[1];
        }

        if (!text.contains("/"))
            return {};

        return text;
    }
};
