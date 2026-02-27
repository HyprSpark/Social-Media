#pragma once
// -- Libraries --
#include <QString>
#include <QJsonObject>

class User
{
public:
    QString username;
    QString email;
    QString password;

    User() {}

    User(const QString& username,
        const QString& email,
        const QString& password)
        : username(username), email(email), password(password) {
    }

    QJsonObject toJson() const
    {
        QJsonObject obj;
        obj["username"] = username;
        obj["email"] = email;
        obj["password"] = password;
        return obj;
    }

    static User fromJson(const QJsonObject& obj)
    {
        return User(
            obj["username"].toString(),
            obj["email"].toString(),
            obj["password"].toString()
        );
    }
};