// -- Headers --
#include "SignUpWindow.h"
#include "LogInWindow.h"

SignUpWindow::SignUpWindow(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);

    connect(ui.logInButton, &QPushButton::clicked, // Switch function
        this, &SignUpWindow::onLogInClicked);
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