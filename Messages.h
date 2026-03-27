#pragma once

#include "models/User.h"
#include <QMainWindow>
#include "ui_Messages.h"

class Messages : public QMainWindow
{
	Q_OBJECT

public:
	Messages(QWidget* parent = nullptr);
	~Messages();

	// This allows FeedWindow to pass the logged-in user to this screen
	void setActiveUser(const User& user);

private:
	Ui::MessagesClass ui;
	User currentUser;
	Messages* messagesWindow = nullptr;

private slots:
	void onSendClicked();
	void onReturnClicked();
	void onReadClicked();
	void onClearInboxClicked();
};