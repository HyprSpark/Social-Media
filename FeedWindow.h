#pragma once

#include <QMainWindow>
#include "ui_FeedWindow.h"

class FeedWindow : public QMainWindow
{
	Q_OBJECT

public:
	FeedWindow(QWidget *parent = nullptr);
	~FeedWindow();

private:
	Ui::FeedWindowClass ui;
};

