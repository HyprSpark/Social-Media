#pragma once // Prevents multiole loads

// -- Libraries --
#include <QString>
#include <QJsonObject>

class User
{
public:
    QString username;
    QString email;
    QString password;

    User() {} // Create an empty user w/ no data

    User(const QString& username,
        const QString& email,
        const QString& password)
        : username(username), email(email), password(password) {
    }

    QJsonObject toJson() const // tells the C++ to work in JSON
    {
        QJsonObject obj;
        obj["username"] = username;
        obj["email"] = email;
        obj["password"] = password;
        return obj;
    }

    static User fromJson(const QJsonObject& obj) // Converts JSON to  C++ as a 'user' object, allowing c++ to readit
    {
        return User(
            obj["username"].toString("Unknown"), // Defaults to said value if missing
            obj["email"].toString("No Email"),
            obj["password"].toString("")
        );
    }

    QString getUsername() const {
        return username;
    }

};