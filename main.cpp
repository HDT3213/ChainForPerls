#include "mainwindow.h"
#include "autoplayerthread.h"

#include <QApplication>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	AutoPlayerThread thread;
	MainWindow w(&thread);
    a.setWindowIcon(QIcon(":/Icon/Debug/Perls.ico"));
	w.setWindowTitle("ChainOfPerls");
    w.show();
	QObject::connect(&w, SIGNAL(needReact()), &thread, SLOT(react()));
	QObject::connect(&thread, SIGNAL(reacted(Step)), &w, SLOT(updateReact(Step)));
    return a.exec();
}
