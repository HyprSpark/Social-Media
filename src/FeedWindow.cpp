#include "FeedWindow.h"
#include "Profile.h"

FeedWindow::FeedWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	connect(ui.btnMyProfile, &QPushButton::clicked, this, &FeedWindow::onMyProfileClicked);
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

