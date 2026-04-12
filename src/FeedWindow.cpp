#include "FeedWindow.h"
#include "ProfileWindow.h"
#include "PostWidget.h"
#include "models/Post.h" 
#include "MessagesWindow.h"
#include "LoginWindow.h"
#include "UserManager.h"
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
#include <QEvent>

FeedWindow::FeedWindow(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    qDebug() << "[INFO] UI: Initializing FeedWindow.";

    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(ui.scrollPosts->layout());
    if (!layout) {
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
    loadPosts();
    updateMessageButtonVisuals();
}

void FeedWindow::onMyProfileClicked() {
    QVector<User> allUsers = UserManager::loadUsers();
    for (const User& u : allUsers) {
        if (u.username == currentUser.username) {
            currentUser = u;
            break;
        }
    }

    ProfileWindow* profileWindow = new ProfileWindow();
    profileWindow->setAttribute(Qt::WA_DeleteOnClose);
    profileWindow->setWindowModality(Qt::ApplicationModal);

    connect(profileWindow, &ProfileWindow::windowClosed, this, &FeedWindow::loadPosts);
    connect(profileWindow, &ProfileWindow::windowClosed, this, &FeedWindow::updateMessageButtonVisuals);

    profileWindow->setActiveUser(currentUser, currentUser);
    profileWindow->show();
}

void FeedWindow::onSubmitPostClicked() {
    QString contentText = ui.newTextPost->toPlainText().trimmed();
    if (contentText.isEmpty()) return;

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
    }

    loadPosts();
    ui.newTextPost->clear();
}

void FeedWindow::loadPosts() {
    QString filePath = QCoreApplication::applicationDirPath() + "/../../resources/posts.json";
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return;

    QJsonArray postsArray = QJsonDocument::fromJson(file.readAll()).array();
    file.close();

    QList<Post> postList;
    for (const QJsonValue& value : postsArray) {
        postList.append(Post::fromJson(value.toObject()));
    }

    QLayoutItem* item;
    while (ui.scrollPosts->layout() && (item = ui.scrollPosts->layout()->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    FeedStrategy* strategy = nullptr;
    int sortType = ui.selectAlgo->currentIndex();
    if (sortType == 1) strategy = new MostLikedStrategy();
    else if (sortType == 2) strategy = new FollowingStrategy();
    else strategy = new NewestStrategy();

    if (strategy) {
        strategy->sort(postList, currentUser);
        delete strategy;
    }

    for (const Post& postData : postList) {
        PostWidget* postWidget = new PostWidget(this);
        postWidget->setPostData(postData, currentUser.username);
        ui.scrollPosts->layout()->addWidget(postWidget);
    }
}

void FeedWindow::onMessagesClicked() {
    if (messagesWindow == nullptr) {
        messagesWindow = new MessagesWindow(this);
        messagesWindow->setWindowModality(Qt::ApplicationModal);
        connect(messagesWindow, &QObject::destroyed, this, [this]() { messagesWindow = nullptr; });
    }
    messagesWindow->setActiveUser(currentUser);
    messagesWindow->show();
}

// --- MISSING FUNCTIONS START HERE ---

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
    loadPosts();
    updateMessageButtonVisuals();
}

void FeedWindow::onSortSelect(int index) {
    qDebug() << "[DEBUG] UI: Algorithm changed. Reloading feed.";
    loadPosts();
}

void FeedWindow::updateMessageButtonVisuals()
{
    QString filePath = QCoreApplication::applicationDirPath() + "/../../resources/messages.json";
    QFile file(filePath);
    int unreadCount = 0;

    if (file.open(QIODevice::ReadOnly)) {
        QJsonArray messagesArray = QJsonDocument::fromJson(file.readAll()).array();
        file.close();
        for (const QJsonValue& value : messagesArray) {
            QJsonObject msgObj = value.toObject();
            if (msgObj["recipient"].toString() == currentUser.getUsername() && !msgObj["isRead"].toBool()) {
                unreadCount++;
            }
        }
    }

    if (unreadCount > 0) {
        ui.btnMessages->setText(QString("Messages (%1)").arg(unreadCount));
        ui.btnMessages->setStyleSheet("background-color: #2ecc71; color: white; font-weight: bold; border-radius: 5px;");
    }
    else {
        ui.btnMessages->setText("Messages");
        ui.btnMessages->setStyleSheet("");
    }
}

bool FeedWindow::event(QEvent* e)
{
    if (e->type() == QEvent::WindowActivate) {
        loadPosts();
        updateMessageButtonVisuals();
    }
    return QMainWindow::event(e);
}