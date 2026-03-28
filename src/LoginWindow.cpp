// -- Headers --
#include "LoginWindow.h"
#include "SignUpWindow.h"
#include "FeedWindow.h"
#include "userManager.h"

#include <QPushButton>
#include <QPixmap>
#include <QDebug>
#include <QFile>

/**
* @brief Helper function to scale images for the login background
* This ensures the image fills the window without distortion
*/
static QPixmap coverPixmap(const QPixmap& src, const QSize& size)
{
    return src.scaled(size, Qt::KeepAspectRatioByExpanding, 
        Qt::SmoothTransformation);
}

LoginWindow::LoginWindow(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    
	// -- Button Connections -- //
   connect(ui.signInButton, &QPushButton::clicked, 
       this, &LoginWindow::onSignInClicked);

   connect(ui.signUpButton, &QPushButton::clicked,
       this, &LoginWindow::onSignUpClicked);

   // -- Load and Set Background Image -- //
   // Using the Qt Resource System to load the image.
    QPixmap px(":/resources/images/LoginBackground.png");

    if (px.isNull()) {
        qDebug() << "ERROR: Pixmap is null!";

        // This check confirms whether the file is actually present in the resource system.
        if (QFile::exists(":/resources/images/LoginBackground.png")) {
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

/**
* @brief This is the initial login window the user will see when they start the program
* It allows the user to enter their email and password to sign in.
* Connects the sign up window for users who don't have an account yet, allowing them to create one.
*/

LoginWindow::~LoginWindow() {}


/**
* @brief Handles the Sign In button click.
* Checks inputs, validates credentials, and transitions to the Feed.
*/
void LoginWindow::onSignInClicked()
{
    User loggedInUser;

    QString email = ui.emailEdit->text();
    QString password = ui.passwordEdit->text();

	// Basic validation, prevents processing empty fields.
    if (email.isEmpty() || password.isEmpty()) {
        ui.statusLabel->setText("Enter email and password.");
        return;
    }

    // Compare data with the user manager.
    if (UserManager::authenticate(email, password, loggedInUser)) {
	    
		// Pass the user data to the feed window so it can display the correct info and posts.
		FeedWindow* feed = new FeedWindow();
		feed->setActiveUser(loggedInUser);
		feed->setAttribute(Qt::WA_DeleteOnClose);
		feed->show();
		this->hide(); // Closes the login window
    }

    else {
		// If authentication fails, show an error message.
        ui.statusLabel->setText("Invalid email or password.");
    }
}

/**
* @brief Handles the Sign Up button click.
*/

void LoginWindow::onSignUpClicked()
{
    SignUpWindow* signup = new SignUpWindow(this);
    signup->show();
    this->hide();   // hides login while signup is open
}