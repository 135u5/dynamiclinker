/*
This code is partially reused from some of the ContikiOS elf-loader source code
 */

#include "elf.h"
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <list>
#if 0
#define PRINTF(...) printf(__VA_ARGS__)
#else
#define PRINTF(...) do {} while (0)
#endif
#define EI_NIDENT 16

struct elf32_ehdr {
  unsigned char e_ident[EI_NIDENT];    /* ident bytes */
  elf32_half e_type;                   /* file type */
  elf32_half e_machine;                /* target machine */
  elf32_word e_version;                /* file version */
  elf32_addr e_entry;                  /* start address */
  elf32_off e_phoff;                   /* phdr file offset */
  elf32_off e_shoff;                   /* shdr file offset */
  elf32_word e_flags;                  /* file flags */
  elf32_half e_ehsize;                 /* sizeof ehdr */
  elf32_half e_phentsize;              /* sizeof phdr */
  elf32_half e_phnum;                  /* number phdrs */
  elf32_half e_shentsize;              /* sizeof shdr */
  elf32_half e_shnum;                  /* number shdrs */
  elf32_half e_shstrndx;               /* shdr string index */
};

/* Values for e_type. */
#define ET_NONE         0       /* Unknown type. */
#define ET_REL          1       /* Relocatable. */
#define ET_EXEC         2       /* Executable. */
#define ET_DYN          3       /* Shared object. */
#define ET_CORE         4       /* Core file. */

struct elf32_shdr {
  elf32_word sh_name; 		/* section name */
  elf32_word sh_type; 		/* SHT_... */
  elf32_word sh_flags; 	        /* SHF_... */
  elf32_addr sh_addr; 		/* virtual address */
  elf32_off sh_offset; 	        /* file offset */
  elf32_word sh_size; 		/* section size */
  elf32_word sh_link; 		/* misc info */
  elf32_word sh_info; 		/* misc info */
  elf32_word sh_addralign; 	/* memory alignment */
  elf32_word sh_entsize; 	/* entry size if table */
};

/* sh_type */
#define SHT_NULL        0               /* inactive */
#define SHT_PROGBITS    1               /* program defined information */
#define SHT_SYMTAB      2               /* symbol table section */   printf("\nelf file path: %s",elf_file); 
#define SHT_STRTAB      3               /* string table section */
#define SHT_RELA        4               /* relocation section with addends*/
#define SHT_HASH        5               /* symbol hash table section */
#define SHT_DYNAMIC     6               /* dynamic section */
#define SHT_NOTE        7               /* note section */
#define SHT_NOBITS      8               /* no space section */
#define SHT_REL         9               /* relation section without addends */
#define SHT_SHLIB       10              /* reserved - purpose unknown */
#define SHT_DYNSYM      11              /* dynamic symbol table section */
#define SHT_LOPROC      0x70000000      /* reserved range for processor */
#define SHT_HIPROC      0x7fffffff      /* specific section header types */
#define SHT_LOUSER      0x80000000      /* reserved range for application */
#define SHT_HIUSER      0xffffffff      /* specific indexes */
struct elf32_rel {
  elf32_addr      r_offset;       /* Location to be relocated. */
  elf32_word      r_info;         /* Relocation type and symbol index. */
};

struct elf32_sym {
  elf32_word      st_name;        /* String table index of name. */
  elf32_addr      st_value;       /* Symbol value. */
  elf32_word      st_size;        /* Size of associated object. */
  unsigned char   st_info;        /* Type and binding information. */
  unsigned char   st_other;       /* Reserved (not used). */
  elf32_half      st_shndx;       /* Section index of symbol. */
};

#define ELF32_R_SYM(info)       ((info) >> 8)
#define ELF32_R_TYPE(info)      ((unsigned char)(info))
#define RELTEXT 1
#define RELRODATA 2
#define RELDATA 3

struct relevant_section {
  unsigned char number;
  unsigned int offset;
  char *address;
};

