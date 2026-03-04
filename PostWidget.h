#pragma once

#include <QMainWindow>
#include "ui_PostWidget.h"

class PostWidget : public QMainWindow
{
	Q_OBJECT

public:
	PostWidget(QWidget *parent = nullptr);
	~PostWidget();

private:
	Ui::PostWidgetClass ui;
};

