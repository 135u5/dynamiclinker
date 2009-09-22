#ifndef ELF_H
#define ELF_H
#include <stdio.h>
#include "symTabGetter.h"
/*
This code is partially reused from some of the ContikiOS elf-loader source code
 */

/**
 * Return value from elfloader_load() indicating that loading worked.
 */
#define ELFLOADER_OK                  0
/**
 * Return value from elfloader_load() indicating that the ELF file had
 * a bad header.
 */
#define ELFLOADER_BAD_ELF_HEADER      1
/**
 * Return value from elfloader_load() indicating that no symbol table
 * could be find in the ELF file.
 */
#define ELFLOADER_NO_SYMTAB           2
/**
 * Return value from elfloader_load() indicating that no string table
 * could be find in the ELF file.
 */
#define ELFLOADER_NO_STRTAB           3
/**
 * Return value from elfloader_load() indicating that the size of the
 * .text segment was zero.
 */
#define ELFLOADER_NO_TEXT             4
/**
 * Return value from elfloader_load() indicating that a symbol
 * specific symbol could not be found.
 *
 * If this value is returned from elfloader_load(), the symbol has
 * been copied into the elfloader_unknown[] array.
 */
#define ELFLOADER_SYMBOL_NOT_FOUND    5
/**
 * Return value from elfloader_load() indicating that one of the
 * required segments (.data, .bss, or .text) could not be found.
 */
#define ELFLOADER_SEGMENT_NOT_FOUND   6
/**
 * Return value from elfloader_load() indicating that no starting
 * point could be found in the loaded module.
 */
#define ELFLOADER_NO_STARTPOINT       7
#define ELFSTORE_SIZE 0x800
#define FILERROR 8
#define WLF_FILE_EXSTENSION ".wlf"
#ifdef ELFLOADER_CONF_DATAMEMORY_SIZE
#define ELFLOADER_DATAMEMORY_SIZE ELFLOADER_CONF_DATAMEMORY_SIZE
#else
#define ELFLOADER_DATAMEMORY_SIZE 0x100
#endif

#ifdef ELFLOADER_CONF_TEXTMEMORY_SIZE
#define ELFLOADER_TEXTMEMORY_SIZE ELFLOADER_CONF_TEXTMEMORY_SIZE
#else
#define ELFLOADER_TEXTMEMORY_SIZE 0x400
#endif
#define COM_DATA_SIZE 64


typedef unsigned long  elf32_word;
typedef   signed long  elf32_sword;
typedef unsigned short elf32_half;
typedef unsigned long  elf32_off;
typedef unsigned long  elf32_addr;

/*----------------------WLF HEADER----------------------------------*/

	typedef unsigned int wlf_off;
	typedef unsigned int wlf_size;

struct wlf_header{
	char wlf_id[3];
	wlf_size text_size;//size of text section
	wlf_size rodata_size;//size of rodata section
	wlf_size data_size;//size of data section 
	wlf_size bss_size; //size of memory to be allocated for bss in the node
};

/*-------------------------------------------------------------------*/

struct symbol {
  char name[COM_DATA_SIZE];
  char value[COM_DATA_SIZE];
};


struct elf32_rela {
  elf32_addr      r_offset;       /* Location to be relocated. */
  elf32_word      r_info;         /* Relocation type and symbol index. */
  elf32_sword     r_addend;       /* Addend. */
};

/* If defined, these two functions map into the loaded ELF object. */

extern char elfloader_unknown[30];


class elf{

public:
	int linker(char* file_path,char* globalSymtab,char*wlf_name,QTextEdit *sysout);
	 
private:	

int relocate_section(unsigned int section, unsigned short size,
		 unsigned int sectionaddr,
		 unsigned int strs,
		 unsigned int strtab,
		 unsigned int symtab, unsigned short symtabsize,
		 unsigned char using_relas,char *elf, char* globSymTab,int type);

void * find_local_symbol(const char *symbol,
		  unsigned int symtab, unsigned short symtabsize,
		  unsigned int strtab,FILE*elf);
		  
const char *symtab_lookup(const char *name, char* golb_symTab);

void apply_to_sgm(int type,elf32_rela *rela,char*addr); 
};

#endif // __ELF__