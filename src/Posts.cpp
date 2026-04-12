#include "Posts.h"
#include "models/Post.h"
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
	qDebug() << "[INFO] UI: Initializing individual Post widget.";

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

void Posts::setPostData(const Post& data, const QString& currentLoggedInUser)
{
	currentData = data;
	currentUser = currentLoggedInUser;

	qDebug() << "[DEBUG] UI: Applying Post data to widget (Sender:" << data.senderUsername << ")";

	ui.btnUsername->setText(data.senderUsername);
	ui.lblContent->setText(data.textContent);
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
		qDebug() << "[DEBUG] UI: No avatar found in resources, using fallback character.";
		ui.lblProfilePic->setText("?");
	}
	else {
		ui.lblProfilePic->setStyleSheet("background: transparent; border: none;");
		QPixmap scaledAvatar = avatar.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		ui.lblProfilePic->setPixmap(scaledAvatar);
		ui.lblProfilePic->setFixedSize(50, 50);
		ui.lblProfilePic->setAlignment(Qt::AlignCenter);
	}

	if (data.senderUsername.trimmed().toLower() == currentLoggedInUser.trimmed().toLower()) {
		ui.btnDelete->setVisible(true);
	}
	else {
		ui.btnDelete->setVisible(false);
	}
}

void Posts::onDeleteClicked()
{
	qDebug() << "[INFO] Post: Delete button clicked for post by" << currentData.senderUsername;

	auto reply = QMessageBox::question(this, "Delete Post", "Delete this post?");
	if (reply != QMessageBox::Yes) {
		qDebug() << "[DEBUG] Post: Deletion cancelled by user.";
		return;
	}

	QString filePath = QCoreApplication::applicationDirPath() + "/../../resources/posts.json";
	QFile file(filePath);

	if (!file.open(QIODevice::ReadOnly)) {
		qDebug() << "[ERROR] PERSISTENCE: Failed to open posts.json for deletion!";
		return;
	}

	QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
	file.close();

	QJsonArray postsArray = doc.array();
	QJsonArray updatedArray;
	bool matchFound = false;

	qDebug() << "[DEBUG] PERSISTENCE: Searching for post match in JSON records...";

	for (const QJsonValue& value : postsArray) {
		QJsonObject obj = value.toObject();
		// Matching based on sender, text, and timestamp to ensure the correct post is hit
		if (obj["username"].toString().trimmed() == currentData.senderUsername &&
			obj["content"].toString().trimmed() == currentData.textContent &&
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
			qDebug() << "[SUCCESS] PERSISTENCE: Post removed from JSON and file truncated.";
		}
	}
	else {
		qDebug() << "[ERROR] PERSISTENCE: Could not find a matching post in the JSON to delete.";
	}

	this->deleteLater();
}

void Posts::onUsernameClicked()
{
	qDebug() << "[INFO] UI: Username button clicked. Opening profile for" << currentData.senderUsername;
	Profile* profile = new Profile();

	QVector<User> allUsers = UserManager::loadUsers();
	qDebug() << "[DEBUG] Auth: Loaded" << allUsers.size() << "users to find author details.";

	User author;
	User viewer;
	bool authorFound = false;

	for (const User& u : allUsers) {
		if (u.username == currentData.senderUsername) {
			author = u;
			authorFound = true;
		}
		if (u.username == currentUser) {
			viewer = u;
		}
	}

	if (!authorFound) {
		qDebug() << "[DEBUG] Auth: Warning - Author not found in database, using fallback username.";
		author.username = currentData.senderUsername;
	}

	profile->setActiveUser(author, viewer);
	profile->show();
}

void Posts::onLikeClicked()
{
	qDebug() << "[INFO] Post:" << (isLiked ? "Unlike" : "Like") << "requested by" << currentUser;

	QString filePath = QCoreApplication::applicationDirPath() + "/../../resources/posts.json";
	QFile file(filePath);

	if (!file.open(QIODevice::ReadOnly)) {
		qDebug() << "[ERROR] PERSISTENCE: Cannot open posts.json to update like status.";
		return;
	}

	QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
	file.close();

	QJsonArray postsArray = doc.array();
	bool updateSaved = false;

	for (int i = 0; i < postsArray.size(); ++i) {
		QJsonObject obj = postsArray[i].toObject();

		if (obj["username"].toString().trimmed() == currentData.senderUsername &&
			obj["content"].toString().trimmed() == currentData.textContent &&
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
			updateSaved = true;
			break;
		}
	}

	if (updateSaved && file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		file.write(QJsonDocument(postsArray).toJson());
		file.close();
		qDebug() << "[SUCCESS] PERSISTENCE: Like status synchronized with database.";
	}
}