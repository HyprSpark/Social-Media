#include "Profile.h"
#include "FeedWindow.h"
#include "Posts.h"
#include <QWidget>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

Profile::Profile(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	QVBoxLayout* layout = new QVBoxLayout(ui.scrollProfilePosts);
	layout->setAlignment(Qt::AlignTop);

	connect(ui.btnToFeed, &QPushButton::clicked,
		this, &Profile::onReturnClicked);

}

Profile::~Profile()
{}

void Profile::setActiveUser(const User& user) {
    currentUser = user;
    ui.lblUsername->setText(currentUser.username);

    // --- The Visual Sync for Header Avatar ---
    // Synchronizing with the path used in the Posts widget
    QPixmap avatar(":resources/images/profile.png");

    if (avatar.isNull()) {
        qDebug() << "ERROR: Profile Avatar image not found!";
        ui.lblProfilePic->setText("?");
        ui.lblProfilePic->setAlignment(Qt::AlignCenter);
    }
    else {
        // Matching the smooth scaling and 80x80 size we planned earlier
        ui.lblProfilePic->setPixmap(avatar.scaled(80, 80, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        ui.lblProfilePic->setFixedSize(80, 80);
        ui.lblProfilePic->setAlignment(Qt::AlignCenter);
        ui.lblProfilePic->setStyleSheet("background: transparent; border: 1px solid #555;");
    }

    loadUserPosts();
}

void Profile::onReturnClicked() {
	this->hide(); 
}

void Profile::loadUserPosts() {
    // Addressing the QLayout warning: Use the existing layout instead of creating a new one
    QLayout* layout = ui.scrollProfilePosts->layout();

    // Clear the layout carefully
    if (layout) {
        QLayoutItem* item;
        while ((item = layout->takeAt(0)) != nullptr) {
            if (item->widget()) {
                delete item->widget();
            }
            delete item;
        }
    }
    else {
        // If no layout exists in the UI file, only then create one
        layout = new QVBoxLayout(ui.scrollProfilePosts);
        layout->setAlignment(Qt::AlignTop);
    }

    QFile file("resources/posts.json");
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray data = file.readAll();
        file.close();
        QJsonDocument doc = QJsonDocument::fromJson(data);
        QJsonArray postsArray = doc.array();

        for (const QJsonValue& value : postsArray) {
            QJsonObject obj = value.toObject();
            if (obj["username"].toString() == currentUser.username) {
                Content postData;
                postData.username = obj["username"].toString();
                postData.content = obj["content"].toString();

                // Parsing Likes
                QJsonArray arr = obj["likedBy"].toArray();
                for (auto v : arr) postData.likedBy << v.toString();

                // Fleshing out the internal list with the post widgets
                Posts* postWidget = new Posts(this);
                postWidget->setPostData(postData, currentUser.username);
                layout->addWidget(postWidget);
            }
        }
    }
    else {
        qDebug() << "LOG: posts.json not found in resources.";
    }
}
