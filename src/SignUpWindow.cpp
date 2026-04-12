// -- Headers --
#include "SignUpWindow.h"
#include "LogInWindow.h"
#include "userManager.h"
#include <QPushButton>
#include <QPixmap>
#include <QDebug>
#include <QFile>

SignUpWindow::SignUpWindow(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    qDebug() << "[INFO] UI: Initializing SignUpWindow.";

    // -- Button Connections -- //
    connect(ui.logInButton, &QPushButton::clicked,
        this, &SignUpWindow::onLogInClicked);

    connect(ui.createAccButton, &QPushButton::clicked,
        this, &SignUpWindow::onCreateAccClicked);

    // -- Background Image Setup -- //
    qDebug() << "[DEBUG] UI: Attempting to load Sign-In background from resources.";
    QPixmap px(":/resources/images/SignInBackground.png");

    if (px.isNull()) {
        qDebug() << "[ERROR] UI: Pixmap is null!";

        if (QFile::exists(":/resources/images/SignInBackground.png")) {
            qDebug() << "[DEBUG] UI: File exists in resources, but QPixmap failed to load (check format).";
        }
        else {
            qDebug() << "[ERROR] UI: File does NOT exist in resource system path.";
        }
    }
    else {
        qDebug() << "[SUCCESS] UI: Sign-In background image loaded. Size:" << px.size();
        ui.heroImageLabel->setScaledContents(true);
        ui.heroImageLabel->setGeometry(0, 0, 1000, 600);
        ui.heroImageLabel->setPixmap(px);
        ui.heroImageLabel->lower(); // Send to back
    }
}

SignUpWindow::~SignUpWindow() {}

void SignUpWindow::onLogInClicked()
{
    qDebug() << "[INFO] UI: Returning to LoginWindow.";
    LoginWindow* login = new LoginWindow(); // Note: Changed to no parent for a clean swap
    login->setAttribute(Qt::WA_DeleteOnClose);
    login->show();
    this->close(); // Closes and deletes the signup window
}

void SignUpWindow::onCreateAccClicked()
{
    // Trim inputs to prevent invisible space errors
    QString username = ui.usernameEdit->text().trimmed();
    QString email = ui.emailEdit->text().trimmed();
    QString password = ui.passEdit->text();
    QString passwordConf = ui.passConfEdit->text();

    qDebug() << "[INFO] Auth: Registration attempt for username:" << username;

    // 1. Empty Field Check
    if (username.isEmpty() || email.isEmpty() || password.isEmpty() || passwordConf.isEmpty()) {
        qDebug() << "[DEBUG] Auth: Rejected signup due to empty fields.";
        ui.statusLabel->setText("Please fill in all fields.");
        return;
    }

    // 2. Email Validation
    if (!email.contains("@") || !email.endsWith(".com")) {
        qDebug() << "[DEBUG] Auth: Invalid email format rejected:" << email;
        ui.statusLabel->setText("Please enter a valid .com email.");
        return;
    }

    // 3. Password Length
    if (password.length() < 6) {
        qDebug() << "[DEBUG] Auth: Password length too short.";
        ui.statusLabel->setText("Password must be at least 6 characters.");
        return;
    }

    // 4. Password Confirmation
    if (password != passwordConf) {
        qDebug() << "[DEBUG] Auth: Password confirmation mismatch.";
        ui.statusLabel->setText("Passwords do not match.");
        return;
    }

    // 5. Check for Spaces in Username
    if (username.contains(" ")) {
        qDebug() << "[DEBUG] Auth: Username contains forbidden spaces.";
        ui.statusLabel->setText("Username cannot contain spaces.");
        return;
    }

    // 6. Check if username or email is already taken
    qDebug() << "[DEBUG] Auth: Checking database for username/email uniqueness.";
    if (!UserManager::isUnique(username, email)) {
        qDebug() << "[ERROR] Auth: Username or Email already exists in User.json.";
        ui.statusLabel->setText("Username or Email is already taken.");
        return;
    }

    // If all checks pass, create the new user and save it.
    User newUser(username, email, password);
    qDebug() << "[DEBUG] Domain: New User object instantiated.";

    UserManager::saveUser(newUser);
    qDebug() << "[SUCCESS] Auth: Account created successfully for" << username;

    onLogInClicked();
}