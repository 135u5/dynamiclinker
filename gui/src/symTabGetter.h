#ifndef __SYMTABGETTER_H__
#define __SYMTABGETTER_H__
#include "serial.h"
#include <QtGui>

typedef unsigned char uint8_t;

void getSymTab(QTextEdit *sysout,char*symtab_name,QComboBox * symtabs);

#endif // __SYMTABGETTER_H__
