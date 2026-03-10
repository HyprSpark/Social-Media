// -- Headers --
#include "UserManager.h"

// -- Libraries --
#include <QFile>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>

// - Accessing the file and loading user details (returns empty if file not found)
QVector<User> UserManager::loadUsers()
{
    QVector<User> users;

    QFile file("resources/User.json");
    if (!file.exists()) {
        qDebug() << "LOG: Local users.json not found, trying resource...";
        file.setFileName(":/resources/User.json"); // match the resource file shipped in project
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "ERROR: Could not open file. Path:" << file.fileName();
        return users;
    }

    qDebug() << "SUCCESS: File opened:" << file.fileName();
    const QByteArray data = file.readAll();
    file.close();

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

// - Adding users to the user file to allow for logging in
void UserManager::saveUser(const User& user) 
{
    // Load user list and add new entry 
    QVector<User> users = loadUsers(); // This prevent data from being overwrote

    users.append(user);

    QJsonArray array;
    for (const auto& u : users)
        array.append(u.toJson());

    QFile file("resources/User.json");
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "ERROR: Could not open file for writing:" << file.errorString();
        return;
    }

    file.write(QJsonDocument(array).toJson());
    file.close();

    qDebug() << "SUCCESS: Saved new user. Total users now:" << users.size();
}

// Checking if the user details match saved data
bool UserManager::authenticate(const QString& email, const QString& password) // Phonebook style (Could change this in the future)
{
    QVector<User> users = loadUsers();

    for (const User& user : users)
    {
        if (user.email == email && user.password == password) // If details match let through
            return true;
    }

    return false; // Block user and send message
}

bool UserManager::emailExists(const QString& email)
{
    QVector<User> users = loadUsers();
    for (const User& u : users) {
        if (u.email.trimmed().toLower() == email.trimmed().toLower()) {
            return true;
        }
    }
    return false;
}

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
    return true;
}