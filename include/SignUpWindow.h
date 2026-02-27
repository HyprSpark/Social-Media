#pragma once

#include <QMainWindow>
#include "ui_SignUpWindow.h"

class SignUpWindow : public QMainWindow
{
	Q_OBJECT

public:
	SignUpWindow(QWidget *parent = nullptr);
	~SignUpWindow();

private:
	Ui::SignUpWindowClass ui;
};

