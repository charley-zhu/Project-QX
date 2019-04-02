#include "mywidget.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	mywidget w;
	w.setWindowTitle(QStringLiteral("上行采集程序"));
	w.show();
	return a.exec();
}
