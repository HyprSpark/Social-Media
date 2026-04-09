#include "Posts.h"
#include "Content.h"
#include "Profile.h"
#include "userManager.h"
#include <QVector>
#include <QDebug>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QPushButton>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QPixmap>         
#include <QCoreApplication> 
#include <QStyle>
#include <QMessageBox>

Posts::Posts(QWidget* parent)
	: QWidget(parent), isLiked(false)
{
	ui.setupUi(this);

	this->setStyleSheet(
		"Posts { "
		"   border: 1px solid #444; "
		"   border-radius: 8px; "
		"   background-color: #222; "
		"   margin-bottom: 10px; "
		"   padding: 5px; "
		"}"
	);

	connect(ui.btnLike, &QPushButton::clicked, this, &Posts::onLikeClicked);
	connect(ui.btnUsername, &QPushButton::clicked, this, &Posts::onUsernameClicked);
	connect(ui.btnDelete, &QPushButton::clicked, this, &Posts::onDeleteClicked);
}

Posts::~Posts() {}

void Posts::setPostData(const Content& data, const QString& currentLoggedInUser)
{
	currentData = data;
	currentUser = currentLoggedInUser;

	ui.btnUsername->setText(data.username);
	ui.lblContent->setText(data.content);
	ui.lblTimestamp->setText(data.timestamp);
	ui.lblTimestamp->setStyleSheet("color: #666; font-size: 10px;");

	ui.lblLikeCount->setText(QString::number(data.likedBy.size()));

	if (data.likedBy.contains(currentUser)) {
		isLiked = true;
		ui.btnLike->setText("Unlike");
		ui.btnLike->setStyleSheet("color: #ff4757; font-weight: bold;");
	}
	else {
		isLiked = false;
		ui.btnLike->setText("Like");
		ui.btnLike->setStyleSheet("");
	}

	QPixmap avatar(":resources/images/profile.png");
	if (avatar.isNull()) {
		ui.lblProfilePic->setText("?");
	}
	else {
		ui.lblProfilePic->setStyleSheet("background: transparent; border: none;");
		QPixmap scaledAvatar = avatar.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		ui.lblProfilePic->setPixmap(scaledAvatar);
		ui.lblProfilePic->setFixedSize(50, 50);
		ui.lblProfilePic->setAlignment(Qt::AlignCenter);
	}

	if (data.username.trimmed().toLower() == currentLoggedInUser.trimmed().toLower()) {
		ui.btnDelete->setVisible(true);
	}
	else {
		ui.btnDelete->setVisible(false);
	}
}

void Posts::onDeleteClicked()
{
	auto reply = QMessageBox::question(this, "Delete Post", "Delete this post?");
	if (reply != QMessageBox::Yes) return;

	QString filePath = QCoreApplication::applicationDirPath() + "/../../resources/posts.json";
	QFile file(filePath);

	if (!file.open(QIODevice::ReadOnly)) return;

	QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
	file.close();

	QJsonArray postsArray = doc.array();
	QJsonArray updatedArray;
	bool matchFound = false;

	for (const QJsonValue& value : postsArray) {
		QJsonObject obj = value.toObject();
		if (obj["username"].toString().trimmed() == currentData.username &&
			obj["content"].toString().trimmed() == currentData.content &&
			obj["timestamp"].toString().trimmed() == currentData.timestamp) {
			matchFound = true;
		}
		else {
			updatedArray.append(obj);
		}
	}

	if (matchFound) {
		if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
			file.write(QJsonDocument(updatedArray).toJson());
			file.close();
		}
	}

	this->deleteLater();
}

/**
 * @brief Opens the profile window of the OP when their username is clicked.
 */
void Posts::onUsernameClicked()
{
	Profile* profile = new Profile();

	// 1. Get the full list of users from the database
	QVector<User> allUsers = UserManager::loadUsers();

	User author;
	User viewer;
	bool authorFound = false;

	// 2. Find the actual Author and Viewer objects in the list
	for (const User& u : allUsers) {
		if (u.username == currentData.username) {
			author = u; 
			authorFound = true;
		}
		if (u.username == currentUser) {
			viewer = u; 
		}
	}

	// 3. If for some reason the author isn't in User.json, fallback (safety check)
	if (!authorFound) {
		author.username = currentData.username;
	}

	// 4. Pass the fully-populated objects to the profile
	profile->setActiveUser(author, viewer);
	profile->show();
}

void Posts::onLikeClicked()
{
	QString filePath = QCoreApplication::applicationDirPath() + "/../../resources/posts.json";
	QFile file(filePath);

	if (!file.open(QIODevice::ReadOnly)) return;
	QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
	file.close();

	QJsonArray postsArray = doc.array();
	for (int i = 0; i < postsArray.size(); ++i) {
		QJsonObject obj = postsArray[i].toObject();

		if (obj["username"].toString().trimmed() == currentData.username &&
			obj["content"].toString().trimmed() == currentData.content &&
			obj["timestamp"].toString().trimmed() == currentData.timestamp) {

			QJsonArray likedByArray = obj["likedBy"].toArray();
			QStringList likedList;
			for (const QJsonValue& v : likedByArray) likedList << v.toString();

			if (!isLiked) {
				if (!likedList.contains(currentUser)) {
					likedList.append(currentUser);
					isLiked = true;
				}
			}
			else {
				likedList.removeAll(currentUser);
				isLiked = false;
			}

			QJsonArray newArray;
			for (const QString& name : likedList) newArray.append(name);
			obj["likedBy"] = newArray;

			postsArray[i] = obj;

			ui.lblLikeCount->setText(QString::number(likedList.size()));
			ui.btnLike->setText(isLiked ? "Unlike" : "Like");
			ui.btnLike->setStyleSheet(isLiked ? "color: #ff4757; font-weight: bold;" : "");

			currentData.likedBy = likedList;
			break;
		}
	}

	if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		file.write(QJsonDocument(postsArray).toJson());
		file.close();
	}
}