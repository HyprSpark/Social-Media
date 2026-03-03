// -- Headers --
#include "UserManager.h"

// -- Libraries --
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>

// - Accessing the file and loading user details (returns empty if file not found)
QVector<User> UserManager::loadUsers()
{
    QVector<User> users;

    QFile file(":/resources/Users.json"); // Check and open user file (r)
    if (!file.open(QIODevice::ReadOnly))
        return users;

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll()); // Read file for all users
    QJsonArray array = doc.array();

    for (const auto& value : array)
        users.append(User::fromJson(value.toObject())); 

    return users; // Allow the login page to compare
}

// - Adding users to the user file to allow for logging in
void UserManager::saveUser(const User& user) 
{
    // Load user list and add new entry 
    QVector<User> users = loadUsers(); // This prevent data from being overwrote

    QJsonArray array;
    for (const auto& u : users)
        array.append(u.toJson());

    QFile file("users.json"); // Access and add to file (w)
    file.open(QIODevice::WriteOnly);
    file.write(QJsonDocument(array).toJson()); // Save for other sessions
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