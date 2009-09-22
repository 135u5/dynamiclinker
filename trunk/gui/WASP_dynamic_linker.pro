TEMPLATE = app
QT = gui core
CONFIG += qt warn_on console release
DESTDIR = bin
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
FORMS = ui/mainwindow.ui
HEADERS = src/mainwindowimpl.h \
 src/serial.h \
 src/symTabGetter.h \
 src/semaphore.h \
 src/sharedMemory.h \
 src/elf.h \
 src/wlf_loader.h
SOURCES = src/mainwindowimpl.cpp \
 src/main.cpp \
 src/serial.cpp \
 src/symTabGetter.cpp \
 src/semaphore.cpp \
 src/sharedMemory.cpp \
 src/elf.cpp \
 src/wlf_loader.cpp
