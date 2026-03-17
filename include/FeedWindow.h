#pragma once
#include "models/User.h"

#include <QMainWindow>
#include "ui_FeedWindow.h"

class FeedWindow : public QMainWindow
{
	Q_OBJECT

public:
	FeedWindow(QWidget *parent = nullptr);
	~FeedWindow();

	void setActiveUser(const User& user);

private:
	Ui::FeedWindowClass ui;
	User currentUser;

private slots:
	void onMyProfileClicked();
};

