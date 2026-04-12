#pragma once

// -- Header -- //
#include <QMainWindow>
#include "ui_MessagesWindow.h" // Pointer to the visual elements for QT
#include "models/User.h"

/**
* @brief The message class handles the private messaging interface.
* It allows users to read, send, and manage their inbox.
*/

class MessagesWindow : public QMainWindow
{
	Q_OBJECT

public:
	MessagesWindow(QWidget* parent = nullptr);
	~MessagesWindow();

	/**
	* @brief Recieves the User object from the feedWindow.
	* @param user the currently authenticated user.
	* This is used to identify the user trying to send or receive messages.
	*/
	void setActiveUser(const User& user);

private:
	Ui::MessagesWindow ui;
	User currentUser;

private slots:
	// -- UI Button Functions -- //

	void onSendClicked(); // Captures text entered from input and sends it to the intended recipient if they exist.
	void onReturnClicked(); // Returns to the feed window without closing the application.
	void onReadClicked(); // Reads the most recent message from the inbox and displays it.
	void onClearInboxClicked(); // Clears the inbox of the user, deleting all messages permanently.
};