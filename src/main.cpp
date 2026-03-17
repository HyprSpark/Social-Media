// -- Libraries --
#include <iostream>
#include <QApplication>

// -- Headers --
#include "LoginWindow.h"

using namespace std;

int main(int argc, char* argv[]) {

    QApplication app(argc, argv);

    // Create the login window on the heap instead of the stack
    LoginWindow* w = new LoginWindow();

    // Tell Qt to safely delete it from memory when it closes
    w->setAttribute(Qt::WA_DeleteOnClose);

    w->show();

    return app.exec();
}