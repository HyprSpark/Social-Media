// -- Libraries --
#include <iostream>
#include <QApplication>

// -- Headers --
#include "LoginWindow.h"

using namespace std;

int main(int argc, char *argv[]) {

	QApplication app(argc, argv); // Accessing and running Qt

	LoginWindow w; //Assigning LoginWindow as "w"

	w.show(); // Printing LoginWindow to user screen

	return app.exec();

}