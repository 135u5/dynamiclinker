#include <QApplication>
#include <stdio.h>
#include "mainwindowimpl.h"

int main(int argc, char ** argv)
{
	QApplication app( argc, argv );
	MainWindowImpl m;	
	m.show(); 
	app.connect( &app, SIGNAL( lastWindowClosed() ), &app, SLOT( quit() ) );	
	return app.exec();
}
