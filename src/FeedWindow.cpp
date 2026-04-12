#include "FeedWindow.h"
#include "Profile.h"
#include "Posts.h"
#include "models/Post.h" 
#include "Messages.h"
#include "LoginWindow.h"
#include "userManager.h"
#include "strategies/FeedStrategy.h"
#include "strategies/MostLikedStrategy.h"
#include "strategies/FollowingStrategy.h"
#include "strategies/NewestStrategy.h"
#include <QApplication> 
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDateTime>
#include <QShowEvent>
#include <algorithm>

FeedWindow::FeedWindow(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    qDebug() << "[INFO] UI: Initializing FeedWindow and setting up layouts.";

    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(ui.scrollPosts->layout());
    if (!layout) {
        qDebug() << "[DEBUG] UI: No layout found for scrollPosts, creating new QVBoxLayout.";
        layout = new QVBoxLayout(ui.scrollPosts);
    }
    layout->setAlignment(Qt::AlignTop);

    connect(ui.btnMyProfile, &QPushButton::clicked, this, &FeedWindow::onMyProfileClicked);
    connect(ui.btnSubmitPost, &QPushButton::clicked, this, &FeedWindow::onSubmitPostClicked);
    connect(ui.btnQuit, &QPushButton::clicked, this, &FeedWindow::onQuitClicked);
    connect(ui.btnSignOut, &QPushButton::clicked, this, &FeedWindow::onSignOutClicked);
    connect(ui.btnMessages, &QPushButton::clicked, this, &FeedWindow::onMessagesClicked);
    connect(ui.selectAlgo, &QComboBox::currentIndexChanged, this, &FeedWindow::onSortSelect);
}

FeedWindow::~FeedWindow() {}

void FeedWindow::setActiveUser(const User& user) {
    currentUser = user;
    qDebug() << "[INFO] Auth: FeedWindow session started for user:" << currentUser.username;
    loadPosts();
}

void FeedWindow::onMyProfileClicked() {
    qDebug() << "[DEBUG] Auth: Re-syncing currentUser with database before opening Profile.";
    QVector<User> allUsers = UserManager::loadUsers();
    for (const User& u : allUsers) {
        if (u.username == currentUser.username) {
            currentUser = u;
            break;
        }
    }

    Profile* profile = new Profile();
    profile->setActiveUser(currentUser, currentUser);
    profile->setAttribute(Qt::WA_DeleteOnClose);
    profile->show();
}

void FeedWindow::onSubmitPostClicked() {
    QString contentText = ui.newTextPost->toPlainText().trimmed();
    if (contentText.isEmpty()) return;

    qDebug() << "[INFO] Post: User" << currentUser.username << "submitting a new post.";

    QString currentTime = QDateTime::currentDateTime().toString("MMM dd, HH:mm");
    Post postData(currentUser.username, contentText, currentTime);

    QString filePath = QCoreApplication::applicationDirPath() + "/../../resources/posts.json";
    QFile file(filePath);
    QJsonArray postsArray;

    if (file.open(QIODevice::ReadOnly)) {
        postsArray = QJsonDocument::fromJson(file.readAll()).array();
        file.close();
    }

    postsArray.prepend(postData.toJson());

    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.write(QJsonDocument(postsArray).toJson());
        file.close();
        qDebug() << "[PERSISTENCE] Success: Post saved to JSON database at:" << filePath;
    }
    else {
        qDebug() << "[ERROR] PERSISTENCE: Failed to write to posts.json!";
    }

    Posts* postWidget = new Posts(this);
    postWidget->setPostData(postData, currentUser.username);

    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(ui.scrollPosts->layout());
    if (layout) layout->insertWidget(0, postWidget);

    ui.newTextPost->clear();
}

void FeedWindow::loadPosts() {
    qDebug() << "[INFO] Feed: Refreshing global post feed.";

    QString filePath = QCoreApplication::applicationDirPath() + "/../../resources/posts.json";
    QFile file(filePath);

    if (!file.exists()) {
        file.setFileName("resources/posts.json");
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "[ERROR] PERSISTENCE: Could not open posts.json for reading.";
        return;
    }

    qDebug() << "[PERSISTENCE] Reading posts from:" << file.fileName();

    // Re-sync user to catch following/follower updates
    QVector<User> allUsers = UserManager::loadUsers();
    for (const User& u : allUsers) {
        if (u.username == currentUser.username) {
            currentUser = u;
            break;
        }
    }

    QJsonArray postsArray = QJsonDocument::fromJson(file.readAll()).array();
    file.close();

    QList<Post> postList;
    for (const QJsonValue& value : postsArray) {
        postList.append(Post::fromJson(value.toObject()));
    }

    qDebug() << "[DEBUG] UI: Clearing" << ui.scrollPosts->layout()->count() << "existing widgets.";
    QLayoutItem* item;
    while (ui.scrollPosts->layout() && (item = ui.scrollPosts->layout()->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    FeedStrategy* strategy = nullptr;
    int sortType = ui.selectAlgo->currentIndex();
    QString strategyName;

    if (sortType == 1) {
        strategy = new MostLikedStrategy();
        strategyName = "MostLikedStrategy";
    }
    else if (sortType == 2) {
        strategy = new FollowingStrategy();
        strategyName = "FollowingStrategy";
    }
    else {
        strategy = new NewestStrategy();
        strategyName = "NewestStrategy";
    }

    if (strategy) {
        qDebug() << "[STRATEGY] Executing" << strategyName << "on" << postList.size() << "posts.";
        strategy->sort(postList, currentUser);
        delete strategy;
    }

    for (const Post& postData : postList) {
        Posts* postWidget = new Posts(this);
        postWidget->setPostData(postData, currentUser.username);
        ui.scrollPosts->layout()->addWidget(postWidget);
    }
    qDebug() << "[SUCCESS] Feed: Finished rendering all post widgets.";
}

void FeedWindow::onMessagesClicked() {
    qDebug() << "[INFO] UI: Opening Messages window.";
    if (messagesWindow == nullptr) {
        messagesWindow = new Messages(this);
        messagesWindow->setActiveUser(currentUser);
    }
    messagesWindow->show();
    messagesWindow->raise();
    messagesWindow->activateWindow();
}

void FeedWindow::onQuitClicked() {
    qDebug() << "[INFO] App: Shutting down.";
    QApplication::quit();
}

void FeedWindow::onSignOutClicked() {
    qDebug() << "[INFO] Auth: User signing out. Returning to LoginWindow.";
    LoginWindow* login = new LoginWindow();
    login->setAttribute(Qt::WA_DeleteOnClose);
    login->show();
    this->close();
}

void FeedWindow::showEvent(QShowEvent* event) {
    QMainWindow::showEvent(event);
    qDebug() << "[INFO] UI: FeedWindow visible. Refreshing posts for data sync.";
    loadPosts();
}

void FeedWindow::onSortSelect(int index) {
    qDebug() << "[DEBUG] UI: Sort algorithm changed (Index:" << index << "). Triggering reload.";
    loadPosts();
}