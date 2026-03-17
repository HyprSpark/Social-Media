#pragma once

#include <QString>
#include <QJsonObject>

class Content {
public:
    QString username;
    QString content;
    int likes;

    Content() {}
    Content(const QString& username, const QString& content, const int& likes)
        : username(username), content(content), likes(likes) {
    }

    QJsonObject toJson() const // tells the C++ to work in JSON
    {
        QJsonObject obj;
        obj["username"] = username;
        obj["content"] = content;
        obj["likes"] = likes;
        return obj;
    }

    static Content fromJson(const QJsonObject& obj) // Converts JSON to  C++ as a 'user' object, allowing c++ to read it
    {
        return Content(
            obj["username"].toString("Unknown"), // Defaults to said value if missing
            obj["content"].toString("No Content"),
            obj["likes"].toInt()
        );
    }
};