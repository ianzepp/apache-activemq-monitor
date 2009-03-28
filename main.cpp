#include "AMQMonitor.h"

#include <QtGui>
#include <QApplication>
#include <QDebug>
#include <iostream>

void debugHandler(QtMsgType type, const char *msg)
{
	switch (type)
	{
	case QtDebugMsg:
		std::cerr << "[DEBUG]    : " << msg << "\n";
		break;
	case QtWarningMsg:
		std::cerr << "[WARNING]  : " << msg << "\n";
		break;
	case QtSystemMsg:
		std::cerr << "[SYSTEM]   : " << msg << "\n";
		break;
	case QtFatalMsg:
		std::cerr << "[FATAL]    : " << msg << "\n";
		abort(); // dump core on purpose
	}
}

int main(int argc, char *argv[])
{
	qInstallMsgHandler(debugHandler);

	QApplication a(argc, argv);
	AMQMonitor w;
	w.show();
	return a.exec();
}
