#include "symTabGetter.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "semaphore.h"
#include "sharedMemory.h"
#include "mainwindowimpl.h"
#define BSL_RESET "bsl.py --telosb -c /dev/ttyUSB0 -r"
#define BSL_ERASE "bsl.py --telosb -c /dev/ttyUSB0 -e"



void getSymTab ( QTextEdit *sysout,char*symtab_name,QComboBox*symtabs){
	uint8_t msg_sym[4]="sym";	
	serial com;
	uint8_t *recv;
	char end[64];
	FILE *symtab;
	
		//reset device	
		system(BSL_RESET);

		//initialize the com connection		
		com.comInit();
		
		//open the file to store symbol table
		symtab=fopen(symtab_name,"w");
		
		//request and receive the symbol table dimension
		com.send_message(msg_sym,sizeof(msg_sym));
		com.comFlush();

			
		
		memset(end,'\0',COM_DATA_SIZE);
		while(memcmp(recv,end,COM_DATA_SIZE)){
			recv=com.recv_message();
			printf("\n ho ricevuto: %s",recv);
			fwrite(recv,COM_DATA_SIZE,1,symtab);
		}

	
		fclose(symtab);
		com.comClose();	
	
		//reset the node
		system(BSL_RESET);
		
		//update the GUI
		sysout->append("\n SYMBOL TABLE DOWNLOADED!\n The relative file has been created with path:");
		QString symtab_path =QDir::currentPath();
		symtab_path.append("/");
		symtab_path.append(symtab_name);
		sysout->append(symtab_path);
		symtabs->addItem(symtab_path);
        symtabs->setCurrentIndex(symtabs->currentIndex()+1);

	
}

