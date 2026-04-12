#include "PostWidget.h"
#include "models/Post.h"
#include "ProfileWindow.h"
#include "UserManager.h"
#include "FeedWindow.h"
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QCoreApplication>

PostWidget::PostWidget(QWidget* parent)
    : QWidget(parent), isLiked(false)
{
    ui.setupUi(this);

    // Apply basic styling
    this->setStyleSheet(
        "PostWidget { "
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

    ui.btnUsername->setText(data.senderUsername);
    ui.lblContent->setText(data.textContent);
    ui.lblTimestamp->setText(data.timestamp);
    ui.lblLikeCount->setText(QString::number(data.likedBy.size()));

    // Setup Like Button State
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

    // Show delete button only for authors
    ui.btnDelete->setVisible(data.senderUsername.trimmed().toLower() == currentUser.trimmed().toLower());
}

void PostWidget::onDeleteClicked()
{
    auto reply = QMessageBox::question(this, "Delete Post", "Are you sure you want to delete this post?");
    if (reply != QMessageBox::Yes) return;

    QString filePath = QCoreApplication::applicationDirPath() + "/../../resources/posts.json";
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return;

    QJsonArray postsArray = QJsonDocument::fromJson(file.readAll()).array();
    file.close();

    QJsonArray updatedArray;
    bool matchFound = false;

    for (const QJsonValue& value : postsArray) {
        QJsonObject obj = value.toObject();
        if (obj["username"].toString() == currentData.senderUsername &&
            obj["content"].toString() == currentData.textContent &&
            obj["timestamp"].toString() == currentData.timestamp) {
            matchFound = true;
        }
        else {
            updatedArray.append(obj);
        }
    }

    if (matchFound && file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        file.write(QJsonDocument(updatedArray).toJson());
        file.close();

        // Hide the widget immediately so the UI feels responsive.
        // The Queued refresh in FeedWindow will handle the actual deletion safely.
        this->hide();
    }
}

void PostWidget::onLikeClicked()
{
    QString filePath = QCoreApplication::applicationDirPath() + "/../../resources/posts.json";
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return;

    QJsonArray postsArray = QJsonDocument::fromJson(file.readAll()).array();
    file.close();

    for (int i = 0; i < postsArray.size(); ++i) {
        QJsonObject obj = postsArray[i].toObject();
        if (obj["username"].toString() == currentData.senderUsername &&
            obj["content"].toString() == currentData.textContent &&
            obj["timestamp"].toString() == currentData.timestamp) {

            QJsonArray likedByArray = obj["likedBy"].toArray();
            QStringList likedList;
            for (const QJsonValue& v : likedByArray) likedList << v.toString();

            if (!isLiked) {
                if (!likedList.contains(currentUser)) likedList.append(currentUser);
                isLiked = true;
            }
            else {
                likedList.removeAll(currentUser);
                isLiked = false;
            }

            QJsonArray newArray;
            for (const QString& name : likedList) newArray.append(name);
            obj["likedBy"] = newArray;
            postsArray[i] = obj;

            if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                file.write(QJsonDocument(postsArray).toJson());
                file.close();
            }

            ui.lblLikeCount->setText(QString::number(likedList.size()));
            ui.btnLike->setText(isLiked ? "Unlike" : "Like");
            ui.btnLike->setStyleSheet(isLiked ? "color: #ff4757; font-weight: bold;" : "");
            break;
        }
    }
}

void PostWidget::onUsernameClicked()
{
    QVector<User> allUsers = UserManager::loadUsers();
    User author;
    User viewer;
    bool found = false;
    for (const User& u : allUsers) {
        if (u.username == currentData.senderUsername) { author = u; found = true; }
        if (u.username == currentUser) viewer = u;
    }
    if (!found) author.username = currentData.senderUsername;

    ProfileWindow* profile = new ProfileWindow();
    profile->setAttribute(Qt::WA_DeleteOnClose);
    profile->setWindowModality(Qt::ApplicationModal);

    // Connect the close signal to refresh the feed
    connect(profile, &ProfileWindow::windowClosed, qobject_cast<FeedWindow*>(this->window()), &FeedWindow::loadPosts);

    profile->setActiveUser(author, viewer);
    profile->show();
}