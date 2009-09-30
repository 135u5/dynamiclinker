DEFS+=-DUSING_ELF

if ARCH_LINUX # Linux rules

BUILT_APPS = load_static.elf 
load_static_elf_SOURCES = symbols.c load_static.c loader/elfloader.c loader/elfloader-x86.c loader/symtab.c elfstore/elfstore-ram.c

.PHONY: symbols.c 
symbols.c:
	@echo Generating symbols....
	generate_symbols $(CORE)

endif # End of Linux rules


if ARCH_MSP430 # Telos rules

BUILT_APPS = load_static.elf 
load_static_elf_SOURCES = symbols.c load_static.c loader/elfloader.c loader/elfloader-msp430.c loader/symtab.c elfstore/elfstore-ram.c dev/flash.c

.PHONY: symbols.c 
symbols.c:
	@echo Generating symbols....
	generate_symbols $(CORE)
endif # End of Telos rules

## Remaking com system with USING_ELF defined
remake_com.c:
	(cd ../../../src/mos/com && make clean && make DEFS=-DUSING_ELF)
	touch remake_com.c

include ../../Makefile.am.inc