char elfloader_unknown[30];	/* Name that caused link error. */

struct relevant_section bss, data, rodata, text;

const static unsigned char elf_magic_header[] =
  {0x7f, 0x45, 0x4c, 0x46,  /* 0x7f, 'E', 'L', 'F' */
   0x01,                    /* Only 32-bit objects. */
   0x01,                    /* Only LSB data. */
   0x01,                    /* Only ELF version 1. */
  };

char *text_sgm, *data_sgm,*rodata_sgm;


/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/


int elf::linker(char* elf_path, char* global_symTab,char* wlf_name,QTextEdit*sysout) {
 
  struct elf32_ehdr ehdr;
  struct elf32_shdr shdr;
  struct elf32_shdr strtable;
  unsigned int strs;
  unsigned int shdrptr;
  unsigned int nameptr;
  char name[12],*wlf_path;
  FILE* elf_file;
  FILE *wlf;
  struct wlf_header wlf_hdr;
  int i,ret,fret,text_flag=0,data_flag=0,rodata_flag=0;
  int relTextFlag=0,relRodataFlag=0,relDataFlag=0;
  unsigned short shdrnum, shdrsize;
  unsigned char using_relas = -1;
  unsigned short textoff = 0, textsize, textrelaoff = 0, textrelasize;
  unsigned short dataoff = 0, datasize, datarelaoff = 0, datarelasize;
  unsigned short rodataoff = 0, rodatasize, rodatarelaoff = 0, rodatarelasize;
  unsigned short symtaboff = 0, symtabsize;
  unsigned short strtaboff = 0, strtabsize;
  unsigned short bsssize = 0;


  elfloader_unknown[0] = 0;

  //open the elf file   
   elf_file=fopen(elf_path,"r");
	

  // The ELF header is located at the start of the buffer. 
    
    fret=fread((char*)&ehdr,sizeof(ehdr),1,elf_file);  
    if (fret == 0) {printf ("\nError while reading the header ELF file");
    							 return FILERROR;}

	
	
  //  print_chars(ehdr.e_ident, sizeof(elf_magic_header));
  // print_chars(elf_magic_header, sizeof(elf_magic_header));
  // Make sure that we have a correct and compatible ELF header. 
  if(memcmp(ehdr.e_ident, elf_magic_header, sizeof(elf_magic_header)) != 0) {
    PRINTF("ELF header problems\n");
    return ELFLOADER_BAD_ELF_HEADER;
  }

  
  
  // Grab the section header.
  shdrptr = ehdr.e_shoff;
  fseek(elf_file,shdrptr,SEEK_SET);
  fret=fread((char*)&shdr, sizeof(shdr),1,elf_file);
  if (fret == 0) {printf ("\nError while reading ELF file");
    							 return FILERROR;}
  
  // Get the size and number of entries of the section header. 
  shdrsize = ehdr.e_shentsize;
  shdrnum = ehdr.e_shnum;
  
  // The string table section: holds the names of the sections. 
  fseek(elf_file,ehdr.e_shoff + shdrsize * ehdr.e_shstrndx,SEEK_SET);
  fret=fread((char *)&strtable, sizeof(strtable),1,elf_file);
  if (fret == 0) {printf ("\nError while reading ELF file");
    							 return FILERROR;}
  
  // Get a pointer to the actual table of strings. This table holds
  //   the names of the sections, not the names of other symbols in the
  //   file (these are in the sybtam section).
  strs = strtable.sh_offset;
  
  // Go through all sections and pick out the relevant ones. The
  //   ".text" segment holds the actual code from the ELF file, the
  //   ".data" segment contains initialized data, the ".bss" segment
  //   holds the size of the unitialized data segment. The ".rel[a].text"
  //   and ".rel[a].data" segments contains relocation information for the
  //   contents of the ".text" and ".data" segments, respectively. The
  //   ".symtab" segment contains the symbol table for this file. The
  //   ".strtab" segment points to the actual string names used by the
  //   symbol table.
     
  //   In addition to grabbing pointers to the relevant sections, we
  //   also save the section number for resolving addresses in the
  //   relocator code.
  
  
  
  // Initialize the segment sizes to zero so that we can check if
  //   their sections was found in the file or not. 
  textsize = textrelasize = datasize = datarelasize =
    rodatasize = rodatarelasize = symtabsize = strtabsize = 0;
  
  bss.number = data.number = rodata.number = text.number = -1;
  
  shdrptr = ehdr.e_shoff;
 
 //initialize wlf header fields
 wlf_hdr.text_size=0;
 wlf_hdr.data_size=0;
 wlf_hdr.rodata_size=0;
 wlf_hdr.bss_size=0;
  
  for(i = 0; i < shdrnum; ++i) {
  	fseek(elf_file,shdrptr,SEEK_SET);
    fret=fread((char *)&shdr, sizeof(shdr),1,elf_file);
    if (fret == 0) {printf ("\nError while reading ELF file");
    							 return FILERROR;}
    
    // The name of the section is contained in the strings table. 
    nameptr = strs + shdr.sh_name;
    fseek(elf_file,nameptr,SEEK_SET);
    fret=fread(name, sizeof(name),1,elf_file);
    if (fret == 0) {printf ("\nError while reading ELF file");
    							 return FILERROR;}
    
    // Match the name of the section with a predefined set of names
    //   (.text, .data, .bss, .rela.text, .rela.data, .symtab, and
    //   .strtab). 
    // added support for .rodata, .rel.text and .rel.data). 
    
    if(strncmp(name, ".text", 5) == 0) {
      textoff = shdr.sh_offset;
      textsize = shdr.sh_size;
      text.number = i;
      text.offset = textoff;
      fseek(elf_file,textoff,SEEK_SET);
      wlf_hdr.text_size=textsize;
      text_sgm=(char*)malloc(textsize);
      fret=fread(text_sgm,textsize,1,elf_file);
      text_flag=1;
      if (fret == 0) {printf ("\nError while reading ELF file");
    							 return FILERROR;}
    } else if(strncmp(name, ".rel.text", 9) == 0) {
      using_relas = 0;
      textrelaoff = shdr.sh_offset;
      textrelasize = shdr.sh_size;
      relTextFlag=1;
    } else if(strncmp(name, ".rela.text", 10) == 0) {
      using_relas = 1;
      textrelaoff = shdr.sh_offset;
      textrelasize = shdr.sh_size;
      relTextFlag=1;
    } else if(strncmp(name, ".data", 5) == 0) {
      dataoff = shdr.sh_offset;
      datasize = shdr.sh_size;
      data.number = i;
      data.offset = dataoff;
      wlf_hdr.data_size=datasize;
	  fseek(elf_file,dataoff,SEEK_SET);
	  data_sgm=(char*)malloc(datasize);
      fret=fread(data_sgm,datasize,1,elf_file);
      data_flag=1;
 	  if (fret == 0) {printf ("\nError while reading ELF file");
    							 return FILERROR;}   
    } else if(strncmp(name, ".rodata", 7) == 0) {
      // read-only data handled the same way as regular text section 
      rodataoff = shdr.sh_offset;
      rodatasize = shdr.sh_size;
      rodata.number = i;
      rodata.offset = rodataoff;
      wlf_hdr.rodata_size=rodatasize;
      rodata_sgm=(char*)malloc(rodatasize);
	  fseek(elf_file,rodataoff,SEEK_SET);
      fret=fread(rodata_sgm,rodatasize,1,elf_file);
      rodata_flag=1;
      if (fret == 0) {printf ("\nError while reading ELF file");
    							 return FILERROR;}
    } else if(strncmp(name, ".rel.rodata", 11) == 0) {
      // using elf32_rel instead of rela 
      using_relas = 0;
      rodatarelaoff = shdr.sh_offset;
      rodatarelasize = shdr.sh_size;
      relRodataFlag=1;
    } else if(strncmp(name, ".rela.rodata", 12) == 0) {
      using_relas = 1;
      rodatarelaoff = shdr.sh_offset;
      rodatarelasize = shdr.sh_size;
      relRodataFlag=1;
    } else if(strncmp(name, ".rel.data", 9) == 0) {
      // using elf32_rel instead of rela 
      using_relas = 0;
      datarelaoff = shdr.sh_offset;
      datarelasize = shdr.sh_size;
      relDataFlag=1;
    } else if(strncmp(name, ".rela.data", 10) == 0) {
      using_relas = 1;
      datarelaoff = shdr.sh_offset;
      datarelasize = shdr.sh_size;
      relDataFlag=1;
    } else if(strncmp(name, ".symtab", 7) == 0) {
      symtaboff = shdr.sh_offset;
      symtabsize = shdr.sh_size;
    } else if(strncmp(name, ".strtab", 7) == 0) {
      strtaboff = shdr.sh_offset;
      strtabsize = shdr.sh_size;
    } else if(strncmp(name, ".bss", 4) == 0) {
      bsssize = shdr.sh_size;
      bss.number = i;
      bss.offset = 0;
      wlf_hdr.bss_size=bsssize;
    }
    
    // Move on to the next section header. 
    shdrptr += shdrsize;
  }
  
  
  if(symtabsize == 0) {
    return ELFLOADER_NO_SYMTAB;
  }
  if(strtabsize == 0) {
    return ELFLOADER_NO_STRTAB;
  }
  if(textsize == 0) {
    return ELFLOADER_NO_TEXT;
  }
 
  fclose(elf_file);
  
  //if we're in light linking mode or the ELF does not present any .rel section write wlf and return 
  if(global_symTab==NULL or (!relTextFlag && !relRodataFlag && !relDataFlag)){
   
  	 wlf=fopen(wlf_name,"w");
  	 wlf_hdr.wlf_id[0]='W';
  	 wlf_hdr.wlf_id[1]='L';
  	 wlf_hdr.wlf_id[2]='F';
  	 fwrite(&wlf_hdr,sizeof(struct wlf_header),1,wlf);
  	 if(text_flag)fwrite(text_sgm,textsize,1,wlf);
  	 if(rodata_flag)fwrite(rodata_sgm,rodatasize,1,wlf);
  	 if(data_flag)fwrite(data_sgm,textsize,1,wlf);
     fclose(wlf);
     sysout->clear();
     QString msg_ligh =QDir::currentPath();
     msg_ligh.append("/");
     msg_ligh.append(wlf_name);
     sysout->append("\nWLF SUCCESSFULLY CREATED!\n The file path is:");
     sysout->append(msg_ligh);
  	 return ELFLOADER_OK;

 	}
  
  // If we have text segment relocations, we process them. 
  PRINTF("elfloader: relocate text\n");
  if(textrelasize > 0) {
    ret = relocate_section(textrelaoff, textrelasize,
			   textoff,
			   strs,
			   strtaboff,
			   symtaboff, symtabsize, 
			   using_relas,elf_path, global_symTab,RELTEXT);
    if(ret != ELFLOADER_OK) {
      return ret;
    }
  }
  
  // If we have any rodata segment relocations, we process them too. 
  PRINTF("elfloader: relocate rodata\n");
  if(rodatarelasize > 0) {
    ret = relocate_section(rodatarelaoff, rodatarelasize,
			   rodataoff,
			   strs,
			   strtaboff,
			   symtaboff, symtabsize, 
			   using_relas,elf_path,global_symTab,RELRODATA);
    if(ret != ELFLOADER_OK) {
      PRINTF("elfloader: data failed\n");
      return ret;
    }
  }
  
  // If we have any data segment relocations, we process them too. 
  PRINTF("elfloader: relocate data\n");
  if(datarelasize > 0) {
    ret = relocate_section(datarelaoff, datarelasize,
			   dataoff,
			   strs,
			   strtaboff,
			   symtaboff, symtabsize,
			   using_relas,elf_path,global_symTab,RELDATA);
    if(ret != ELFLOADER_OK) {
      PRINTF("elfloader: data failed\n");
      return ret;
    }
  }

	//WRITE WLF
	
   wlf=fopen(wlf_name,"w");
   wlf_hdr.wlf_id[0]='W';
   wlf_hdr.wlf_id[1]='L';
   wlf_hdr.wlf_id[2]='F';
   fwrite(&wlf_hdr,sizeof(struct wlf_header),1,wlf);
   if(text_flag)fwrite(text_sgm,textsize,1,wlf);
   if(rodata_flag)fwrite(rodata_sgm,rodatasize,1,wlf);
   if(data_flag)fwrite(data_sgm,textsize,1,wlf);
   QString msg =QDir::currentPath();
   msg.append("/");
   msg.append(wlf_name);
   sysout->append("\nWLF SUCCESSFULLY CREATED!\n The file path is:");
   sysout->append(msg);
   fclose(wlf);


  return ELFLOADER_OK;
}





