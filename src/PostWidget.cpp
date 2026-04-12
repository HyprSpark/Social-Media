#include "PostWidget.h"
#include "models/Post.h"
#include "ProfileWindow.h"
#include "UserManager.h"
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

// Changed Posts::Posts to PostWidget::PostWidget
PostWidget::PostWidget(QWidget* parent)
	: QWidget(parent), isLiked(false)
{
	ui.setupUi(this);
	qDebug() << "[INFO] UI: Initializing individual Post widget.";

	this->setStyleSheet(
		"PostWidget { " // Match the CSS to the new class name
		"   border: 1px solid #444; "
		"   border-radius: 8px; "
		"   background-color: #222; "
		"   margin-bottom: 10px; "
		"   padding: 5px; "
		"}"
	);

	connect(ui.btnLike, &QPushButton::clicked, this, &PostWidget::onLikeClicked);
	connect(ui.btnUsername, &QPushButton::clicked, this, &PostWidget::onUsernameClicked);
	connect(ui.btnDelete, &QPushButton::clicked, this, &PostWidget::onDeleteClicked);
}

PostWidget::~PostWidget() {}

void PostWidget::setPostData(const Post& data, const QString& currentLoggedInUser)
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

void PostWidget::onDeleteClicked()
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

	for (const QJsonValue& value : postsArray) {
		QJsonObject obj = value.toObject();
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
			qDebug() << "[SUCCESS] PERSISTENCE: Post removed from JSON.";
		}
	}

	this->deleteLater();
}

void PostWidget::onUsernameClicked()
{
	qDebug() << "[INFO] UI: Username button clicked. Opening profile for" << currentData.senderUsername;

	QVector<User> allUsers = UserManager::loadUsers();
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

	if (!authorFound) author.username = currentData.senderUsername;

	QWidget* topLevel = this->window();
	ProfileWindow* existingProfile = qobject_cast<ProfileWindow*>(topLevel);

	if (existingProfile) {
		existingProfile->setActiveUser(author, viewer);
	}
	else {
		ProfileWindow* profileWindow = new ProfileWindow();
		profileWindow->setAttribute(Qt::WA_DeleteOnClose);
		profileWindow->setActiveUser(author, viewer);
		profileWindow->show();
	}
}

void PostWidget::onLikeClicked()
{
	qDebug() << "[INFO] Post:" << (isLiked ? "Unlike" : "Like") << "requested by" << currentUser;

	QString filePath = QCoreApplication::applicationDirPath() + "/../../resources/posts.json";
	QFile file(filePath);

	if (!file.open(QIODevice::ReadOnly)) return;

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
		qDebug() << "[SUCCESS] PERSISTENCE: Like status synchronized.";
	}
}