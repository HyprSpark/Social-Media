// -- Header --
#include <QApplication>
#include <QFile>
#include <QDebug>
#include "LoginWindow.h"

int main(int argc, char* argv[]) {
    // Standardizing the start of the application log
    qDebug() << "[INFO] App: Starting Social Media Platform execution.";

    QApplication app(argc, argv);

    qDebug() << "[DEBUG] App: Initializing LoginWindow instance.";
    LoginWindow* w = new LoginWindow();

    // This is a key OOD detail: ensuring proper memory cleanup via attributes
    w->setAttribute(Qt::WA_DeleteOnClose);
    qDebug() << "[DEBUG] App: Set WA_DeleteOnClose for LoginWindow memory management.";

    qDebug() << "[INFO] UI: Launching main interface.";
    w->show();

    // Capturing the exit code for the audit trail
    int exitCode = app.exec();

    qDebug() << "[INFO] App: Execution finished. Exit code:" << exitCode;
    return exitCode;
}