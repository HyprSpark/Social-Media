// -- Headers --
#include "SignUpWindow.h"
#include "LogInWindow.h"
#include "userManager.h"
#include <QPushButton>
#include <QPixmap>
#include <QDebug>
#include <QFile>

/**
* @brief This window allows the user to make a new account.
* Captures the username, email, and password, checking if the username/email doesn't already exist.
* Checks for proper email formatting and password confirmation before creating the account.
* Connects back to the login window when user is created.
*/

SignUpWindow::SignUpWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

	// -- Button Connections -- //
    connect(ui.logInButton, &QPushButton::clicked,
        this, &SignUpWindow::onLogInClicked);

    connect(ui.createAccButton, &QPushButton::clicked,
		this, &SignUpWindow::onCreateAccClicked);

	// -- Background Image Setup -- //
    QPixmap px(":/resources/images/SignInBackground.png");

    if (px.isNull()) {
        qDebug() << "ERROR: Pixmap is null!";

        // Check if the file even exists in the resource system.
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

/**
* @brief Brings the user back to the login window.
*/
void SignUpWindow::onLogInClicked() 
{
    LoginWindow* login = new LoginWindow(this);
    login->show(); // Displays the signup window
    this->hide();   // hides login while signup is open
}

/**
* @brief Handles the Create Account button click.
* Checks inputs for validity, creates a new user, saves it, and returns to the login window.
*/
void SignUpWindow::onCreateAccClicked()
{
	QString username = ui.usernameEdit->text();
	QString email = ui.emailEdit->text();
	QString password = ui.passEdit->text();
	QString passwordConf = ui.passConfEdit->text();

	// 1. Empty Field Check
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
    
	// 4. Password Confirmation
    if (password != passwordConf) {
        ui.statusLabel->setText("Passwords do not match.");
        return;
    }

	// 5. Check for Spaces in Username
    if (username.contains(" ")) {
        ui.statusLabel->setText("Username cannot contain spaces.");
        return;
    }

	// 6. Check for Spaces in Email
    if (email.contains(" ")) {
        ui.statusLabel->setText("Email cannot contain spaces.");
        return;
    }

	// 7. Check if username or email is already taken
    if (!UserManager::isUnique(username, email)) {
        ui.statusLabel->setText("Username or Email is already taken.");
        return;
    }

	// If all checks pass, create the new user and save it.
	User newUser(username, email, password);
	UserManager::saveUser(newUser);

	onLogInClicked(); // Return to login after successful account creation
}