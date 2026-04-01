// -- Headers --
#include "UserManager.h"
#include <QFile>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>

/**
* @brief Loads the user data from a JSON file and returns a list of User objects.
* opens the file, checks if it exists, and parses the JSON content to create User instances.
*/

QVector<User> UserManager::loadUsers()
{
    QVector<User> users;

    QFile file("resources/User.json");

    // Fallback mechanism: If the file doesn't exist in the expected location try to load from resources
    if (!file.exists()) {
        qDebug() << "LOG: Local users.json not found, trying resource...";
        file.setFileName(":/resources/User.json");
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "ERROR: Could not open file. Path:" << file.fileName();
        return users;
    }

    qDebug() << "SUCCESS: File opened:" << file.fileName();
    const QByteArray data = file.readAll();
    file.close();

	// -- Parse the JSON data --
    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "ERROR: JSON parse failed:" << parseError.errorString();
        return users;
    }

    if (!doc.isArray()) {
        qDebug() << "ERROR: JSON document is not an array.";
        return users;
    }

    const QJsonArray array = doc.array();
    for (const auto& value : array)
        users.append(User::fromJson(value.toObject()));

    return users;
}

/**
* @brief saves a new user to the JSON file.
* It first loads the existing users to prevent overwriting, appends the new user, and then writes the entire list back to the file.
*/
void UserManager::saveUser(const User& user) 
{
	// 1. Load existing users to prevent overwriting
    QVector<User> users = loadUsers(); 
    users.append(user);

	// 2. Convert the list of users to a JSON array
    QJsonArray array;
    for (const auto& u : users) {
        array.append(u.toJson());
    }

	// 3. Write the JSON array back to the file
    QFile file("resources/User.json");
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "ERROR: Could not open file for writing:" << file.errorString();
        return;
    }

    file.write(QJsonDocument(array).toJson());
    file.close();

    qDebug() << "SUCCESS: Saved new user. Total users now:" << users.size();
}

/**
* @brief Authenticates a user by checking the provided email and password against the stored user data.
*/


bool UserManager::authenticate(const QString& email, const QString& password, User& outUser)
{
    QVector<User> users = loadUsers();

    for (const User& user : users)
    {
        if (user.email == email && user.password == password)
        {
            outUser = user;
            return true;
        }
    }

	return false; // Access denied if no match is found
}

/**
* @brief Checks if the provided email already exists in the user database.
*/

bool UserManager::emailExists(const QString& email)
{
    QVector<User> users = loadUsers();
    for (const User& u : users) {
        if (u.email.trimmed().toLower() == email.trimmed().toLower()) {
            return true;
        }
    }
	return false; // No match found, email is available
}

/**
* @brief Checks if both the username and email are unique in the user database.
*/

bool UserManager::isUnique(const QString& username, const QString& email)
{
    QVector<User> users = loadUsers();
    for (const User& u : users) {
        // compare() returns 0 if the strings match
        if (u.username.compare(username, Qt::CaseInsensitive) == 0) {
            return false;
        }
        if (u.email.compare(email, Qt::CaseInsensitive) == 0) {
            return false;
        }
    }
	return true; // Both username and email are unique
}

/**
* @brief Checks if a user with the given username exists in the user database.
*/

bool UserManager::userExists(const QString& username)
{
    // 1. Load the current list of users
    QVector<User> users = loadUsers();

    // 2. Loop through the users to see if there's a match
    for (const User& u : users) {
        if (u.getUsername() == username) {
            return true; // Found the user!
        }
    }

    // 3. If the loop finishes without finding a match, the user doesn't exist
    return false;
}

/**
 * @brief Toggles the friendship status between two users.
 * If they are friends, it removes them. If not, it adds them.
 */

void UserManager::toggleFollow(const QString& currentUsername, const QString& targetUsername) {
    QVector<User> users = loadUsers();
    bool changed = false;

    for (User& u : users) {
        if (u.username == currentUsername) {
            if (u.following.contains(targetUsername)) {
                u.following.removeAll(targetUsername); // Unfriend logic
            }
            else {
                u.following.append(targetUsername);    // Friend logic
            }
            changed = true;
            break;
        }
    }

    if (changed) {
        QFile file("resources/User.json");
        if (file.open(QIODevice::WriteOnly)) {
            QJsonArray arr;
            for (const auto& u : users) arr.append(u.toJson());
            file.write(QJsonDocument(arr).toJson());
            file.close();
        }
    }
}