/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/


/*----------------------------------------------------------------------------*/



int elf::relocate_section(unsigned int section, unsigned short size,
		 unsigned int sectionaddr,
		 unsigned int strs,
		 unsigned int strtab,
		 unsigned int symtab, unsigned short symtabsize,
		 unsigned char using_relas,char* elf_path, char *globSymTab,int type)
{
  // sectionbase added; runtime start address of current section 
  struct elf32_rela rela; // Now used both for rel and rela data! 
  int rel_size = 0,fret;
  struct elf32_sym s;
  unsigned int a;
  char name[30];
  char *addr;
  struct relevant_section *sect;
  FILE*elf_file;
  
  //open the elf file in reading mode
  elf_file=fopen(elf_path,"r");
  
  // determine correct relocation entry sizes 
  if(using_relas) {
    rel_size = sizeof(struct elf32_rela);
  } else {
    rel_size = sizeof(struct elf32_rel);
  }
  
  for(a = section; a < section + size; a += rel_size) {
	
  	fseek(elf_file,a,SEEK_SET);
    fret=fread((char *)&rela,rel_size,1,elf_file);
	if (fret == 0) {printf ("\nError while reading ELF file");
    							 return FILERROR;}
    						 
	fseek(elf_file,symtab + sizeof(struct elf32_sym) * ELF32_R_SYM(rela.r_info),
			SEEK_SET);    
    fret=fread((char *)&s,sizeof(s),1,elf_file);
    if (fret == 0) {printf ("\nError while reading ELF file");
    							 return FILERROR;}
    							 
    if(s.st_name != 0) {
      fseek(elf_file,strtab + s.st_name,SEEK_SET);
      fret=fread(name, sizeof(name),1,elf_file);
      if (fret == 0) {printf ("\nError while reading ELF file");
    							 return FILERROR;}
      PRINTF("name: %s\n", name);
      addr = (char *)symtab_lookup(name,globSymTab);
      
      if(addr == NULL) {
	PRINTF("name not found in global: %s\n", name);
	addr = (char*)find_local_symbol(name, symtab, symtabsize, strtab,elf_file);
	PRINTF("found address %p\n", addr);
      }

      if(addr == NULL) {
	if(s.st_shndx == bss.number) {
	  sect = &bss;
	} else if(s.st_shndx == data.number) {
	  sect = &data;
	} else if(s.st_shndx == rodata.number) {
	  sect = &rodata;
	} else if(s.st_shndx == text.number) {
	  sect = &text;
	} else {
	  PRINTF("elfloader unknown name: '%30s'\n", name);
	  memcpy(elfloader_unknown, name, sizeof(elfloader_unknown));
	  elfloader_unknown[sizeof(elfloader_unknown) - 1] = 0;
	  return ELFLOADER_SYMBOL_NOT_FOUND;
	}
	addr = sect->address;
      }
    } else {
      if(s.st_shndx == bss.number) {
	sect = &bss;
      } else if(s.st_shndx == data.number) {
	sect = &data;
      } else if(s.st_shndx == rodata.number) {
	sect = &rodata;
      } else if(s.st_shndx == text.number) {
	sect = &text;
      } else {
	return ELFLOADER_SEGMENT_NOT_FOUND;
      }
      
      addr = sect->address;
    }
    
    if (!using_relas) {
       //convert rel to rela by setting offset 
      
      fseek(elf_file,sectionaddr + rela.r_offset,SEEK_SET);
      fret=fread(&rela.r_addend, 1,4,elf_file);
      if (fret == 0) {printf ("\nError while reading ELF file");
    							 return FILERROR;}
    }
    if(type==RELTEXT)
    	apply_to_sgm(RELTEXT,&rela, addr);
    else if(type==RELRODATA)
    	apply_to_sgm(RELRODATA,&rela,addr);
    else if(type==RELDATA)
    	apply_to_sgm(RELDATA,&rela,addr);
    			
  }//end for
  
  fclose(elf_file);
  
  return ELFLOADER_OK;
  
  
}



