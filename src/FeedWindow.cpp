#include "FeedWindow.h"
#include "ProfileWindow.h"
#include "PostWidget.h"
#include "MessagesWindow.h"
#include "LoginWindow.h"
#include "UserManager.h"
#include "strategies/FeedStrategy.h"
#include "strategies/MostLikedStrategy.h"
#include "strategies/FollowingStrategy.h"
#include "strategies/NewestStrategy.h"
#include <QVBoxLayout>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDateTime>
#include <QEvent>
#include <QMetaObject>
#include <QDebug>
#include <QApplication>

FeedWindow::FeedWindow(QWidget* parent) : QMainWindow(parent) {
    ui.setupUi(this);
    if (!ui.scrollPosts->layout()) {
        new QVBoxLayout(ui.scrollPosts);
    }
    ui.scrollPosts->layout()->setAlignment(Qt::AlignTop);

    connect(ui.btnMyProfile, &QPushButton::clicked, this, &FeedWindow::onMyProfileClicked); // Connects buttons on the UI to the corresponding functions
    connect(ui.btnSubmitPost, &QPushButton::clicked, this, &FeedWindow::onSubmitPostClicked);
    connect(ui.btnQuit, &QPushButton::clicked, this, &FeedWindow::onQuitClicked);
    connect(ui.btnSignOut, &QPushButton::clicked, this, &FeedWindow::onSignOutClicked);
    connect(ui.btnMessages, &QPushButton::clicked, this, &FeedWindow::onMessagesClicked);
    connect(ui.selectAlgo, &QComboBox::currentIndexChanged, this, &FeedWindow::onSortSelect);
}

// --- FIX 1: The Missing Destructor ---
FeedWindow::~FeedWindow() {
	delete currentStrategy; // Clean up the strategy to prevent memory leaks
}

void FeedWindow::loadPosts() {

    QString filePath = QCoreApplication::applicationDirPath() + "/../../resources/posts.json";
	QFile file(filePath); // Construct the file path to the posts.json file
    if (!file.open(QIODevice::ReadOnly)) return;

	QJsonArray postsArray = QJsonDocument::fromJson(file.readAll()).array(); // Read the contents of the file and parse it as a JSON array. Each element in this array represents a post.
    file.close();

    QList<Post> posts;
    for (const QJsonValue& val : postsArray)
		posts.append(Post::fromJson(val.toObject())); // Convert each JSON object in the array into a Post object and store it in a list. Adds the most recent post to the first in the list

    // Apply strategy if one is set
    if (currentStrategy)
		currentStrategy->sort(posts, currentUser); // Sort the posts based on the current strategy (e.g., by most recent, most liked, or following)

    // Clear layout
    if (ui.scrollPosts->layout()) {
        QLayoutItem* item;
		while ((item = ui.scrollPosts->layout()->takeAt(0)) != nullptr) { // Remove each widget from the layout and delete it to free memory    
            if (QWidget* widget = item->widget()) {
                widget->setParent(nullptr); 
                widget->deleteLater();
            }
            delete item;
        }
    }

    for (const Post& post : posts) {
        PostWidget* pw = new PostWidget(this);
		pw->setPostData(post, currentUser.username); // Create a new PostWidget for each post and set its data based on the Post object and the current user's username
		ui.scrollPosts->layout()->addWidget(pw); // Add the PostWidget to the scroll area layout
    }
}

// --- FIX 2: The Missing showEvent ---
void FeedWindow::showEvent(QShowEvent* event) {
    QMainWindow::showEvent(event);
    loadPosts();
	updateMessageButtonVisuals(); // Ensure the message button visuals are updated whenever the window is shown, in case there are new messages
}

bool FeedWindow::event(QEvent* e) {
    if (e->type() == QEvent::WindowActivate) {
        QMetaObject::invokeMethod(this, "loadPosts", Qt::QueuedConnection);
        QMetaObject::invokeMethod(this, "updateMessageButtonVisuals", Qt::QueuedConnection); 
    }
    return QMainWindow::event(e);
}

void FeedWindow::updateMessageButtonVisuals() {
    QString filePath = QCoreApplication::applicationDirPath() + "/../../resources/messages.json";
    QFile file(filePath);
    int unread = 0;
    if (file.open(QIODevice::ReadOnly)) {
        QJsonArray arr = QJsonDocument::fromJson(file.readAll()).array();
        for (const QJsonValue& v : arr) {
            QJsonObject obj = v.toObject();
			if (obj["recipient"].toString() == currentUser.username && !obj["isRead"].toBool()) { // Check if the message is for the current user and is unread
                unread++;
            }
        }
        file.close();
    }

    if (unread > 0) {
        ui.btnMessages->setText(QString("Messages (%1)").arg(unread)); 
        ui.btnMessages->setStyleSheet("background-color: #2ecc71; color: white; font-weight: bold; border-radius: 5px;");
    }
    else {
        ui.btnMessages->setText("Messages");
        ui.btnMessages->setStyleSheet("");
    }
}

void FeedWindow::onMyProfileClicked() {
    ProfileWindow* profile = new ProfileWindow();
    profile->setAttribute(Qt::WA_DeleteOnClose);
    profile->setWindowModality(Qt::ApplicationModal);
    connect(profile, &ProfileWindow::windowClosed, this, &FeedWindow::loadPosts); 
	profile->setActiveUser(currentUser, currentUser); // Open the profile window for the active user, passing their own data as both the user being viewed and the viewer
    profile->show();
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

void FeedWindow::onSubmitPostClicked() {
	QString content = ui.newTextPost->toPlainText().trimmed(); // Get the text from the "Create Post" box and trim whitespace. This ensures that posts with only spaces are not submitted.
    if (content.isEmpty()) return;

	Post newPost(currentUser.username, content, QDateTime::currentDateTime().toString("MMM dd, HH:mm")); // Create a new Post object with the current user's username, the content from the text box, and the current date/time formatted as "Month Day, Hour:Minute"
    QString filePath = QCoreApplication::applicationDirPath() + "/../../resources/posts.json";
    QFile file(filePath);
    QJsonArray arr;
    if (file.open(QIODevice::ReadOnly)) {
        arr = QJsonDocument::fromJson(file.readAll()).array();
        file.close();
    }
    arr.prepend(newPost.toJson());
    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) { 
        file.write(QJsonDocument(arr).toJson());
        file.close();
    }
    loadPosts();
	ui.newTextPost->clear(); // Clear the "Create Post" text box after submitting a post
}

void FeedWindow::setActiveUser(const User& user) {
	currentUser = user; // Store the active user's data in the FeedWindow for later use (e.g., when creating posts or checking messages)
    loadPosts(); 
    updateMessageButtonVisuals();
}

void FeedWindow::onSignOutClicked() {
    LoginWindow* lw = new LoginWindow();
	lw->show(); // Show the login window again
    this->close(); 
}

void FeedWindow::onQuitClicked() {
	QApplication::quit(); // Safely shuts down the entire application
}

void FeedWindow::onSortSelect(int index) {
    delete currentStrategy;

    if (index == 0) {
		currentStrategy = new NewestStrategy(); // Default strategy shows the most recent posts first
    }
    else if (index == 1) {
		currentStrategy = new MostLikedStrategy(); // Sorts posts based on the number of likes, with the most liked posts appearing first
    }
    else if (index == 2) {
		currentStrategy = new FollowingStrategy(); // Sorts posts based on the users that the current user is following, with posts from followed users appearing
    }

    loadPosts();
}