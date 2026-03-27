#include "Posts.h"
#include "Content.h"
#include "Profile.h"
#include <QPushButton>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QPixmap>          // Required for loading images
#include <QCoreApplication> // Required for secure file saving
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

Posts::~Posts()
{
}

void Posts::setPostData(const Content& data, const QString& currentLoggedInUser)
{
	currentData = data;
	currentUser = currentLoggedInUser;

	ui.btnUsername->setText(data.username);
	ui.lblContent->setText(data.content);
	ui.lblTimestamp->setText(data.timestamp);
	ui.lblTimestamp->setStyleSheet("color: #666; font-size: 10px;"); // Make it subtle

	// Display the count based on the list size
	ui.lblLikeCount->setText(QString::number(data.likedBy.size()));

	// --- The Visual Sync ---
	if (data.likedBy.contains(currentUser)) {
		isLiked = true;
		ui.btnLike->setText("Unlike");
		// ADD THIS LINE: This ensures it turns red on reboot
		ui.btnLike->setStyleSheet("color: #ff4757; font-weight: bold;");
	}
	else {
		isLiked = false;
		ui.btnLike->setText("Like");
		ui.btnLike->setStyleSheet(""); // Reset to default
	}

	// --- Avatar Logic ---
	// Note: If your image is inside a folder in the .qrc (like /resources/profile.png), 
	// update the string below to match exactly!
	QPixmap avatar(":resources/images/profile.png");

	if (avatar.isNull()) {
		// If it fails, print an error so we know the path is wrong
		qDebug() << "ERROR: Avatar image not found at the specified path!";
		ui.lblProfilePic->setText("?"); // Put a question mark as a temporary fallback
	}
	else {
		// If it succeeds, scale and display it
		ui.lblProfilePic->setStyleSheet("background: transparent; border: none;");

		QPixmap scaledAvatar = avatar.scaled(50, 50, Qt::KeepAspectRatio, Qt::SmoothTransformation);

		ui.lblProfilePic->setPixmap(scaledAvatar);

		// 4. Ensure the label itself is exactly 80x80 so it doesn't "cut" the image
		ui.lblProfilePic->setFixedSize(50, 50);
		ui.lblProfilePic->setAlignment(Qt::AlignCenter);
	}

	if (data.username == currentLoggedInUser) {
		ui.btnDelete->setVisible(true);
	}
	else {
		ui.btnDelete->setVisible(false);
	}
	qDebug() << "Checking Delete Button:";
	qDebug() << "Post Author:" << data.username;
	qDebug() << "Logged In User:" << currentLoggedInUser;

	if (data.username.trimmed().toLower() == currentLoggedInUser.trimmed().toLower()) {
		ui.btnDelete->setVisible(true);
		qDebug() << "Match found! Showing button.";
	}
	else {
		ui.btnDelete->setVisible(false);
		qDebug() << "No match. Hiding button.";
	}
}


void Posts::onDeleteClicked()
{
	auto reply = QMessageBox::question(this, "Delete Post", "Delete this post?");
	if (reply != QMessageBox::Yes) return;

	// Direct path to your source resources folder
	QString filePath = QCoreApplication::applicationDirPath() + "/../../resources/posts.json";
	QFile file(filePath);

	if (!file.open(QIODevice::ReadOnly)) {
		qDebug() << "ERROR: Still can't find the file at:" << filePath;
		return;
	}

	QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
	file.close();

	QJsonArray postsArray = doc.array();
	QJsonArray updatedArray;
	bool matchFound = false;

	for (const QJsonValue& value : postsArray) {
		QJsonObject obj = value.toObject();
		// Matching exactly what is in your JSON
		if (obj["username"].toString() == currentData.username &&
			obj["content"].toString() == currentData.content &&
			obj["timestamp"].toString() == currentData.timestamp) {
			matchFound = true;
			qDebug() << "MATCH FOUND! Removing from file...";
		}
		else {
			updatedArray.append(obj);
		}
	}

	if (matchFound) {
		if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
			file.write(QJsonDocument(updatedArray).toJson());
			file.close();
			qDebug() << "SUCCESS: File updated in resources folder.";
		}
	}

	this->deleteLater();
}

void Posts::onUsernameClicked()
{
	Profile* profile = new Profile(this);
	User clickedUser;
	clickedUser.username = currentData.username;
	profile->setActiveUser(clickedUser);
	profile->show();
}

void Posts::onLikeClicked()
{
	// 1. Direct path to your resources folder (the one that worked!)
	QString filePath = QCoreApplication::applicationDirPath() + "/../../resources/posts.json";
	QFile file(filePath);

	if (!file.open(QIODevice::ReadOnly)) return;
	QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
	file.close();

	QJsonArray postsArray = doc.array();
	for (int i = 0; i < postsArray.size(); ++i) {
		QJsonObject obj = postsArray[i].toObject();

		// Match the specific post
		if (obj["username"].toString() == currentData.username &&
			obj["content"].toString() == currentData.content &&
			obj["timestamp"].toString() == currentData.timestamp) {

			// Get the current list of people who liked it
			QJsonArray likedByArray = obj["likedBy"].toArray();
			QStringList likedList;
			for (const QJsonValue& v : likedByArray) likedList << v.toString();

			if (!isLiked) {
				// ADD LIKE: Only add if we aren't already there
				if (!likedList.contains(currentUser)) {
					likedList.append(currentUser);
					isLiked = true;
				}
			}
			else {
				// REMOVE LIKE: Take our name out
				likedList.removeAll(currentUser);
				isLiked = false;
			}

			// Convert back to JSON array
			QJsonArray newArray;
			for (const QString& name : likedList) newArray.append(name);
			obj["likedBy"] = newArray;

			// Update the main array
			postsArray[i] = obj;

			// Update the UI immediately
			ui.lblLikeCount->setText(QString::number(likedList.size()));
			ui.btnLike->setText(isLiked ? "Unlike" : "Like");
			ui.btnLike->setStyleSheet(isLiked ? "color: #ff4757; font-weight: bold;" : "");

			// Sync our local widget data
			currentData.likedBy = likedList;
			break;
		}
	}

	// 2. Save permanently
	if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
		file.write(QJsonDocument(postsArray).toJson());
		file.close();
		qDebug() << "SUCCESS: Like status synced for user:" << currentUser;
	}
}
