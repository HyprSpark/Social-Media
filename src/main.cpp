#include <iostream>
#include <QApplication>
#include "QtWidgetsClass.h"
#include "LoginWindow.h"

using namespace std;

int main(int argc, char *argv[]) {

	QApplication app(argc, argv);

	LoginWindow w;
	w.show();

	return app.exec();

}