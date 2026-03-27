#pragma once

#include <QMainWindow>
#include "ui_FeedWindow.h"
#include "models/User.h"

// Forward declaration so the compiler knows this class exists
class Messages;

class FeedWindow : public QMainWindow
{
	Q_OBJECT

public:
	FeedWindow(QWidget* parent = nullptr);
	~FeedWindow();

	// Function to receive the logged-in user from the Login screen
	void setActiveUser(const User& user);
	void loadPosts();

private:
	Ui::FeedWindowClass ui;
	User currentUser;

	// This pointer keeps track of the messages window so we don't open 100 of them
	Messages* messagesWindow = nullptr;

private slots:
	void onMyProfileClicked();
	void onSubmitPostClicked();
	void onMessagesClicked();
	void onQuitClicked();
	void onSignOutClicked();
};