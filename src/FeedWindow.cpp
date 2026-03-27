#include "FeedWindow.h"
#include "Profile.h"
#include "Posts.h"
#include "Content.h" 
#include "Messages.h"
#include "LoginWindow.h"

#include <QApplication> 
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

FeedWindow::FeedWindow(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(ui.scrollPosts->layout());

    if (!layout) {
        layout = new QVBoxLayout(ui.scrollPosts);
    }
    layout->setAlignment(Qt::AlignTop);

    connect(ui.btnMyProfile, &QPushButton::clicked, this, &FeedWindow::onMyProfileClicked);
    connect(ui.btnSubmitPost, &QPushButton::clicked, this, &FeedWindow::onSubmitPostClicked);
    connect(ui.btnQuit, &QPushButton::clicked, this, &FeedWindow::onQuitClicked);
    connect(ui.btnSignOut, &QPushButton::clicked, this, &FeedWindow::onSignOutClicked);

    // Wires the UI button to the new secure messaging protocol
    connect(ui.btnMessages, &QPushButton::clicked, this, &FeedWindow::onMessagesClicked);

    
}

FeedWindow::~FeedWindow() {}

void FeedWindow::setActiveUser(const User& user) {
    currentUser = user;

    loadPosts();
}

void FeedWindow::onMyProfileClicked() {
    Profile* profile = new Profile();
    profile->setActiveUser(currentUser);
    profile->setAttribute(Qt::WA_DeleteOnClose); // the window is deleted when closed
    profile->show();
}

void FeedWindow::onMessagesClicked() {
    if (messagesWindow == nullptr) {
        messagesWindow = new Messages(this);
        messagesWindow->setActiveUser(currentUser);
    }

    messagesWindow->show();
    messagesWindow->raise();
    messagesWindow->activateWindow();

    // Pass the active user's identity matrix so the comms channel knows who is sending the message

    

}

void FeedWindow::onSubmitPostClicked() {
    QString contentText = ui.newTextPost->toPlainText().trimmed();
    if (contentText.isEmpty()) return;

    // 1. Generate the timestamp
    QString currentTime = QDateTime::currentDateTime().toString("MMM dd, HH:mm");

    // 2. Create the Content object (Make sure there is NO "Content data;" line above this!)
    // This one line replaces all the individual assignments
    Content postData(currentUser.username, contentText, QStringList(), currentTime);

    // 3. Save to JSON (Using the new timestamp)
    QString filePath = QCoreApplication::applicationDirPath() + "/../../resources/posts.json";
    QFile file(filePath);
    QJsonArray postsArray;

    if (file.open(QIODevice::ReadOnly)) {
        postsArray = QJsonDocument::fromJson(file.readAll()).array();
        file.close();
    }

    // Use our new postData object to fill the JSON
    postsArray.prepend(postData.toJson());

    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.write(QJsonDocument(postsArray).toJson());
        file.close();
    }

    // 4. Update the UI
    Posts* postWidget = new Posts(this);
    postWidget->setPostData(postData, currentUser.username);

    // Insert at the top of the layout
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(ui.scrollPosts->layout());
    if (layout) layout->insertWidget(0, postWidget);

    ui.newTextPost->clear();
}

void FeedWindow::loadPosts() {
    // 1. Use the SAME path as the delete function
    QString filePath = QCoreApplication::applicationDirPath() + "/posts.json";
    QFile file(filePath);

    // 2. If the file doesn't exist in Debug yet, let's try to grab the "default" one
    if (!file.exists()) {
        qDebug() << "LOG: posts.json not in Debug, falling back to resources...";
        file.setFileName("resources/posts.json");
    }

    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "ERROR: Could not find any posts.json to load!";
        return;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonArray postsArray = doc.array();

    // Clear existing posts so we don't double up
    QLayoutItem* item;
    while ((item = ui.scrollPosts->layout()->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    for (const QJsonValue& value : postsArray) {
        QJsonObject obj = value.toObject();
        Content postData;
        postData.username = obj["username"].toString();
        postData.content = obj["content"].toString();
        postData.likedBy = obj["likedBy"].toVariant().toStringList();

        Posts* postWidget = new Posts(this);
        // Pass the user so the delete button knows to show up
        postWidget->setPostData(postData, currentUser.username);

        ui.scrollPosts->layout()->addWidget(postWidget);
    }
}

void FeedWindow::onQuitClicked()
{
    // This safely and immediately shuts down the entire application
    QApplication::quit();
}

void FeedWindow::onSignOutClicked()
{
    // 1. Create a fresh Login Window
    LoginWindow* login = new LoginWindow();
    login->setAttribute(Qt::WA_DeleteOnClose); // Clean up memory when closed
    login->show();

    // 2. Close the Feed Window
    this->close();
}