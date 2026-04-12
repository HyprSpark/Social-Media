// -- Headers --
#include "LoginWindow.h"
#include "SignUpWindow.h"
#include "FeedWindow.h"
#include "UserManager.h"
#include <QPushButton>
#include <QPixmap>
#include <QDebug>
#include <QFile>

/** * @brief Helper function to scale images for the login background
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
    qDebug() << "[INFO] UI: Initializing LoginWindow.";

    // -- Button Connections --
    connect(ui.signInButton, &QPushButton::clicked,
        this, &LoginWindow::onSignInClicked);

    connect(ui.signUpButton, &QPushButton::clicked,
        this, &LoginWindow::onSignUpClicked);

    // -- QoL: Enter Key Support --
    // This allows the user to hit 'Enter' from either text box to trigger the sign-in
    connect(ui.emailEdit, &QLineEdit::returnPressed, this, &LoginWindow::onSignInClicked);
    connect(ui.passwordEdit, &QLineEdit::returnPressed, this, &LoginWindow::onSignInClicked);

    // -- Load and Set Background Image --
    qDebug() << "[DEBUG] UI: Attempting to load background image from resources.";
    QPixmap px(":/resources/images/LoginBackground.png");

    if (px.isNull()) {
        qDebug() << "[ERROR] UI: Pixmap is null!";
        if (!QFile::exists(":/resources/images/LoginBackground.png")) {
            qDebug() << "[ERROR] UI: File does NOT exist in the resource system path.";
        }
    }
    else {
        qDebug() << "[SUCCESS] UI: Background image loaded.";
        ui.heroImageLabel->setScaledContents(true);
        ui.heroImageLabel->setGeometry(0, 0, 1000, 600);
        ui.heroImageLabel->setPixmap(px);
        ui.heroImageLabel->lower(); // Send to background layer
    }
}

LoginWindow::~LoginWindow() {}

void LoginWindow::onSignInClicked()
{
    qDebug() << "[INFO] Auth: Sign-in attempt initiated.";
    User loggedInUser;

    QString email = ui.emailEdit->text().trimmed();
    QString password = ui.passwordEdit->text();

    if (email.isEmpty() || password.isEmpty()) {
        qDebug() << "[DEBUG] Auth: Rejected empty email or password fields.";
        ui.statusLabel->setText("Enter email and password.");
        return;
    }

    if (UserManager::authenticate(email, password, loggedInUser)) {
        qDebug() << "[SUCCESS] Auth: Login verified for" << loggedInUser.username;

        FeedWindow* feed = new FeedWindow();
        feed->setActiveUser(loggedInUser);
        feed->setAttribute(Qt::WA_DeleteOnClose);
        feed->show();

        qDebug() << "[INFO] UI: Transitioning to FeedWindow.";
        this->hide();
    }
    else {
        qDebug() << "[ERROR] Auth: Invalid credentials for" << email;
        ui.statusLabel->setText("Invalid email or password.");

        // -- The Password Wipe --
        // Clears the field so the user can immediately re-type without hitting backspace
        ui.passwordEdit->clear();
        ui.passwordEdit->setFocus();
    }
}

void LoginWindow::onSignUpClicked()
{
    qDebug() << "[INFO] UI: Redirecting to SignUpWindow.";
    SignUpWindow* signup = new SignUpWindow(this);
    signup->show();
    this->hide();
}