#pragma once
#include "models/User.h"
#include <QMainWindow>
#include "ui_Profile.h"

class Profile : public QMainWindow
{
	Q_OBJECT

public:
	Profile( QWidget *parent = nullptr);
	~Profile();

	void setActiveUser(const User& user);

private:
	Ui::ProfileClass ui;
	User currentUser;

private slots:
	void onReturnClicked();
};

