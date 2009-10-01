/**
 * This is a simple application template that can be used to test the correctness of the
 * on-board FLASH memory on the TelosB module.
 * Several tests can be performed by simply modifying a little bit the present file.
 *
 * To be able to see the printf results on the screen you should type the following
 * once your TelosB module is programmed, otherwise programming will likely not to 
 * occur:
 *
 *     # mos_shell --sdev /dev/ttyUSB0 -n
 *
 * @author Simone Corbetta <simone.corbetta@gmail.com>
 */

/******** Includes ********/
#include <inttypes.h>

#include "mos.h"
#include "msched.h"	
#include "led.h"
#include "clock.h"
#include "telos-flash.h"
#include "dev.h"
#include "com.h"
#include "printf.h"

/******** Defines ********/
#define MAX_DATA_BUFFER_SIZE 6

/******** Function prototypes ********/
void led_status_blink(void);

/******* Global variables ********/
char *buffer;

/*
 * This has to be considered as the main function of the module, invoked upon bootstrap'ing
 * within the on-chip memory
 */
void start(void)
{	
	uint8_t index, count;
	char *temp;

	/* Start thread */
	mos_thread_new(led_status_blink, 128, PRIORITY_NORMAL);

	/* Initialize global variables */		
	buffer = (char *)mos_mem_alloc((uint16_t)MAX_DATA_BUFFER_SIZE);	
	temp = (char *)mos_mem_alloc((uint16_t)MAX_DATA_BUFFER_SIZE);
	if(buffer == NULL || temp == NULL)
	{
		/* Memory allocation failed */		
		exit(0);
	}
	else	
	{			
		/* Allocation succesfully completed */
		printf("Initializing buffer memory... ");
		memset(buffer, 0, MAX_DATA_BUFFER_SIZE);
		printf("done.\n");			
		printf("Initializing temp memory... ");
		memset(temp, 0, MAX_DATA_BUFFER_SIZE);
		printf("done.\n");
	}

	/* Turn the flash controller on */
	dev_mode(DEV_TELOS_FLASH, DEV_MODE_ON);
	
	dev_ioctl(DEV_TELOS_FLASH, TELOS_FLASH_BULK_ERASE);
	dev_ioctl(DEV_TELOS_FLASH, DEV_SEEK, 0);

	/* Gain exclusive access to the FLASH memory */
	dev_open(DEV_TELOS_FLASH);	
	printf("Exclusive lock has been aquired; critical section begins\n");
	/* Access acquired */
	count = dev_write(DEV_TELOS_FLASH, "abcdef", 6);
	printf("Succesfully written %d bytes to FLASH\n", count);
	/* Release the lock */
	dev_close(DEV_TELOS_FLASH);	
	printf("Exclusive lock has been released; critical section ends\n");

	/* Read back written data, and output it in 3-digits 
     * decimal representation */	
	dev_open(DEV_TELOS_FLASH);		
	dev_read(DEV_TELOS_FLASH, temp, 3/*MAX_DATA_BUFFER_SIZE*/);
	printf("Entire buffer has been read from FLASH memory\n");	
	for(index = 0; index < 3/*MAX_DATA_BUFFER_SIZE*/; index++)		
	{	
		printf("[%c] has been read from FLASH memory\n", temp[index]);		
		mos_thread_sleep(500);	
	}		
	dev_close(DEV_TELOS_FLASH);

	/* Release allocated resources */
	mos_mem_free(buffer);
	mos_mem_free(temp);
	dev_mode(DEV_TELOS_FLASH, DEV_MODE_OFF);
	printf("Resources have been freezed\n");
}

/*
 * The following will be threaded to make a led blink 
 */
void led_status_blink(void)
{
	uint32_t sleep_time;

	/* Repeatedly blink */
	sleep_time = 250;
	while(1)
	{
		mos_thread_sleep(sleep_time);
		mos_led_toggle(2);				/* The blue one */
	}
}

