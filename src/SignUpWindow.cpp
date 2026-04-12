// -- Headers --
#include "SignUpWindow.h"
#include "LoginWindow.h" // Note: Ensured case matches your renamed file
#include "UserManager.h"
#include <QPushButton>
#include <QPixmap>
#include <QDebug>
#include <QFile>
#include <QLineEdit>

SignUpWindow::SignUpWindow(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    qDebug() << "[INFO] UI: Initializing SignUpWindow.";

    // -- Button Connections --
    connect(ui.logInButton, &QPushButton::clicked,
        this, &SignUpWindow::onLogInClicked);

    connect(ui.createAccButton, &QPushButton::clicked,
        this, &SignUpWindow::onCreateAccClicked);

    // -- QoL: Enter Key Support from ANY box --
    // This allows the user to finish typing and just hit Enter to create the account
    connect(ui.usernameEdit, &QLineEdit::returnPressed, this, &SignUpWindow::onCreateAccClicked);
    connect(ui.emailEdit, &QLineEdit::returnPressed, this, &SignUpWindow::onCreateAccClicked);
    connect(ui.passEdit, &QLineEdit::returnPressed, this, &SignUpWindow::onCreateAccClicked);
    connect(ui.passConfEdit, &QLineEdit::returnPressed, this, &SignUpWindow::onCreateAccClicked);

    // -- Background Image Setup --
    QPixmap px(":/resources/images/SignInBackground.png");
    if (!px.isNull()) {
        ui.heroImageLabel->setScaledContents(true);
        ui.heroImageLabel->setGeometry(0, 0, 1000, 600);
        ui.heroImageLabel->setPixmap(px);
        ui.heroImageLabel->lower();
    }
}

SignUpWindow::~SignUpWindow() {}

void SignUpWindow::onLogInClicked()
{
    qDebug() << "[INFO] UI: Returning to LoginWindow.";
    LoginWindow* login = new LoginWindow();
    login->setAttribute(Qt::WA_DeleteOnClose);
    login->show();
    this->close();
}

void SignUpWindow::onCreateAccClicked()
{
    QString username = ui.usernameEdit->text().trimmed();
    QString email = ui.emailEdit->text().trimmed();
    QString password = ui.passEdit->text();
    QString passwordConf = ui.passConfEdit->text();

    // Lambda to wipe password fields on any validation error
    auto wipePasswords = [this]() {
        ui.passEdit->clear();
        ui.passConfEdit->clear();
        ui.passEdit->setFocus();
        };

    // 1. Empty Field Check
    if (username.isEmpty() || email.isEmpty() || password.isEmpty() || passwordConf.isEmpty()) {
        ui.statusLabel->setText("Please fill in all fields.");
        wipePasswords();
        return;
    }

    // 2. Email Validation
    if (!email.contains("@") || !email.endsWith(".com")) {
        ui.statusLabel->setText("Please enter a valid .com email.");
        wipePasswords();
        return;
    }

    // 3. Password Length
    if (password.length() < 6) {
        ui.statusLabel->setText("Password must be at least 6 characters.");
        wipePasswords();
        return;
    }

    // 4. Password Confirmation
    if (password != passwordConf) {
        ui.statusLabel->setText("Passwords do not match.");
        wipePasswords();
        return;
    }

    // 5. Check for Spaces in Username
    if (username.contains(" ")) {
        ui.statusLabel->setText("Username cannot contain spaces.");
        wipePasswords();
        return;
    }

    // 6. Check for Uniqueness
    if (!UserManager::isUnique(username, email)) {
        ui.statusLabel->setText("Username or Email is already taken.");
        wipePasswords();
        return;
    }

    // SUCCESS
    User newUser(username, email, password);
    UserManager::saveUser(newUser);
    qDebug() << "[SUCCESS] Auth: Account created successfully for" << username;

    onLogInClicked();
}