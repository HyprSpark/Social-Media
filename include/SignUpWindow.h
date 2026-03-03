#pragma once // Prevents multiple loading

// -- Headers --
#include "ui_SignUpWindow.h"

// -- Libraries --
#include <QMainWindow>


class SignUpWindow : public QMainWindow
{
	Q_OBJECT

public:
	SignUpWindow(QWidget *parent = nullptr);
	~SignUpWindow();

private:
	Ui::SignUpWindowClass ui;

private slots:
	void onLogInClicked();
};

