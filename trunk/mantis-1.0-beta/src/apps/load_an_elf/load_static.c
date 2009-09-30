#include <inttypes.h>
#include <string.h>
#include "mos.h"
#include "msched.h"
#include "com.h"
#include "led.h"  
#include "node_id.h"
#include "clock.h"
#include "telos-flash.h"
#include "dev.h"
#include "loader/elfloader.h"
#include "elfstore/elfstore.h"
#include "dev/flash.h"
#include "symbols.h"
#define NAME_REQ 1
#define VAL_REQ 2
#define BUFF_LENGHT 16
#define READSIZE 32

static void convert_and_send(uint16_t value,uint8_t radix,comBuf *send);
static char datamemory[ELFLOADER_DATAMEMORY_SIZE];
static const char textmemory[ELFLOADER_TEXTMEMORY_SIZE] = {0};
uint8_t msg_wlf[4]="wlf";//receive a WLF file to load
uint8_t msg_dim[4]="sym";//get symbol table
uint8_t msg_ack[4]="ack";
uint8_t msg_textsgm[4]="txt";
uint8_t msg_rodatasgm[4]="rod";
uint8_t msg_datasgm[4]="dat";
uint8_t msg_end[4]="end";

static unsigned short ie1, ie2;
comBuf *ack, *read_pk, send_pk;

void blink_led(void)
{
	uint32_t sleep_time;
	sleep_time = 250;
	while(1)
	{
		mos_thread_sleep(sleep_time);	
		mos_led_toggle(1);
	}
}

///////////////  START  ////////////////////////////////////////// 



static unsigned short ie1, ie2;
void start(void) {

	uint8_t flag=0,i=0,total,max_pkt;
	const struct symbols *s;	
	com_mode(IFACE_SERIAL2,IF_LISTEN);
	unsigned int textsize,rodatasize,datasize,bsssize,j;
	char* bssAddress, *dataAddress,*textAddress,*rodataAddress,end[COM_DATA_SIZE],prova[COM_DATA_SIZE];
	uint32_t temp;

	while(1){
		//waiting for req

		ack=com_recv(IFACE_SERIAL2);
		com_free_buf(ack);

		//if the operation required is to send the symbol table to the host, execute this first branch of the "if" 
		if(ack->data[0]== msg_dim[0] && ack->data[1]== msg_dim[1] && ack->data[2]== msg_dim[2]){


			//send the symbol table to the host
			for(s = symbols;  s->name != (const void *)0; s++) {  
				
				//set packet
				send_pk.size=strlen(s->name);
				memset((char*)send_pk.data,'\0',COM_DATA_SIZE);
				memcpy(send_pk.data,s->name,strlen(s->name));
				//send symbol value	
				com_send(IFACE_SERIAL2,&send_pk);
				convert_and_send((uint16_t)s->value,10,&send_pk);


			}

			memset(send_pk.data,'\0',COM_DATA_SIZE);
			//sending symbol name	
			com_send(IFACE_SERIAL2,&send_pk);
			//mos_led_blink(2);	
		}

		/* Receive and load a WLF file */
		else if(ack->data[0] == msg_wlf[0] && ack->data[1] == msg_wlf[1] && 
			ack->data[2]== msg_wlf[2])
		{
			/* Complete the wlf protocol handshake */
			send_pk.size=BUFF_LENGHT;
        	memset((char *)send_pk.data, '\0', COM_DATA_SIZE);
        	memcpy(send_pk.data, msg_wlf, sizeof(msg_wlf));				
			com_send(IFACE_SERIAL2, &send_pk);
			com_free_buf(&send_pk);

			/* Receiving text size */
			ack=com_recv(IFACE_SERIAL2);
			com_free_buf(ack);
			textsize = atoi(ack->data);
			send_pk.size=BUFF_LENGHT;
    		memset((char*)send_pk.data,'\0',COM_DATA_SIZE);
    		memcpy(send_pk.data,msg_ack,sizeof(msg_ack));
			com_send(IFACE_SERIAL2,&send_pk);
 			com_free_buf(&send_pk);
				
			//receiving rodata size
			ack=com_recv(IFACE_SERIAL2);
			com_free_buf(ack);
			rodatasize=atoi(ack->data);
			//mos_led_blink(0);
				//complete the rodata size handshake		
				send_pk.size=BUFF_LENGHT;
       				memset((char*)send_pk.data,'\0',COM_DATA_SIZE);
       				memcpy(send_pk.data,msg_ack,sizeof(msg_ack));	
				com_send(IFACE_SERIAL2,&send_pk);
				com_free_buf(&send_pk);

			//receiving data size
			ack=com_recv(IFACE_SERIAL2);
			com_free_buf(ack);
			datasize=atoi(ack->data);
			//mos_led_blink(0);
				//complete the data size handshake			
				send_pk.size=BUFF_LENGHT;
       				memset((char*)send_pk.data,'\0',COM_DATA_SIZE);
       				memcpy(send_pk.data,msg_ack,sizeof(msg_ack));				
				com_send(IFACE_SERIAL2,&send_pk);
				com_free_buf(&send_pk);
				
			//receiving bss size
			ack=com_recv(IFACE_SERIAL2);
			com_free_buf(ack);
			bsssize=atoi(ack->data);
			//mos_led_blink(0);
				//complete the bss size handshake
				send_pk.size=BUFF_LENGHT;
       				memset((char*)send_pk.data,'\0',COM_DATA_SIZE);
       				memcpy(send_pk.data,msg_ack,sizeof(msg_ack));				
				com_send(IFACE_SERIAL2,&send_pk);
				com_free_buf(&send_pk);
				

			mos_thread_new(blink_led, 128, PRIORITY_NORMAL);

			//allocate space for segments and get the relative starting points
			 bssAddress = (char *)elfloader_arch_allocate_ram(bsssize + datasize);
  			 dataAddress = (char *)bssAddress + bsssize;
  			 textAddress = (char *)elfloader_arch_allocate_rom(textsize + rodatasize);
  			 rodataAddress = (char *)textAddress + textsize;
				
			
			//recv segments and allocate memory	
			ack=com_recv(IFACE_SERIAL2);
			
			//recv text segment
			if(ack->data[0]== msg_textsgm[0] && ack->data[1]== msg_textsgm[1] && ack->data[2]== msg_textsgm[2]){

				
				//send ack
				send_pk.size=BUFF_LENGHT;
       				memset((char*)send_pk.data,'\0',COM_DATA_SIZE);
       				memcpy(send_pk.data,msg_textsgm,sizeof(msg_textsgm));			
				com_send(IFACE_SERIAL2,&send_pk);
				com_free_buf(&send_pk);
				

				//flash ops 
  				dev_open(DEV_TELOS_FLASH);
 
				//set termination message
				memset(end,'Z',COM_DATA_SIZE);
				
				//recv first message
				ack=com_recv(IFACE_SERIAL2);
				com_free_buf(ack);

				while(memcmp(ack->data,end,COM_DATA_SIZE)){
					
					dev_write(DEV_TELOS_FLASH,ack->data,COM_DATA_SIZE);
					ack=com_recv(IFACE_SERIAL2);
					com_free_buf(ack);							
									
				}

			
				dev_read(DEV_TELOS_FLASH, prova,sizeof(prova));
				printf("%s",prova);
				
				dev_close(DEV_TELOS_FLASH);


			}

		
					/*
					ack=com_recv(IFACE_SERIAL2);
					com_free_buf(ack);
					if(ack->data[0]== msg_rodatasgm[0] && ack->data[1]== msg_rodatasgm[1] && ack->data[2]== msg_rodatasgm[2])
						receive_segm_and_write_rom(rodatasize, rodataAddress,ack);
					//mos_led_blink(1);


					
					ack=com_recv(IFACE_SERIAL2);
					com_free_buf(ack);
					if(ack->data[0]== msg_datasgm[0] && ack->data[1]== msg_datasgm[1] && ack->data[2]== msg_datasgm[2])
						receive_segment_and_allocate_RAM(datasize, dataAddress,ack);
					//mos_led_blink(2);
				
							*/							

	
				//allocate memory for bss
			 memset(bssAddress, 0, bsssize);
							

		}
		else
		{
			printf("\nERROR: command not recognized!\n");
		}
  			
		com_free_buf(ack);
	}//end while

	//mos_led_off(1);
	//mos_led_on(0);
	//mos_led_on(2);
}//end start



