#pragma once

// -- Header -- //
#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>

/**
* @brief Used to handle a post, data storage and conversion to/from a JSON file
*/

class Content {
public:
    
	// -- Data members --
	QString username; // User who posted the content
	QString content; // The actual content of the post
	QStringList likedBy; // List of usernames who liked the post
	QString timestamp; // Time when the post was created

	// Default constructor
    Content() {}

	// Parameterized constructor
    Content(const QString& username, const QString& content, const QStringList& likedBy, const QString& timestamp)
        : username(username), content(content), likedBy(likedBy), timestamp(timestamp) {
    }

    /**
	* @brief Converts the Content object to a QJsonObject for JSON serialization
    */

    QJsonObject toJson() const {
        QJsonObject obj;
        obj["username"] = username;
        obj["content"] = content;
        obj["timestamp"] = timestamp;

		// Convert QStringList to QJsonArray
        QJsonArray likesArray;
        for (const QString& name : likedBy) likesArray.append(name);
        obj["likedBy"] = likesArray;

        return obj;
    }

    static Content fromJson(const QJsonObject& obj) {
		// Extract the list of likes from the JSON array
        QStringList likesList;
        QJsonArray likesArray = obj["likedBy"].toArray();
        for (const QJsonValue& value : likesArray) likesList.append(value.toString());

		// Create and return a Content object using the extracted data
		// Use default values if certain keys are missing in the JSON object
        return Content(
            obj["username"].toString("Unknown"),
            obj["content"].toString("No Content"),
            likesList,
            obj["timestamp"].toString("Long ago") // Load from JSON
        );
    }
};