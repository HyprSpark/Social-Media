#pragma once
// -- Headers --
#include "models/User.h"

// -- Libraries --
#include <QVector>


class UserManager
{
public:
    // Reads the user file
    static QVector<User> loadUsers(); 

    // Write the entered 'sign up' to JSON file
    static void saveUser(const User& user); 

    // Check if entered login data enetred matches saved data (returns true/false)
    static bool authenticate(const QString& email, const QString& password); 
};