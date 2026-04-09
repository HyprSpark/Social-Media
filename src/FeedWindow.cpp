#include "FeedWindow.h"
#include "Profile.h"
#include "Posts.h"
#include "Content.h" 
#include "Messages.h"
#include "LoginWindow.h"
#include "userManager.h"
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

/**
 * @brief Constructor: Sets up the main dashboard and UI layout.
 */
FeedWindow::FeedWindow(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    // --- Layout Setup --- //
    // Fetches the existing layout from the scroll area or creates one if missing.
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(ui.scrollPosts->layout());
    if (!layout) {
        layout = new QVBoxLayout(ui.scrollPosts);
    }
    layout->setAlignment(Qt::AlignTop);

    // --- Button Connections --- //
    connect(ui.btnMyProfile, &QPushButton::clicked, this, &FeedWindow::onMyProfileClicked);
    connect(ui.btnSubmitPost, &QPushButton::clicked, this, &FeedWindow::onSubmitPostClicked);
    connect(ui.btnQuit, &QPushButton::clicked, this, &FeedWindow::onQuitClicked);
    connect(ui.btnSignOut, &QPushButton::clicked, this, &FeedWindow::onSignOutClicked);
    connect(ui.btnMessages, &QPushButton::clicked, this, &FeedWindow::onMessagesClicked);
    connect(ui.selectAlgo, &QComboBox::currentIndexChanged, this, &FeedWindow::onSortSelect);
}

FeedWindow::~FeedWindow() {}

/**
 * @brief Sets the active user and triggers the initial post load.
 */
void FeedWindow::setActiveUser(const User& user) {
    currentUser = user;
    loadPosts();
}

/**
 * @brief Opens the Profile window.
 * Passes the currentUser twice: once as the owner and once as the viewer.
 */
void FeedWindow::onMyProfileClicked() {
    // Before opening the profile, re-load the users to ensure we have the latest friend list
    QVector<User> allUsers = UserManager::loadUsers();
    for (const User& u : allUsers) {
        if (u.username == currentUser.username) {
            currentUser = u; // Sync the local object with the database
            break;
        }
    }

    Profile* profile = new Profile();
    profile->setActiveUser(currentUser, currentUser);
    profile->setAttribute(Qt::WA_DeleteOnClose);
    profile->show();
}

/**
 * @brief Logic for creating and saving a new post.
 */
void FeedWindow::onSubmitPostClicked() {
    QString contentText = ui.newTextPost->toPlainText().trimmed();
    if (contentText.isEmpty()) return;

    QString currentTime = QDateTime::currentDateTime().toString("MMM dd, HH:mm");

    // Create the data object
    Content postData(currentUser.username, contentText, QStringList(), currentTime);

    // Save to JSON
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

    // Refresh UI by adding the new post to the top
    Posts* postWidget = new Posts(this);
    postWidget->setPostData(postData, currentUser.username);

    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(ui.scrollPosts->layout());
    if (layout) layout->insertWidget(0, postWidget);

    ui.newTextPost->clear();
}

/**
 * @brief Reads the post database and populates the scroll area.
 */
void FeedWindow::loadPosts() {
    QString filePath = QCoreApplication::applicationDirPath() + "/../../resources/posts.json";
    QFile file(filePath);

    // Fallback for Debug mode
    if (!file.exists()) {
        file.setFileName("resources/posts.json");
    }

    if (!file.open(QIODevice::ReadOnly)) {
        return;
    }

    QVector<User> allUsers = UserManager::loadUsers();
    for (const User& u : allUsers) {
        if (u.username == currentUser.username) {
            currentUser = u;
            break;
        }
    }

    QJsonArray postsArray = QJsonDocument::fromJson(file.readAll()).array();
    file.close();

    // Move JSON data into a temporary list for sorting 
    QList<Content> postList;
    for (const QJsonValue& value : postsArray) {
        postList.append(Content::fromJson(value.toObject()));
    }

    // THE SORTING ALGORITHM 
    int sortType = ui.selectAlgo->currentIndex();

    if (sortType == 1) { // Most Liked sort
        std::sort(postList.begin(), postList.end(), [](const Content& a, const Content& b) {
            return a.likedBy.size() > b.likedBy.size(); // Most likes first
            });
    }
    else if (sortType == 2) { // Following sort
        std::sort(postList.begin(), postList.end(), [this](const Content& a, const Content& b) {
            // Check if 'a' and 'b' are in the current user's following list
            bool aIsFollowed = currentUser.following.contains(a.username);
            bool bIsFollowed = currentUser.following.contains(b.username);

            if (aIsFollowed != bIsFollowed) {
                return aIsFollowed; // If only one is followed, put them first
            }
            return false; // Otherwise, maintain original order
            });
    }
    // Note: If sortType is 0 (Newest), we don't need to sort because 
    // the JSON is already prepended (newest first).

    // Clear existing widgets 
    QLayoutItem* item;
    while (ui.scrollPosts->layout() && (item = ui.scrollPosts->layout()->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }

    // Build the UI using the sorted list 
    for (const Content& postData : postList) {
        Posts* postWidget = new Posts(this);
        postWidget->setPostData(postData, currentUser.username);
        ui.scrollPosts->layout()->addWidget(postWidget);
    }
}

void FeedWindow::onMessagesClicked() {
    if (messagesWindow == nullptr) {
        messagesWindow = new Messages(this);
        messagesWindow->setActiveUser(currentUser);
    }
    messagesWindow->show();
    messagesWindow->raise();
    messagesWindow->activateWindow();
}

void FeedWindow::onQuitClicked() {
    QApplication::quit();
}

void FeedWindow::onSignOutClicked() {
    LoginWindow* login = new LoginWindow();
    login->setAttribute(Qt::WA_DeleteOnClose);
    login->show();
    this->close();
}

void FeedWindow::showEvent(QShowEvent* event) {
    QMainWindow::showEvent(event); // Call the base class logic first

    qDebug() << "LOG: FeedWindow reappeared. Refreshing posts for sync...";
    loadPosts(); // This pulls the fresh 'likedBy' lists from the JSON
}

void FeedWindow::onSortSelect(int index) {
    loadPosts();
}