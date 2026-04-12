#pragma once

// -- Header -- //
#include <QMainWindow>
#include "ui_FeedWindow.h" // Pointer to the visual elements for QT
#include "models/User.h"

/**
* @brief The FeedWindow class represents the main interface of the application.
* Allows users to view posts, create new posts, access their profile, and manage their messages.
*/

// Forward declaration: Tells the compiler that this class exists,
// Without needing to include the entire header file, reducing complication.
class MessagesWindow;

class FeedWindow : public QMainWindow
{
	Q_OBJECT

public:
	// Constructor & Destructor used for UI initialization.
	FeedWindow(QWidget* parent = nullptr);
	~FeedWindow();

	/**
	* @brief Stores the user who just logged in.
	* This allows the feed to know who is currenty using the app
	*/
	void setActiveUser(const User& user);

	// Refeshes the UI by pulling posts from a file.
	void loadPosts();

private:
	Ui::FeedWindowClass ui; // POinter to the visual elements for QT
	User currentUser; // The data object representing the currently logged in user.

	/**
	* @brief Pointer to the Messages window.
	* Keeing this as a member prevents multiple windows from being opened.
	* It will check beofre creating a new window and will reuse the existing one if it's already open.
	*/
	MessagesWindow* messagesWindow = nullptr;

private slots:
	// -- UI Button Functions -- //

	void onMyProfileClicked(); // Opens the profile window for the active user
	void onSubmitPostClicked(); // Takes the text from the "Create Post" box and adds it to the feed
	void onMessagesClicked(); // Opens the secure messaging window
	void onQuitClicked(); // Safely shuts down the entire application when the "Quit" button is clicked
	void onSignOutClicked(); // Signs the user out and returns to the login screen when the "Sign Out" button is clicked
	void onSortSelect(int index); // Sorts the feed based on the selected criteria (e.g., "Most Recent", "Most Liked")
	void updateMessageButtonVisuals(); // Checks for new messages and updates the "Messages" button appearance accordingly

protected:
	void showEvent(QShowEvent* event) override; // Detects when the window is shown again
	bool event(QEvent* e) override;
};