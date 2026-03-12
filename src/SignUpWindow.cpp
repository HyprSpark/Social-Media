// -- Headers --
#include "SignUpWindow.h"
#include "LogInWindow.h"
#include "userManager.h"

// -- Libraries --
#include <QPushButton>
#include <QPixmap>
#include <QDebug>
#include <QFile>

SignUpWindow::SignUpWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

    connect(ui.logInButton, &QPushButton::clicked, // Switch function
        this, &SignUpWindow::onLogInClicked);

    connect(ui.createAccButton, &QPushButton::clicked, // Sign up function
		this, &SignUpWindow::onCreateAccClicked);

    QPixmap px(":/resources/images/SignInBackground.png"); // Load the image located in the Resource.qrc file

    if (px.isNull()) {
        qDebug() << "ERROR: Pixmap is null!";

        // Check if the file even exists in the resource system
        if (QFile::exists(":/resources/images/SignInBackground.png")) {
            qDebug() << "SUCCESS: File exists in resources, but QPixmap failed to load it (Format issue?)";
        }
        else {
            qDebug() << "FAILURE: File does NOT exist at that path in the resource system.";
        }
    }
    else {
        qDebug() << "SUCCESS: Image loaded! Size:" << px.size();
        ui.heroImageLabel->setScaledContents(true);
        ui.heroImageLabel->setGeometry(0, 0, 1000, 600); // Align with window
        qDebug() << "image size changed:" << px.size();
        ui.heroImageLabel->lower(); // Send to back so it's a true background
    }
    ui.heroImageLabel->setPixmap(px);
}

SignUpWindow::~SignUpWindow()
{}

// - Switching to the log in page when the user chooses log in
void SignUpWindow::onLogInClicked() 
{
    LoginWindow* login = new LoginWindow(this);
    login->show(); // Displays the signup window
    this->hide();   // hides login while signup is open
}

void SignUpWindow::onCreateAccClicked()
{
	QString username = ui.usernameEdit->text();
	QString email = ui.emailEdit->text();
	QString password = ui.passEdit->text();
	QString passwordConf = ui.passConfEdit->text();

	// 1. Empty Field Check (Did they forget to fill something in?)
	if (username.isEmpty() || email.isEmpty() || password.isEmpty() || passwordConf.isEmpty()) {
		ui.statusLabel->setText("Please fill in all fields.");
		return;

	}

    // 2. Email Validation (Basic check for @ and .com)
    if (!email.contains("@") || !email.endsWith(".com")) {
        ui.statusLabel->setText("Please enter a valid .com email.");
        return;
    }

    // 3. Password Length (Minimum 6 characters)
    if (password.length() < 6) {
        ui.statusLabel->setText("Password must be at least 6 characters.");
        return;
    }

    if (username.contains(" ")) {
        ui.statusLabel->setText("Username cannot contain spaces.");
        return;
    }

    // 2. Check for Spaces in Email (Emails shouldn't have them anyway)
    if (email.contains(" ")) {
        ui.statusLabel->setText("Email cannot contain spaces.");
        return;
    }

    // 3. Check for Uniqueness
    if (!UserManager::isUnique(username, email)) {
        ui.statusLabel->setText("Username or Email is already taken.");
        return;
    }

	User newUser(username, email, password);
	UserManager::saveUser(newUser);

	onLogInClicked();
}