/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/

void * elf::find_local_symbol(const char *symbol,
		  unsigned int symtab, unsigned short symtabsize,
		  unsigned int strtab,FILE*elf_file)
{
  struct elf32_sym s;
  unsigned int a;
  char name[30];
  int fret;
  struct relevant_section *sect;
  
  
  for(a = symtab; a < symtab + symtabsize; a += sizeof(s)) {
    fseek(elf_file,a,SEEK_SET);
    fret=fread((char *)&s, sizeof(s),1,elf_file);
    if (fret == 0) {printf ("\nError while reading ELF file");
    							 exit(8);}
    if(s.st_name != 0) {
      fseek(elf_file,strtab+s.st_name,SEEK_SET);
	  fret=fread(name,sizeof(name),1,elf_file);
      if (fret == 0) {printf ("\nError while reading ELF file");
    							 exit(8);}
      if(strcmp(name, symbol) == 0) {
	if(s.st_shndx == bss.number) {
	  sect = &bss;
	} else if(s.st_shndx == data.number) {
	  sect = &data;
	} else if(s.st_shndx == text.number) {
	  sect = &text;
	} else {
	  return NULL;
	}
	return &(sect->address[s.st_value]);
      }
    }
  }
  
  
  return NULL;
}

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/


const char * elf::symtab_lookup(const char *name, char* globSymTab)
{	  std::ifstream symTab;
	  std::list <struct symbol> symbols;
	  std::list<struct symbol>::iterator i;
	  struct symbol temp;
	  symTab.open(globSymTab);
  
	while(symTab.getline(temp.name,COM_DATA_SIZE,'\0')){
	
		symTab.getline(temp.value,COM_DATA_SIZE,'\0');	
		symbols.push_back(temp);
	}
	
	printf("\nSymbol lookup:\n");
	
	for(i=symbols.begin();i!=symbols.end();i++){
		if(strcmp(name, i->name) == 0) {
      		return (char*)atoi(i->value);}
	}
	
	
}


/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/


void elf::apply_to_sgm(int type, elf32_rela* rela, char* addr){

	addr += rela->r_addend;
if(type==RELTEXT){	
    memcpy(&text_sgm[rela->r_offset],&addr, 2);
}else if(type==RELRODATA){
	memcpy(&rodata_sgm[rela->r_offset],&addr, 2);
}else if(type==RELDATA){
	memcpy(&data_sgm[rela->r_offset],&addr, 2);
}

}
