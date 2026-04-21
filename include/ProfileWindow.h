#pragma once

// -- Header -- //
#include "models/User.h" // Note: Ensure this path matches your project structure
#include <QMainWindow>
#include "ui_ProfileWindow.h" // Pointer to the visual elements for QT

/**
 * @brief The Profile class represents the user profile interface of the application.
 * This window displays the user's name and a list of their posts.
 */
class ProfileWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit ProfileWindow(QWidget* parent = nullptr);
	~ProfileWindow();

	/**
	* @brief Receives the User object from the feedWindow.
	* @param user the currently authenticated user.
	* @param viewer the person currently looking at the screen.
	*/
	void setActiveUser(const User& user, const User& viewer);
	
private:
	Ui::ProfileWindow ui;
	User viewedUser;           // FIXED: Matches the .cpp file
	User loggedInUser;         // FIXED: Matches the .cpp file
	bool isAlreadyFollowing = false;

private slots:
	void onReturnClicked(); // Returns to the feed window.
	void loadUserPosts(); // Loads the posts into the scroll area.
	void onFollowClicked(); // Toggles the follow/unfollow state and updates the followers count accordingly.

signals:
	void windowClosed();

protected:
	void closeEvent(QCloseEvent* event) override; // Override the close event to emit a signal
};