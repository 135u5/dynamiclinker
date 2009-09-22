#ifndef __SERIAL_H__
#define __SERIAL_H__

//libraries

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <termios.h>
#include <stdio.h>

//constants

#define BAUDRATE B57600
#define COM_DATA_SIZE 64
#define MODEMDEVICE "/dev/ttyUSB0" 
#define _POSIX_SOURCE 1 /* POSIX compliant source */
#define PREAMBLE 0x53
#define PREAMBLE_SIZE 2
#define ERROR_READ "Error while reading"


//structures
typedef unsigned char uint8_t;

typedef struct{
	char ACK1;
	char ACK2;
	unsigned char size;
}preamble;



//class
class serial{

public:
void comInit();
void comClose();
void comFlush();
unsigned char* recv_message();
void send_message(unsigned char *message, unsigned int size);

private:
uint8_t byte, preamble_count, size;
int fd,res;
unsigned char data[COM_DATA_SIZE];
struct termios oldtio,newtio;
preamble pre;
};

#endif // __SERIAL_H__
