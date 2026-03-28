#include "Profile.h"
#include "FeedWindow.h"
#include "Posts.h"
#include "UserManager.h"
#include <QMessageBox>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QVBoxLayout>

Profile::Profile(QWidget* parent) : QMainWindow(parent) {
    ui.setupUi(this);
    QVBoxLayout* layout = new QVBoxLayout(ui.scrollProfilePosts);
    layout->setAlignment(Qt::AlignTop);

    connect(ui.btnToFeed, &QPushButton::clicked, this, &Profile::onReturnClicked);
    connect(ui.btnFriend, &QPushButton::clicked, this, &Profile::onFriendClicked);
}

Profile::~Profile() {}

void Profile::setActiveUser(const User& user, const User& viewer) {
    viewedUser = user;     // The profile owner
    loggedInUser = viewer; // The person viewing

    ui.lblUsername->setText(viewedUser.username);

    // Profile Image Logic
    QPixmap avatar(":resources/images/profile.png");
    if (!avatar.isNull()) {
        ui.lblProfilePic->setPixmap(avatar.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ui.lblProfilePic->setFixedSize(80, 80);
        ui.lblProfilePic->setAlignment(Qt::AlignCenter);
        ui.lblProfilePic->setStyleSheet("background: transparent; border: 1px solid #555;");
    }
    else {
        ui.lblProfilePic->setText("?");
        ui.lblProfilePic->setAlignment(Qt::AlignCenter);
    }

    // Load the owner's posts
    loadUserPosts();

    // Check relationship status
    isAlreadyFriends = loggedInUser.friends.contains(viewedUser.username);

    if (viewedUser.username == loggedInUser.username) {
        ui.btnFriend->setVisible(false);
    }
    else {
        ui.btnFriend->setVisible(true);
        ui.btnFriend->setText(isAlreadyFriends ? "Unfriend" : "Add Friend");
        ui.btnFriend->setStyleSheet(isAlreadyFriends ? "background-color: #444; color: white;" : "background-color: #0078d4; color: white;");
    }
    ui.lblFriendCount->setText(QString::number(viewedUser.friends.size()) + " Friends");
}

void Profile::onReturnClicked() {
    this->hide();
}

void Profile::onFriendClicked() {
    // 1. Update the Permanent JSON Database
    UserManager::toggleFriend(loggedInUser.username, viewedUser.username);

    // 2. Flip the UI state
    isAlreadyFriends = !isAlreadyFriends;

    // 3. IMPORTANT: Update the C++ object in memory
    if (isAlreadyFriends) {
        if (!loggedInUser.friends.contains(viewedUser.username)) {
            loggedInUser.friends.append(viewedUser.username);
        }
        ui.btnFriend->setText("Unfriend");
        ui.btnFriend->setStyleSheet("background-color: #444; color: white;");
    }
    else {
        loggedInUser.friends.removeAll(viewedUser.username);
        ui.btnFriend->setText("Add Friend");
        ui.btnFriend->setStyleSheet("background-color: #0078d4; color: white;");
    }

    // 4. Update the Friend Count label visually (Optional but looks better)
    int currentCount = viewedUser.friends.size();
    ui.lblFriendCount->setText(QString::number(isAlreadyFriends ? currentCount + 1 : currentCount) + " Friends");
}

void Profile::loadUserPosts() {
    QLayout* layout = ui.scrollProfilePosts->layout();

    if (layout) {
        QLayoutItem* item;
        while ((item = layout->takeAt(0)) != nullptr) {
            if (item->widget()) delete item->widget();
            delete item;
        }
    }
    else {
        layout = new QVBoxLayout(ui.scrollProfilePosts);
        layout->setAlignment(Qt::AlignTop);
    }

    QFile file("resources/posts.json");
    if (file.open(QIODevice::ReadOnly)) {
        QJsonArray postsArray = QJsonDocument::fromJson(file.readAll()).array();
        file.close();

        for (const QJsonValue& value : postsArray) {
            QJsonObject obj = value.toObject();
            if (obj["username"].toString() == viewedUser.username) {
                Content postData;
                postData.username = obj["username"].toString();
                postData.content = obj["content"].toString();
                postData.timestamp = obj["timestamp"].toString(); // Don't forget timestamp!

                // Parsing Likes
                QJsonArray arr = obj["likedBy"].toArray();
                for (auto v : arr) postData.likedBy << v.toString();

                Posts* postWidget = new Posts(this);
                postWidget->setPostData(postData, loggedInUser.username);

                layout->addWidget(postWidget);
            }
        }
    }
}