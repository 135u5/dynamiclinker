/*This class holds methods to load and unload a .WLF file into the node*/


#include "elf.h"
#include "serial.h"
#include "wlf_loader.h"
#define BSL_RESET "bsl.py --telosb -c /dev/ttyUSB0 -r"

uint8_t msg_wlf[4]="wlf";
uint8_t msg_textsgm[4]="txt";
uint8_t msg_rodatasgm[4]="rod";
uint8_t msg_datasgm[4]="dat";
uint8_t msg_end[4]="end";


void wlfLoader:: load(char*wlf_path){	
	serial com;
	int ret;
	char *sgm, *temp;
	int total;
	uint8_t *recv;
	FILE *wlf;
	wlf_header wlf_hdr;
	unsigned char numbuf[COM_DATA_SIZE];
	
		//open wlf file	
		wlf=fopen(wlf_path,"r");
	
		//reset device	
		system(BSL_RESET);
			
		
		//initialize the com connection		
		com.comInit();
		
	
		//request to send a WLF file
		com.send_message(msg_wlf,sizeof(msg_wlf));
		com.comFlush();
		
		
		
		//complete the wlf protocol handshake
		recv=com.recv_message();
		printf("\nHo ricevuto %s",recv);
		
		//get wlf header
		ret=fread(&wlf_hdr,sizeof(wlf_header),1,wlf);
		
		//send wlf text size to the node
		memset(numbuf,'\0',COM_DATA_SIZE);
		sprintf((char*)numbuf,"%u",wlf_hdr.text_size);
		com.send_message(numbuf,COM_DATA_SIZE);
			//complete the textsize handshake
			recv=com.recv_message();
			printf("\nHo ricevuto %s",recv);
		
		//send wlf rodata size to the node
		memset(numbuf,'\0',COM_DATA_SIZE);
		sprintf((char*)numbuf,"%u",wlf_hdr.rodata_size);
		com.send_message(numbuf,COM_DATA_SIZE);
			//complete the rodatasize handshake
			recv=com.recv_message();
			printf("\nHo ricevuto %s",recv);
		
		//send wlf data size to the node
		memset(numbuf,'\0',COM_DATA_SIZE);
		sprintf((char*)numbuf,"%u",wlf_hdr.data_size);
		com.send_message(numbuf,COM_DATA_SIZE);
			//complete the datasize handshake
			recv=com.recv_message();
			printf("\nHo ricevuto %s",recv);
		
		//send wlf bss size to the node
		memset(numbuf,'\0',COM_DATA_SIZE);
		sprintf((char*)numbuf,"%u",wlf_hdr.bss_size);
		com.send_message(numbuf,COM_DATA_SIZE);
			//complete the bsssize handshake
			recv=com.recv_message();
			printf("\nHo ricevuto %s",recv);

		
			
		//send text segment
		total=wlf_hdr.text_size;
		sgm = (char *)malloc(wlf_hdr.text_size);
		fseek(wlf,sizeof(wlf_header),SEEK_SET);
		fread(sgm, wlf_hdr.text_size, 1, wlf);
		com.send_message(msg_textsgm, sizeof(msg_textsgm));
		//receive ack
		recv=com.recv_message();
		printf("\nHo ricevuto %s",recv);
			
		/* Need to copy pointer */
		temp = sgm;
		while(total >= 64)
		{
			//com.send_message((unsigned char*)sgm, COM_DATA_SIZE);
			com.send_message((unsigned char*)temp, COM_DATA_SIZE);
			//sgm += COM_DATA_SIZE;
			temp += COM_DATA_SIZE;
			total -= COM_DATA_SIZE;
		}
		if(total < 0)
		{
			printf("\nNo bytes left");			
		}
		else
		{
			/* total should be < 64 */
			printf("\n%d bytes left", total);
		}
		fflush(stdout);
		
		free(sgm);		
		
		printf("\ntext send");
	
	/*	
		//send rodata segment
		if(wlf_hdr.rodata_size){		
			total=wlf_hdr.rodata_size;
			sgm=(unsigned char*)malloc(wlf_hdr.rodata_size);
			fseek(wlf,sizeof(wlf_header)+wlf_hdr.text_size,SEEK_SET);
			fread(sgm,wlf_hdr.rodata_size,1,wlf);
			com.send_message(msg_rodatasgm,sizeof(msg_rodatasgm));
			while(total>0){
				com.send_message(sgm,COM_DATA_SIZE);
				sgm+=COM_DATA_SIZE;
				total-=COM_DATA_SIZE;
			}
			free(sgm);
			printf("\nRodata send");
		}
	
		//send data segment
		total=wlf_hdr.data_size;
		sgm=(unsigned char*)malloc(wlf_hdr.data_size);
		fseek(wlf,sizeof(wlf_header)+wlf_hdr.text_size+wlf_hdr.rodata_size,SEEK_SET);
		fread(sgm,wlf_hdr.data_size,1,wlf);
		com.send_message(msg_datasgm,sizeof(msg_datasgm));
		while(total>0){
			com.send_message(sgm,COM_DATA_SIZE);
			sgm+=COM_DATA_SIZE;
			total-=COM_DATA_SIZE;
		}
		free(sgm);
		printf("\ndata send");*/

		//close the connection
		com.comClose();

		fclose(wlf);
		//system(BSL_RESET);

		printf("\nConnection closed");
		fflush(stdout);		
}//end_load



void wlfLoader::unloadThenLoad(){
	
}
