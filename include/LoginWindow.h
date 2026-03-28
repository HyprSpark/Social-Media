#pragma once

// -- Header -- //
#include "ui_LoginWindow.h" // Pointer to the visual elements for QT
#include <QMainWindow>

/**
* @brief The LoginWindow class represents the initial login screen of the application.
* It handles the gatekeeping logic before a user can access the Feed.
* Allows the program to recognise who is logging in and pass that to the FeedWindow.
*/
class LoginWindow : public QMainWindow
{
	Q_OBJECT

public:
	LoginWindow(QWidget *parent = nullptr);
	~LoginWindow();

private:
	Ui::LoginWindowClass ui;
	/**
	* @brief A QPixmap is used to store and display login background image.
	* This allows a large background image for the login screen 
	*/
	QPixmap m_hero;

private slots:
	// -- UI Button Functions -- //

	void onSignInClicked(); // Handles the logic for when the "Sign In" button is clicked, including validating credentials and opening the FeedWindow if successful.
	void onSignUpClicked(); // Handles the logic for when the "Sign Up" button is clicked, including opening the SignUpWindow where users can create a new account.

};

