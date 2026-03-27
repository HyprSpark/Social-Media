#pragma once
#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime> // Add this for time functions

class Content {
public:
    QString username;
    QString content;
    QStringList likedBy;
    QString timestamp; // New field

    Content() {}

    Content(const QString& username, const QString& content, const QStringList& likedBy, const QString& timestamp)
        : username(username), content(content), likedBy(likedBy), timestamp(timestamp) {
    }

    QJsonObject toJson() const {
        QJsonObject obj;
        obj["username"] = username;
        obj["content"] = content;
        obj["timestamp"] = timestamp; // Save to JSON

        QJsonArray likesArray;
        for (const QString& name : likedBy) likesArray.append(name);
        obj["likedBy"] = likesArray;

        return obj;
    }

    static Content fromJson(const QJsonObject& obj) {
        QStringList likesList;
        QJsonArray likesArray = obj["likedBy"].toArray();
        for (const QJsonValue& value : likesArray) likesList.append(value.toString());

        return Content(
            obj["username"].toString("Unknown"),
            obj["content"].toString("No Content"),
            likesList,
            obj["timestamp"].toString("Long ago") // Load from JSON
        );
    }
};