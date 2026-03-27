#pragma once

#include <QWidget>
#include "ui_Posts.h"
#include "Content.h"

class Posts : public QWidget
{
	Q_OBJECT

public:
	Posts(QWidget* parent = nullptr);
	~Posts();

	void setPostData(const Content& data, const QString& currentLoggedInUser);

private:
	Ui::PostsClass ui;
	Content currentData;
	QString currentUser;
	bool isLiked = false;

private slots:
	void onUsernameClicked();
	void onLikeClicked();
	void onDeleteClicked();
};

