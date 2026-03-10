// -- Headers --
#include "SignUpWindow.h"
#include "LogInWindow.h"
#include "userManager.h"

SignUpWindow::SignUpWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

    connect(ui.logInButton, &QPushButton::clicked, // Switch function
        this, &SignUpWindow::onLogInClicked);

    connect(ui.createAccButton, &QPushButton::clicked, // Sign up function
		this, &SignUpWindow::onCreateAccClicked);
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

	if (username.isEmpty() || email.isEmpty() || password.isEmpty() || passwordConf.isEmpty()) {
		ui.statusLabel->setText("Please fill in all fields.");
		return;

	}

	User newUser(username, email, password);
	UserManager::saveUser(newUser);

	onLogInClicked();
}