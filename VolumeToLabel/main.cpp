#include "VolumeToLabel.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	VolumeToLabel w;
	w.show();
	return a.exec();
}