////////////////////////  CONVERSION UTILITY  //////////////////////////////////////////


/*----------------------function to convert integer/hex/oct to ascii---------*/

static void convert_and_send (uint16_t value, uint8_t radix, comBuf *send){

	uint8_t remainder,buffsize,i;
	static uint8_t charbuff[BUFF_LENGHT];
	static uint8_t temp[BUFF_LENGHT];

	buffsize=0; 
   	remainder = value % radix;
	memset(temp,'\0',BUFF_LENGHT);	
	memset(charbuff,'\0',BUFF_LENGHT);

   	if(value == 0)  
      		temp[buffsize++] = '0';
   
   	while(value >= radix) {
      		if(remainder <= 9)
	 		temp[buffsize++] = remainder + '0';
      		else
	 		temp[buffsize++] = remainder - 10 + 'A';

      		value /= radix;
      		remainder = value % radix;
   	}
   

   //last place
	if(remainder > 0) {
      		if(remainder <= 9)
	 		temp[buffsize++] = remainder + '0';
      		else
	 		temp[buffsize++] = remainder - 10 + 'A';
   	}


   //invert the buffer

	for(i=0;i<buffsize;i++)
		charbuff[i]=temp[buffsize-1-i];



   //packaging

	send->size=BUFF_LENGHT;
        memset((char*)send->data,'\0',COM_DATA_SIZE);
        memcpy(send->data,charbuff,BUFF_LENGHT);
        com_send(IFACE_SERIAL2,send);
	com_free_buf(send);
}

////////////////////////////////MEMORY ALLOCATION UTILITIES///////////////////////

/*----------------------function to allocate ROM to readonly segments---------------------------*/

inline void
receive_segm_and_write_rom( comBuf *recv_data)
{
 
  dev_open(DEV_TELOS_FLASH);
     
 //write the first 32Byte-bulk to rom
 dev_write(DEV_TELOS_FLASH,recv_data->data,COM_DATA_SIZE);
 dev_close(DEV_TELOS_FLASH);	
// mos_led_blink(1);
}


/*------------------function to allocate RAM to rd/wr segments---------------*/


void receive_segment_and_allocate_RAM(unsigned int size, char *mem){
	int remaining=size;
	char*currentAddr;
	currentAddr=mem;

	while(remaining>0){
		read_pk=com_recv(IFACE_SERIAL2);
		com_free_buf(read_pk);
		memcpy(currentAddr,read_pk->data,COM_DATA_SIZE);
		currentAddr+=COM_DATA_SIZE;
	remaining-=COM_DATA_SIZE;
	}

}



