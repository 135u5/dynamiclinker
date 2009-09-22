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

/* Ack static variable */
static comBuf ack;

/*
 * This has to be considered as the main function of the module, invoked upon bootstrap'ing
 * within the on-chip memory
 */
void start(void)
{	
	uint8_t index;
	char *temp;

	/* Start thread */
	mos_thread_new(led_status_blink, 128, PRIORITY_NORMAL);

	/* Initialize global variables */		
	buffer = (char *)mos_mem_alloc((uint16_t)MAX_DATA_BUFFER_SIZE);	
	temp = (char *)mos_mem_alloc((uint16_t)MAX_DATA_BUFFER_SIZE);
	if(buffer == NULL)
	{
		/* Memory allocation failed */		
		exit(0);
	}
	else	
	{			
		/* Allocation succesfully completed */
		memset(buffer, 0, MAX_DATA_BUFFER_SIZE);		
		printf("buffer has been correctly initialized\n");
		memset(temp, 0, MAX_DATA_BUFFER_SIZE);
		printf("temporary buffer has been correctly initialized\n");
	}

	/* Gain exclusive access to the FLASH memory */
	dev_open(DEV_TELOS_FLASH);	
	printf("Exclusive lock has been aquired; critical section begins\n");

	/* Access acquired */
	dev_write(DEV_TELOS_FLASH, "abcdef", 6);
	printf("6 bytes written to FLASH\n");

	/* Release the lock */
	dev_close(DEV_TELOS_FLASH);	
	printf("Exclusive lock has been released; critical section ends\n");

	/* Read back written data, and output it in 3-digits 
     * decimal representation */	
	dev_open(DEV_TELOS_FLASH);
	dev_read(DEV_TELOS_FLASH, temp, 6);
	printf("Entire buffer has been read from FLASH memory\n");
	dev_close(DEV_TELOS_FLASH);
	for(index = 0; index < 6; index++)		
	{	
		/* ---- uncomment the following if you want leds displaying
         * ---- decimal value */		
		//mos_led_display(temp[index]);
		printf("%c has been read from FLASH memory\n", temp[index]);
		mos_thread_sleep(500);	
	}	
	
	/* Release allocated resources */
	mos_mem_free(buffer);
	mos_mem_free(temp);
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

