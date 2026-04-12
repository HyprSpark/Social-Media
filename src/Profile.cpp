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
#include <QDebug>

Profile::Profile(QWidget* parent) : QMainWindow(parent) {
    ui.setupUi(this);
    qDebug() << "[INFO] UI: Initializing Profile Window.";

    connect(ui.btnToFeed, &QPushButton::clicked, this, &Profile::onReturnClicked);
    connect(ui.btnFollow, &QPushButton::clicked, this, &Profile::onFollowClicked);
}

Profile::~Profile() {}

void Profile::setActiveUser(const User& user, const User& viewer) {
    viewedUser = user;
    qDebug() << "[INFO] Profile: Loading data for profile owner:" << viewedUser.username;

    // Re-syncing viewer to ensure we have the latest 'following' list
    QVector<User> allUsers = UserManager::loadUsers();
    for (const User& u : allUsers) {
        if (u.username == viewer.username) {
            loggedInUser = u;
            qDebug() << "[DEBUG] Auth: Viewer session synchronized as:" << loggedInUser.username;
            break;
        }
    }

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
        qDebug() << "[DEBUG] UI: Default avatar not found, using placeholder.";
        ui.lblProfilePic->setText("?");
        ui.lblProfilePic->setAlignment(Qt::AlignCenter);
    }

    qDebug() << "[INFO] Profile: Filtering posts for owner's personal feed.";
    loadUserPosts();

    isAlreadyFollowing = loggedInUser.following.contains(viewedUser.username, Qt::CaseInsensitive);

    if (viewedUser.username == loggedInUser.username) {
        qDebug() << "[DEBUG] UI: Hiding follow button (User is viewing own profile).";
        ui.btnFollow->setVisible(false);
    }
    else {
        ui.btnFollow->setVisible(true);
        ui.btnFollow->setText(isAlreadyFollowing ? "Unfollow" : "Follow");
        ui.btnFollow->setStyleSheet(isAlreadyFollowing ? "background-color: #444; color: white;" : "background-color: #0078d4; color: white;");
    }
    ui.lblFollowCount->setText(QString::number(viewedUser.followers.size()) + " Followers");
}

void Profile::onReturnClicked() {
    this->hide();
}

void Profile::onFollowClicked() {
    qDebug() << "[INFO] Social: Toggling follow status between" << loggedInUser.username << "and" << viewedUser.username;

    // 1. Update the Permanent JSON Database
    UserManager::toggleFollowing(loggedInUser.username, viewedUser.username);

    // 2. Flip the UI state
    isAlreadyFollowing = !isAlreadyFollowing;

    // 3. Update the objects in memory
    if (isAlreadyFollowing) {
        if (!loggedInUser.following.contains(viewedUser.username)) {
            loggedInUser.following.append(viewedUser.username);
        }
        if (!viewedUser.followers.contains(loggedInUser.username)) {
            viewedUser.followers.append(loggedInUser.username);
        }
        ui.btnFollow->setText("Unfollow");
        ui.btnFollow->setStyleSheet("background-color: #444; color: white;");
    }
    else {
        loggedInUser.following.removeAll(viewedUser.username);
        viewedUser.followers.removeAll(loggedInUser.username);
        ui.btnFollow->setText("Follow");
        ui.btnFollow->setStyleSheet("background-color: #0078d4; color: white;");
    }

    ui.lblFollowCount->setText(QString::number(viewedUser.followers.size()) + " Followers");
    qDebug() << "[SUCCESS] PERSISTENCE: Follow relationship updated and synced to disk.";
}

void Profile::loadUserPosts() {
    // FIX: Instead of creating a new layout, we look for the one created in the constructor
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(ui.scrollProfilePosts->layout());

    if (layout) {
        qDebug() << "[DEBUG] UI: Clearing existing profile posts from layout.";
        QLayoutItem* item;
        while ((item = layout->takeAt(0)) != nullptr) {
            if (item->widget()) delete item->widget();
            delete item;
        }
    }
    else {
        // Fallback if somehow the constructor layout was lost
        qDebug() << "[DEBUG] UI: Layout missing, initializing fallback QVBoxLayout.";
        layout = new QVBoxLayout(ui.scrollProfilePosts);
        layout->setAlignment(Qt::AlignTop);
    }

    QFile file("resources/posts.json");
    if (file.open(QIODevice::ReadOnly)) {
        QJsonArray postsArray = QJsonDocument::fromJson(file.readAll()).array();
        file.close();

        int postCount = 0;
        for (const QJsonValue& value : postsArray) {
            QJsonObject obj = value.toObject();
            if (obj["username"].toString() == viewedUser.username) {
                // Use the polymorphic model to build data
                Post postData = Post::fromJson(obj);

                Posts* postWidget = new Posts(this);
                postWidget->setPostData(postData, loggedInUser.username);

                layout->addWidget(postWidget);
                postCount++;
            }
        }
        qDebug() << "[SUCCESS] Profile: Rendered" << postCount << "posts for user" << viewedUser.username;
    }
    else {
        qDebug() << "[ERROR] PERSISTENCE: Failed to open posts.json for profile filtering.";
    }
}