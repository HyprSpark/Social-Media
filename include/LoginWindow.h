#pragma once

// -- Headers --
#include "ui_LoginWindow.h"


// -- Libraries --
#include <QMainWindow>


class LoginWindow : public QMainWindow
{
	Q_OBJECT

public:
	LoginWindow(QWidget *parent = nullptr);
	~LoginWindow();

private:
	Ui::LoginWindowClass ui;
	QPixmap m_hero;

private slots:
	void onSignInClicked();
	void onSignUpClicked();

};

