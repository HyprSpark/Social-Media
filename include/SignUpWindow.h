#pragma once

// -- Headers --
#include "ui_SignUpWindow.h" // Pointer to the visual elements for QT
#include <QMainWindow>

/**
* @brief This window allows the user to make a new account
* It captures the username, email, and password, checking if the username/email doesn't already exist
* Checks for proper email formatting and password confirmation before creating the account
*/

class SignUpWindow : public QMainWindow
{
	Q_OBJECT

public:
	SignUpWindow(QWidget *parent = nullptr);
	~SignUpWindow();

private:
	Ui::SignUpWindowClass ui;

private slots:
	void onLogInClicked(); // Returns to the login window.
	void onCreateAccClicked(); // If details are valid, create a new account.
};

