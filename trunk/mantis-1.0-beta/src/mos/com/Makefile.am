
noinst_LIBRARIES = lib_com.a
lib_com_a_SOURCES = com.c loopback.c

if ARCH_LINUX
lib_com_a_SOURCES += serial.c terminal.c udp.c xmos_radio.c
endif

if ARCH_MSP430
lib_com_a_SOURCES += uart.c
endif

if PLATFORM_TELOSB
lib_com_a_SOURCES += cc2420.c cc2420-gpio.c
endif

include ../../Makefile.lib.inc
