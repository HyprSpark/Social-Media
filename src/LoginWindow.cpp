// -- Headers --
#include "LoginWindow.h"
#include "SignUpWindow.h"
#include "FeedWindow.h"
#include "userManager.h"

// -- Libraries --
#include <QPushButton>
#include <QPixmap>
#include <QDebug>
#include <QFile>

static QPixmap coverPixmap(const QPixmap& src, const QSize& size)
{
    return src.scaled(size, Qt::KeepAspectRatioByExpanding,
        Qt::SmoothTransformation);
}

LoginWindow::LoginWindow(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

   connect(ui.signInButton, &QPushButton::clicked, // login function
        this, &LoginWindow::onSignInClicked);

   connect(ui.signUpButton, &QPushButton::clicked, // Switch function
       this, &LoginWindow::onSignUpClicked);

    QPixmap px(":/resources/images/LoginBackground.png"); // Load the image located in the Resource.qrc file

    if (px.isNull()) {
        qDebug() << "ERROR: Pixmap is null!";

        // Check if the file even exists in the resource system
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

LoginWindow::~LoginWindow() {}

void LoginWindow::onSignInClicked()
{
    User loggedInUser;

    QString email = ui.emailEdit->text();
    QString password = ui.passwordEdit->text();

    if (email.isEmpty() || password.isEmpty()) {
        ui.statusLabel->setText("Enter email and password.");
        return;
    }

    // Giving test data 
    if (UserManager::authenticate(email, password, loggedInUser)) {
	
		FeedWindow* feed = new FeedWindow(loggedInUser); // Creates a new instance of the FeedWindow class

		feed->setAttribute(Qt::WA_DeleteOnClose); // Ensures the feed window is deleted from memory when closed

		feed->show(); // Displays the feed window

		this->close(); // Closes the login window
    }
    else {
        ui.statusLabel->setText("Invalid email or password.");
    }
}
// - Switching to the Sign Up page when the user chooses create account
void LoginWindow::onSignUpClicked()
{
    SignUpWindow* signup = new SignUpWindow(this);
    signup->show(); // Displays the signup window
    this->hide();   // hides login while signup is open
}