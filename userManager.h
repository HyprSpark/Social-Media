#pragma once
// -- Headers --
#include "models/User.h"

// -- Libraries --
#include <QVector>


class UserManager
{
public:
    static QVector<User> loadUsers();
    static void saveUser(const User& user);
    static bool authenticate(const QString& email, const QString& password);
};