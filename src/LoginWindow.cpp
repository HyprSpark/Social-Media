// -- Headers --
#include "LoginWindow.h"

// -- Libraries --
#include <QPushButton>
#include <QPixmap>

static QPixmap coverPixmap(const QPixmap& src, const QSize& size)
{
    return src.scaled(size, Qt::KeepAspectRatioByExpanding,
        Qt::SmoothTransformation);
}

LoginWindow::LoginWindow(QWidget* parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

   connect(ui.signInButton, &QPushButton::clicked,
        this, &LoginWindow::onSignInClicked);

    QPixmap px(":/resources/images/cheers.jpg"); // Load the image located in the Resource.qrc file
    ui.heroImageLabel->setPixmap(px);
    
    if (px.isNull()) { // If file is missing or corrupt show error message
        ui.statusLabel->setText("Image not found (resource path wrong).");
    }
    
}

LoginWindow::~LoginWindow() {}

void LoginWindow::onSignInClicked()
{
    QString email = ui.emailEdit->text();
    QString password = ui.passwordEdit->text();

    if (email.isEmpty() || password.isEmpty()) {
        ui.statusLabel->setText("Enter email and password.");
        return;
    }

    if (email == "test" && password == "1234") {
        close();
    }
    else {
        ui.statusLabel->setText("Invalid email or password.");
    }
}