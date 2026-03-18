#include "FeedWindow.h"
#include "Profile.h"
#include "Posts.h"
#include "Content.h" // Ensure this defines the Content struct
#include <QPushButton>
#include <QString>
#include <QVBoxLayout>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

FeedWindow::FeedWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	QVBoxLayout* layout = new QVBoxLayout(ui.scrollPosts);
	layout->setAlignment(Qt::AlignTop);

	connect(ui.btnMyProfile, &QPushButton::clicked, this, &FeedWindow::onMyProfileClicked);
	connect(ui.btnSubmitPost, &QPushButton::clicked, this, &FeedWindow::onSubmitPostClicked);

	loadPosts();
}

FeedWindow::~FeedWindow() {}

void FeedWindow::setActiveUser(const User& user) {
	currentUser = user;
}

void FeedWindow::onMyProfileClicked() {

	Profile* profile = new Profile();
	profile->setActiveUser(currentUser);
	profile->setAttribute(Qt::WA_DeleteOnClose); // Ensure the window is deleted when closed
	profile->show();
	this->close();

}

void FeedWindow::onSubmitPostClicked() {
	QString contentText = ui.newTextPost->toPlainText().trimmed();

	if (contentText.isEmpty()) { return; } // Doesn't submit empty posts 

    // 1. Create the new post object
    QJsonObject newPost;
    newPost["username"] = currentUser.username; // Use the logged-in user
    newPost["content"] = contentText;
    newPost["likes"] = 0; // Initialize at 0

    // 2. Load existing posts from file
    QFile file("resources/posts.json");
    QJsonArray postsArray;

    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        if (doc.isArray()) {
            postsArray = doc.array();
        }
    }

    // 3. Add the new post to the array (prepend to show at the top)
    postsArray.prepend(newPost);

    // 4. Save the updated array back to the JSON file
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(postsArray);
        file.write(doc.toJson());
        file.close();
    }

    // 5. Update the UI immediately
    // Create a new post widget and insert it at the top of the layout
    Posts* postWidget = new Posts(this);
    Content data;
    data.username = currentUser.username;
    data.content = contentText;
    data.likes = 0;

    postWidget->setPostData(data);

    // Insert at index 0 so it appears at the top of the scroll area
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(ui.scrollPosts->layout());
    if (layout) {
        layout->insertWidget(0, postWidget);
    }

    // 6. Clear the input box
    ui.newTextPost->clear();
}


void FeedWindow::loadPosts() {
    QFile file("resources/Posts.json"); 
    if (!file.open(QIODevice::ReadOnly)) return;

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    QJsonArray postsArray = doc.array();

    for (const QJsonValue& value : postsArray) {
        QJsonObject obj = value.toObject();

        Content postData;
        postData.username = obj["username"].toString();
        postData.content = obj["content"].toString();
        postData.likes = obj["likes"].toInt();

        Posts* postWidget = new Posts(this);
        postWidget->setPostData(postData);

        ui.scrollPosts->layout()->addWidget(postWidget);
    }
}

