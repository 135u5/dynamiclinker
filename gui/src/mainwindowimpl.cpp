#include "mainwindowimpl.h"
#include <QtGui>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <string>
#include "symTabGetter.h"
#include "wlf_loader.h"
#define PYTHON "python "
#define SYSTEM_LOG " > system_log 2>&1"
#define SYSTEM_FILE "system_log"
#define BSL_RESET "/mos/bin/bsl.py --telosb -c /dev/ttyUSB0 -r"
#define BSL_ERASE "/mos/bin/bsl.py --telosb -c /dev/ttyUSB0 -e"
#define MOS_SHELL "/mos/bin/mos_shell --sdev /dev/ttyUSB0 -n"
#define BOOTSTRAP_NO_ERASE "/mos/bin/bsl.py --telosb -r -c /dev/ttyUSB0 -p "
#define BOOTSTRAP "/mos/bin/bsl.py --telosb -e -c /dev/ttyUSB0 -p "


//MAIN WINDOW

MainWindowImpl::MainWindowImpl( QWidget * parent, Qt::WFlags f) 
	: QMainWindow(parent, f)
{
	setupUi(this);
	connect(actionExit,SIGNAL(activated()),this, SLOT(close()));
	connect(BrowseELF,SIGNAL(clicked()),this,SLOT(browse_elf()));
	connect(BrowseST,SIGNAL(clicked()),this,SLOT(browse_st()));
	connect(fullLinking,SIGNAL(clicked()),this,SLOT(full_linking()));
	connect(lightLinking,SIGNAL(clicked()),this,SLOT(light_linking()));
	connect(connectToTelosb,SIGNAL(clicked()),this,SLOT(connect_to_telosb()));
	connect(telosReset,SIGNAL(clicked()),this,SLOT(reset()));
	connect(telosErase,SIGNAL(clicked()),this,SLOT(erase()));
	connect(BrowseBootstrap,SIGNAL(clicked()),this,SLOT(browseBootstrap()));
	connect(Bootstrap,SIGNAL(clicked()),this,SLOT(bootstrap()));
	connect(BootstrapNoErase,SIGNAL(clicked()),this,SLOT(bootstrapNoErase()));
	connect(BrowseWLF,SIGNAL(clicked()),this,SLOT(browse_wlf()));
	connect(loadWLF,SIGNAL(clicked()),this,SLOT(load_wlf()));
	connect(unloadThenLoadWLF,SIGNAL(clicked()),this,SLOT(unload_then_load_wlf()));

}


/*------------------------------SYM TAB DOWNLOADER----------------------------*/

void MainWindowImpl::connect_to_telosb(){
	
	if(!symtab->text().isEmpty()){
	 	QString symtab_name ="symbol_tables_database/";
	 	symtab_name.append(symtab->text());
	 	symtab_name.append(".sytab");
		systemOut->clear();
		//launch the symbol table getter	
		getSymTab(systemOut,symtab_name.toAscii().data(),ComboBoxST);
		//enable the operation menu components
	
	}else{
		systemOut->clear();
		systemOut->append("Before download a symbol table you have to specify a name for it. Once downloaded, the relative '.sytab' file will be stored in symbol tables database for future application");
	}

	}


/*---------------------------------ELF MENU----------------------------------*/

 void MainWindowImpl::browse_elf()
    {  elfFile = QFileDialog::getOpenFileName(this,
                                   tr("Open ELF"), QDir::currentPath(),
                                   					tr("ELF files(*.elf)"));       
       ComboBoxELF->addItem(elfFile);
       ComboBoxELF->setCurrentIndex(ComboBoxELF->currentIndex()+1); 
         
    }
     

void MainWindowImpl::browse_st(){
		QString openPath=QDir::currentPath();
		openPath.append("/symbol_tables_database");
    	symbol_table = QFileDialog::getOpenFileName(this,
                                   tr("Open ST"),openPath,tr("SYTAB FILES(*.sytab)"));       
       ComboBoxST->addItem(symbol_table);
       ComboBoxST->setCurrentIndex(ComboBoxST->currentIndex()+1);

}

void MainWindowImpl::full_linking(){
	 
	 if(!ComboBoxELF->currentText().isEmpty() && !ComboBoxST->currentText().isEmpty() && !wlf->text().isEmpty()) {
	 	QString wlf_name="WLF_database/";
		wlf_name.append(wlf->text().toAscii().data());
	 	wlf_name.append(".wlf");
 		full_linker.linker(elfFile.toAscii().data(),symbol_table.toAscii().data(),wlf_name.toAscii().data(),systemOut);
     }else {
 		systemOut->clear();
 		systemOut->append("To start the full linking process, you must specify the ELF file, the symbol table and a name for WLF.");
		}
       						
	
}
void MainWindowImpl::light_linking(){
	elf light_linker;
	 if(!ComboBoxELF->currentText().isEmpty()&& !wlf->text().isEmpty()){
		QString wlf_name="WLF_database/";
		wlf_name.append(wlf->text().toAscii().data());
	 	wlf_name.append(".wlf");
 		light_linker.linker(elfFile.toAscii().data(),NULL,wlf_name.toAscii().data(),systemOut);
     }else {
 		systemOut->clear();
 		systemOut->append("To start the light linking process,you must specify the ELF file and a name for WLF.");
		}
	
}
/*------------------------------WLF MENU--------------------------------------*/

