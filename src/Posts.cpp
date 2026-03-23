#include "Posts.h"
#include "Content.h"
#include "Profile.h"
#include <QPushButton>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

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
	currentData = data;
	ui.btnUsername->setText(data.username);
	ui.lblContent->setText(data.content);
	ui.lblLikeCount->setText(QString::number(data.likes));
}
void Posts::onUsernameClicked()
{
	Profile* profile = new Profile(this);
	User clickedUser;
	clickedUser.username = currentData.username;
	profile->setActiveUser(clickedUser);
	profile->show(); // Displays the profile window
}
void Posts::onLikeClicked()
{
	if (hasLiked) {
		return; // Prevent multiple likes
	}

	currentData.likes++;
	ui.lblLikeCount->setText(QString::number(currentData.likes));
	ui.btnLike->setEnabled(false); // Disable the like button after liking
	hasLiked = true;

	QFile file("resources/posts.json");

	if (file.open(QIODevice::ReadOnly)) {
		QByteArray data = file.readAll();
		file.close();
		QJsonDocument doc = QJsonDocument::fromJson(data);
		QJsonArray postsArray = doc.array();
		for (int i = 0; i < postsArray.size(); ++i) {
			QJsonObject obj = postsArray[i].toObject();
			if (obj["username"].toString() == currentData.username && obj["content"].toString() == currentData.content) {
				obj["likes"] = currentData.likes; // Update the like count
				postsArray[i] = obj; // Update the array with the modified object
				break;
			}
		}
		if (file.open(QIODevice::WriteOnly)) {
			QJsonDocument updatedDoc(postsArray);
			file.write(updatedDoc.toJson());
			file.close();
		}
	}

}

