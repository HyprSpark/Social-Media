#pragma once

// -- Header -- //
#include <QWidget>
#include "ui_Posts.h" // Pointer to the visual elements for QT
#include "Content.h"

/**
* @brief The Posts class represents an individual post in the feed.
* It allows users to interact with the post by liking and unliking.
* It also provides the original user (OP) the ability to delete their post.
*/

class Posts : public QWidget
{
	Q_OBJECT

public:
	Posts(QWidget* parent = nullptr);
	~Posts();

	/**
	* @brief Populates the post widget with the provided content data and the current logged in user.
	* This function is responsible for updating the UI elements of the post based on the content data, such as setting the username, content text, timestamp, and like count.
	*/
	void setPostData(const Content& data, const QString& currentLoggedInUser);

private:
	Ui::PostsClass ui;
	Content currentData; // Local copy of the post's data.
	QString currentUser; // Who is currently viewing the post

	// Tracks the tpgg;e state of the like button for this post.
	bool isLiked = false;

private slots:
	void onUsernameClicked(); // Opens the profile window of the OP when their username is clicked.
	void onLikeClicked(); // Handles the logic for liking and unliking a post, including updating the like count and the visual state of the like button.
	void onDeleteClicked(); // If the current user is the OP, this function allows them to delete their post.
};

