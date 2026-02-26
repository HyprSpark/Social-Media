#pragma once

#include <QMainWindow>
#include "ui_LoginWindow.h"

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

};

