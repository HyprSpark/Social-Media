#pragma once 

// -- Headers --
#include <QString>
#include <QStringList>
#include <QJsonObject>
#include <QJsonArray>

/**
 * @brief The User class represents a registered account in the system.
 * It stores personal credentials and manages "Friend" relationships.
 */
class User
{
public:
    // --- Data Members ---
    QString username;
    QString email;
    QString password;
    QStringList friends;

    // --- Constructors ---

    // Default constructor: Creates an empty user with no data
    User() {}

    // Parameterized constructor for creating a new user (e.g., during Sign-Up)
    User(const QString& username, const QString& email, const QString& password)
        : username(username), email(email), password(password) {
    }

    /**
     * @brief Converts the C++ User object into a QJsonObject.
     * This allows the user's data and friends list to be saved to User.json.
     */

    QJsonObject toJson() const
    {
        QJsonObject obj;
        obj["username"] = username;
        obj["email"] = email;
        obj["password"] = password;

        // Convert the QStringList of friends into a JSON Array
        QJsonArray friendsArray;
        for (const QString& friendName : friends) {
            friendsArray.append(friendName);
        }
        obj["friends"] = friendsArray;

        return obj;
    }

    /**
     * @brief Creates a User object from a QJsonObject.
     * @param obj The JSON object containing user data.
     * @return A fully populated User instance.
     */

    static User fromJson(const QJsonObject& obj)
    {
        User u;
        u.username = obj["username"].toString("Unknown");
        u.email = obj["email"].toString("No Email");
        u.password = obj["password"].toString("");

        // Extract the friends list from the JSON array
        QJsonArray friendsArray = obj["friends"].toArray();
        for (const QJsonValue& value : friendsArray) {
            u.friends.append(value.toString());
        }

        return u;
    }

    // --- Getters ---

    /** @brief Returns the user's unique identifier. */
    QString getUsername() const {
        return username;
    }
};