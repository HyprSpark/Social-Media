#pragma once

// -- Headers --
#include "models/User.h"
#include <QVector>

/**
* @brief The UserManager class is used for handling all user-data related operations
* This includes loading and saving user data to a JSON file
* It also includes data validation for different user operations.
*/

class UserManager
{
public:
    // Reads the user file
    static QVector<User> loadUsers(); 

	static void saveUser(const User& user);  // Appends a new user to the existing list of users and saves it back to the file.

    static bool authenticate(const QString& email, const QString& password, User& outUser); // Check if the provided details match an existing user.

    static bool emailExists(const QString& email); // Check if the provided email already exists

	static bool isUnique(const QString& username, const QString& email); // Check if both the username and email are unique

	static bool userExists(const QString& username); // Check if the user exists when trying to send a message.

	static void toggleFollowing(const QString& currentUsername, const QString& targetUsername); // Adds the target user to the current user's friend list if they aren't already friends.

};