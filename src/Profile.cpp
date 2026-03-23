#include "Profile.h"
#include "FeedWindow.h"
#include "Posts.h"
#include <QWidget>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

Profile::Profile(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	QVBoxLayout* layout = new QVBoxLayout(ui.scrollProfilePosts);
	layout->setAlignment(Qt::AlignTop);

	connect(ui.btnToFeed, &QPushButton::clicked,
		this, &Profile::onReturnClicked);

}

Profile::~Profile()
{}

void Profile::setActiveUser(const User& user) {
	currentUser = user;
	ui.lblUsername->setText(currentUser.username);

	loadUserPosts();
}

void Profile::onReturnClicked() {
	this->hide(); 
}

void Profile::loadUserPosts() {
	QFile file("resources/posts.json");
	if (file.open(QIODevice::ReadOnly)) {
		QByteArray data = file.readAll();
		file.close();
		QJsonDocument doc = QJsonDocument::fromJson(data);
		QJsonArray postsArray = doc.array();
		for (const QJsonValue& value : postsArray) {
			QJsonObject obj = value.toObject();
			if (obj["username"].toString() == currentUser.username) {
				Content postData;
				postData.username = obj["username"].toString();
				postData.content = obj["content"].toString();
				postData.likes = obj["likes"].toInt();
				Posts* postWidget = new Posts(this);
				postWidget->setPostData(postData);
				ui.scrollProfilePosts->layout()->addWidget(postWidget);
			}
		}
	}
}
