#pragma once

#include <QWidget>
#include "ui_Posts.h"

class Posts : public QWidget
{
	Q_OBJECT

public:
	Posts(QWidget *parent = nullptr);
	~Posts();

private:
	Ui::PostsClass ui;
};

