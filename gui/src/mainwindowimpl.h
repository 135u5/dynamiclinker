#ifndef MAINWINDOWIMPL_H
#define MAINWINDOWIMPL_H
#include "elf.h"
//
#include <QMainWindow>
#include <QWidget>
#include "ui_mainwindow.h"
//
class MainWindowImpl : public QMainWindow, public Ui::MainWindow
{
Q_OBJECT
public:
	MainWindowImpl( QWidget * parent = 0, Qt::WFlags f = 0 );
	
private slots:
void browse_elf();
void browse_st();
void full_linking();
void light_linking();
void browse_wlf();
void load_wlf();
void unload_then_load_wlf();
void connect_to_telosb();
void reset();
void erase();
void browseBootstrap();
void bootstrap();
void bootstrapNoErase();


private:
QString elfFile, bootFile,symbol_table,wlf_file;
void show_system_output(char*file_name);
elf full_linker;
};
#endif




