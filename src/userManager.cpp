// -- Headers --
#include "UserManager.h"
#include <QFile>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonArray>
#include <QCoreApplication>

QVector<User> UserManager::loadUsers()
{
    QVector<User> users;

    // Standardizing path to look for resources in the project head
    QString filePath = QCoreApplication::applicationDirPath() + "/../../resources/User.json";
    QFile file(filePath);

    // Fallback mechanism
    if (!file.exists()) {
        qDebug() << "[DEBUG] PERSISTENCE: Local User.json not found at project head, trying internal resource...";
        file.setFileName(":/resources/User.json");
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "[ERROR] PERSISTENCE: Could not open user database. Path:" << file.fileName();
        return users;
    }

    qDebug() << "[PERSISTENCE] Successfully opened user database:" << file.fileName();
    const QByteArray data = file.readAll();
    file.close();

    QJsonParseError parseError;
    const QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
    if (parseError.error != QJsonParseError::NoError) {
        qDebug() << "[ERROR] JSON: Parse failed for User.json -" << parseError.errorString();
        return users;
    }

    if (!doc.isArray()) {
        qDebug() << "[ERROR] JSON: User document is not a valid array.";
        return users;
    }

    const QJsonArray array = doc.array();
    for (const auto& value : array)
        users.append(User::fromJson(value.toObject()));

    qDebug() << "[DEBUG] Auth: Loaded" << users.size() << "users into memory cache.";
    return users;
}

void UserManager::saveUser(const User& user)
{
    qDebug() << "[INFO] Auth: Registering new user record for" << user.username;

    QVector<User> users = loadUsers();
    users.append(user);

    QJsonArray array;
    for (const auto& u : users) {
        array.append(u.toJson());
    }

    QString filePath = QCoreApplication::applicationDirPath() + "/../../resources/User.json";
    QFile file(filePath);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        qDebug() << "[ERROR] PERSISTENCE: Could not write to User.json! Reason:" << file.errorString();
        return;
    }

    file.write(QJsonDocument(array).toJson());
    file.close();

    qDebug() << "[SUCCESS] PERSISTENCE: Saved new user. Total users in database:" << users.size();
}

bool UserManager::authenticate(const QString& email, const QString& password, User& outUser)
{
    qDebug() << "[INFO] Auth: Verifying credentials for email:" << email;
    QVector<User> users = loadUsers();

    for (const User& user : users)
    {
        if (user.email == email && user.password == password)
        {
            qDebug() << "[SUCCESS] Auth: Match found for" << user.username;
            outUser = user;
            return true;
        }
    }

    qDebug() << "[ERROR] Auth: No matching credentials found in database.";
    return false;
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
    qDebug() << "[DEBUG] Auth: Checking uniqueness for" << username << "/" << email;
    QVector<User> users = loadUsers();
    for (const User& u : users) {
        if (u.username.compare(username, Qt::CaseInsensitive) == 0) return false;
        if (u.email.compare(email, Qt::CaseInsensitive) == 0) return false;
    }
    return true;
}

bool UserManager::userExists(const QString& username)
{
    QVector<User> users = loadUsers();
    for (const User& u : users) {
        if (u.getUsername() == username) return true;
    }
    return false;
}

void UserManager::toggleFollowing(const QString& followerName, const QString& targetName) {
    qDebug() << "[INFO] Social: Toggling relationship between" << followerName << "and" << targetName;

    QVector<User> users = loadUsers();
    bool changed = false;

    for (User& u : users) {
        if (u.username == followerName) {
            if (u.following.contains(targetName)) {
                u.following.removeAll(targetName);
                qDebug() << "[DEBUG] Social:" << followerName << "unfollowed" << targetName;
            }
            else {
                u.following.append(targetName);
                qDebug() << "[DEBUG] Social:" << followerName << "followed" << targetName;
            }
            changed = true;
        }
        if (u.username == targetName) {
            if (u.followers.contains(followerName)) u.followers.removeAll(followerName);
            else u.followers.append(followerName);
            changed = true;
        }
    }

    if (changed) {
        QString filePath = QCoreApplication::applicationDirPath() + "/../../resources/User.json";
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            QJsonArray arr;
            for (const auto& u : users) arr.append(u.toJson());
            file.write(QJsonDocument(arr).toJson());
            file.close();
            qDebug() << "[SUCCESS] PERSISTENCE: User.json social graph updated on disk.";
        }
    }
}