void MainWindowImpl::browse_wlf(){
		QString openPath=QDir::currentPath();
		openPath.append("/WLF_database");
    	wlf_file = QFileDialog::getOpenFileName(this,
                                   tr("Open WLF"),openPath,tr("WLF FILES(*.wlf)"));       
       ComboBoxWLF->addItem(wlf_file);
       ComboBoxWLF->setCurrentIndex(ComboBoxST->currentIndex()+1);

}

void MainWindowImpl::load_wlf(){
	 wlfLoader loader;
	 if(!ComboBoxWLF->currentText().isEmpty()) {
 		loader.load(wlf_file.toAscii().data());
		}else {
 		systemOut->clear();
 		systemOut->append("You must select a valid -.wlf- file to laod it");
		}
       						
	
}
void MainWindowImpl::unload_then_load_wlf(){
	elf light_linker;
	 if(!ComboBoxWLF->currentText().isEmpty()){
 		//light_linker.linker(elfFile.toAscii().data(),NULL,wlf_name.toAscii().data(),systemOut);
     }else {
 		systemOut->clear();
 		systemOut->append("You must select a valid -.wlf- file to laod it and unload the previous");
		}
	
}



	
/*------------------------------BOOTSTRAP MENU--------------------------------*/
	
void MainWindowImpl::browseBootstrap() {  
	   bootFile = QFileDialog::getOpenFileName(this, 
	   		tr("Open ELF"), 
	   		"/home/iceman/cefriel/telosb/mantis-1.0-beta/build/telosb/src/apps",//QDir::currentPath(),
	   		tr("ELF files(*.elf)"));       
       fileBootstrap->addItem(bootFile);
       fileBootstrap->setCurrentIndex(fileBootstrap->currentIndex()+1);   
    }


void MainWindowImpl::bootstrap(){
	char*path;
	QString dir=QDir::currentPath();
	
	 if(!fileBootstrap->currentText().isEmpty()){
	 	path=(char*) malloc(dir.length()+sizeof(BOOTSTRAP)+bootFile.length()
							+sizeof(PYTHON)+sizeof(SYSTEM_LOG));
		strncpy(path,PYTHON,sizeof(PYTHON));
		strncat(path,dir.toAscii().data(),dir.length());
		strncat(path,BOOTSTRAP,sizeof(BOOTSTRAP));
		strncat(path,bootFile.toAscii().data(),bootFile.length());
		strncat(path,SYSTEM_LOG,sizeof(SYSTEM_LOG));
	 	systemOut->clear();
    	system(path);	
    	show_system_output(SYSTEM_FILE);
 		free(path);
 	}else {
 		systemOut->clear();
 		systemOut->append("To launch the Bootstrap loader, you must specify an ELF file.");
		}

	
	
	}	

void MainWindowImpl::bootstrapNoErase(){
	char*path;
	QString dir=QDir::currentPath();
	
	if(!fileBootstrap->currentText().isEmpty()){
		path=(char*) malloc(dir.length()+sizeof(BOOTSTRAP_NO_ERASE)+bootFile.length()
							+sizeof(PYTHON)+sizeof(SYSTEM_LOG));
		strncpy(path,PYTHON,sizeof(PYTHON));
		strncat(path,dir.toAscii().data(),dir.length());
		strncat(path,BOOTSTRAP,sizeof(BOOTSTRAP_NO_ERASE));
		strncat(path,bootFile.toAscii().data(),bootFile.length());
		strncat(path,SYSTEM_LOG,sizeof(SYSTEM_LOG));
		system(path);
		free(path);
		systemOut->clear();
		show_system_output(SYSTEM_FILE);
		
	}else {
 		systemOut->clear();
 		systemOut->append("To launch the non-erasing Bootstrap loader, you must specify an ELF file.");
		}
	
	}

/*---------------------------------DEVICE UTILITIES---------------------------*/
	
void  MainWindowImpl::reset(){
	char*path;
	QString dir=QDir::currentPath();
	QString sysout;	
	path=(char*) malloc(dir.length()+sizeof(BSL_RESET)+sizeof(PYTHON)+sizeof(SYSTEM_LOG));
	strncpy(path,PYTHON,sizeof(PYTHON));
	strncat(path,dir.toAscii().data(),dir.length());
	strncat(path,BSL_RESET,sizeof(BSL_RESET));
	strncat(path,SYSTEM_LOG,sizeof(SYSTEM_LOG));
	system(path);
	free(path);
	systemOut->clear();
	show_system_output(SYSTEM_FILE);
	
}

void  MainWindowImpl::erase(){
	char*path;
	QString dir=QDir::currentPath();
	path=(char*) malloc(dir.length()+sizeof(BSL_RESET)+sizeof(PYTHON)+sizeof(SYSTEM_LOG));
	strncpy(path,PYTHON,sizeof(PYTHON));
	strncat(path,dir.toAscii().data(),dir.length());
	strncat(path,BSL_ERASE,sizeof(BSL_ERASE));
	strncat(path,SYSTEM_LOG,sizeof(SYSTEM_LOG));
	system(path);
	free(path);
	systemOut->clear();
	show_system_output(SYSTEM_FILE);
}




/*---------------------------SYSTEM OUTPUT UTILITY----------------------------*/

void MainWindowImpl:: show_system_output(char*file_name){
   std::ifstream in(file_name);
   if(!in){
      std::cout << "Cannot open file.";
      exit (1);
   }
   char str[255];
   while(in){
      in.getline(str, 255);      // Delimiter defaults to newline
      systemOut->append(str);
   }
   in.close();
}

//
