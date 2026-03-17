#include "Profile.h"
#include "FeedWindow.h"

Profile::Profile(QWidget* parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	connect(ui.btnToFeed, &QPushButton::clicked,
		this, &Profile::onReturnClicked);

}

Profile::~Profile()
{}

void Profile::setActiveUser(const User& user) {
	currentUser = user;
	ui.lblUsername->setText(currentUser.username);
}

void Profile::onReturnClicked() {
	FeedWindow* feed = new FeedWindow();
	feed->setActiveUser(currentUser); // Pass the active user to the feed window
	feed->setAttribute(Qt::WA_DeleteOnClose);
	feed->show(); // Displays the feed window
	this->close(); 
}

