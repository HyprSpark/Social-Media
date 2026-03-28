// -- Header -- //
#include <QApplication>
#include <QFile>
#include <QDebug>
#include "LoginWindow.h"

int main(int argc, char* argv[]) {

    QApplication app(argc, argv); // Initializes the Qt application.

    LoginWindow* w = new LoginWindow(); // Creates a new instance of the LoginWindow class.

    w->setAttribute(Qt::WA_DeleteOnClose); // Ensures that the LoginWindow instance is deleted from memory when it is closed.

    w->show(); // Displays the LoginWindow on the screen.

    return app.exec(); // Enters the main event loop of the application.
}