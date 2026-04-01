#pragma once

// -- Header -- //
#include "models/User.h" // Note: Ensure this path matches your project structure
#include <QMainWindow>
#include "ui_Profile.h" // Pointer to the visual elements for QT

/**
 * @brief The Profile class represents the user profile interface of the application.
 * This window displays the user's name and a list of their posts.
 */
class Profile : public QMainWindow
{
	Q_OBJECT

public:
	Profile(QWidget* parent = nullptr);
	~Profile();

	/**
	* @brief Receives the User object from the feedWindow.
	* @param user the currently authenticated user.
	* @param viewer the person currently looking at the screen.
	*/
	void setActiveUser(const User& user, const User& viewer);

private:
	Ui::ProfileClass ui;
	User viewedUser;           // FIXED: Matches the .cpp file
	User loggedInUser;         // FIXED: Matches the .cpp file
	bool isAlreadyFollowing = false;

private slots:
	void onReturnClicked();
	void loadUserPosts();
	void onFollowClicked();
};