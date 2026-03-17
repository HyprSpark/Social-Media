#include "Posts.h"
#include "Content.h"
#include "Profile.h"
#include <QPushButton>

Posts::Posts(QWidget* parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	connect(ui.btnLike, &QPushButton::clicked, this, &Posts::onLikeClicked);
	connect(ui.btnUsername, &QPushButton::clicked, this, &Posts::onUsernameClicked);
}

Posts::~Posts()
{
}

void Posts::setPostData(const Content& data)
{
	ui.btnUsername->setText(data.username);
	ui.lblContent->setText(data.content);
	ui.lblLikeCount->setText(QString::number(data.likes));
}
void Posts::onUsernameClicked()
{
	Profile* profile = new Profile(this);
	profile->///loadUserProfile/// NOT CREATED YET, but will load the profile data based on the username passed in
		(currentData.username);
	profile->show(); // Displays the profile window
	this->hide();   // hides feed while profile is open
}
void Posts::onLikeClicked()
{
	currentData.likes++;
	ui.lblLikeCount->setText(QString::number(currentData.likes));
}

