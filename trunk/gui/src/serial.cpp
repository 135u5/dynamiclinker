#include "serial.h"
#include "ctype.h"
typedef unsigned long int uint32_t;

void serial:: comInit(){
	fd = open(MODEMDEVICE, O_RDWR | O_NOCTTY ); 
	if (fd <0) {
		perror(MODEMDEVICE); exit(-1); 
		}else{
		fcntl(fd, F_SETFL, 0);
		}
	tcgetattr(fd,&oldtio); /* save current port settings */
   
   // Setup new serial port settings
   bzero (&newtio, sizeof (newtio));
   // manually do what cfmakeraw () does since cygwin doesn't support it
   newtio.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP |
		       INLCR | IGNCR | ICRNL | IXON);
   newtio.c_oflag &= ~OPOST;
   newtio.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
   newtio.c_cflag &= ~(CSIZE | PARENB);
   newtio.c_cflag |= CS8;
//   newtio.c_cflag |= O_NOCTTY;

   newtio.c_iflag &= ~(INPCK | IXOFF | IXON);
   newtio.c_cflag &= ~(HUPCL | CSTOPB | CRTSCTS);
   newtio.c_cflag |= (CLOCAL | CREAD);
   
   // set input mode (non-canonical, no echo,...)
   newtio.c_lflag = 0;  
   newtio.c_cc[VTIME] = 0;   // min time between chars (*.1sec)
   newtio.c_cc[VMIN] = 1;   // min number of chars for read 

   cfsetispeed (&newtio, BAUDRATE);
   cfsetospeed (&newtio, BAUDRATE);
   tcflush (fd, TCIFLUSH);
   tcsetattr (fd,TCSANOW,&newtio);

}

void serial::comFlush(){
	tcflush(fd, TCIFLUSH);
}

void serial::comClose(){
	tcsetattr(fd,TCSANOW,&oldtio);
	close(fd);
}

void serial::send_message (unsigned char* message, unsigned int size){
	pre.ACK1='S';
	pre.ACK2='S';
	pre.size=(unsigned char)size;
	res=write(fd,&pre,sizeof(preamble));
	res=write(fd,message,size);
}



unsigned char*serial:: recv_message(){
	preamble_count = 0;
	byte=0;
	while(1){
			while(byte!=PREAMBLE){
				res = read (fd, &byte, 1);
			}
			preamble_count++;
			res=read (fd, &byte, 1);
			if(byte==PREAMBLE){
				preamble_count++;
				break;
			}
			preamble_count=0;
		}
				if (preamble_count == PREAMBLE_SIZE) {
						res = 0;
						while (res == 0) {
							res = read (fd,&size, 1);
							}
		
						if (res == -1) {
							perror("error1 in reading data from device");
							return (unsigned char*)ERROR_READ;
							}
						}				
						
							memset(data,'\0',COM_DATA_SIZE);
							res = read (fd, data, size);
							if (res == -1) {
								perror("error2 in reading data from device");
								return (unsigned char*)ERROR_READ;
								}

						return data;		
				}
				


