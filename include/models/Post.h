#pragma once
#include "models/Content.h"
#include <QStringList>
#include <QJsonArray>
#include <QJsonObject>

/**
 * @brief Represents a public feed post. Inherits from Content.
 */
class Post : public Content {
public:
    QStringList likedBy;

    Post() {}

    Post(const QString& sender, const QString& text, const QString& time, const QStringList& likes = QStringList())
        : Content(sender, text, time), likedBy(likes) { // FIX 2: Call Content constructor, not Post
    }

    // Override the base class method
    QJsonObject toJson() const override {
        QJsonObject obj;
        obj["username"] = senderUsername;
        obj["content"] = textContent;
        obj["timestamp"] = timestamp;

        QJsonArray likesArray;
        for (const QString& name : likedBy) {
            likesArray.append(name);
        }
        obj["likedBy"] = likesArray;

        return obj;
    }

	// Static method to create a Post object from a QJsonObject
    static Post fromJson(const QJsonObject& obj) {
        QStringList likesList;
        QJsonArray likesArray = obj["likedBy"].toArray();
        for (const QJsonValue& value : likesArray) {
            likesList.append(value.toString());
        }

        return Post(
            obj["username"].toString("Unknown"),
            obj["content"].toString("No Content"),
            obj["timestamp"].toString("Long ago"),
            likesList
        );
    }
};