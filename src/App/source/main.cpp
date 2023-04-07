#include <iostream>
#include <QApplication>
#include "ExMainWindow.h"
int main(int argc, char* argv[])
{
	QApplication a(argc, argv);

	ExMainWindow w;

	w.show();

	return a.exec();
}
