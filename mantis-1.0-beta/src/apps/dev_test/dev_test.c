/**
 * Testing the dev_*_TELOS_FLASH(...) APIs for FLASH I/O capabilities
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
#define BUFFER_SIZE	16

/******* Global variables ********/
char *buffer;

void start(void)
{	
	uint16_t count;
	uint8_t index;

	buffer = (char *)mos_mem_alloc((uint16_t)BUFFER_SIZE);
	memset(buffer, 'X', BUFFER_SIZE);

	/* For safety, erase entire FLASH */
	dev_ioctl(DEV_TELOS_FLASH, TELOS_FLASH_BULK_ERASE);

	/* Turn on the FLASH */
	dev_mode(DEV_TELOS_FLASH, DEV_MODE_ON);
	
	/* Acquire lock on FLASH and preliminarly 
	 * write 64 bits of data */
	dev_open(DEV_TELOS_FLASH);
	count = dev_write(DEV_TELOS_FLASH, "abcdefgh", 8);
	printf("%d bytes of data have been written to FLASH memory\n", count);
	dev_close(DEV_TELOS_FLASH);

			/* Perform experiments over R/W pointer to FLASH */	

	/* Experiment#1 - flash is on, lock is free; aquire lock and read 
	 *                without using any SEEK function */
	dev_open(DEV_TELOS_FLASH);
	count = dev_read(DEV_TELOS_FLASH, buffer, 1);
	printf("#1 : %c has been read from FLASH memory\n", buffer[0]);	
	dev_close(DEV_TELOS_FLASH);

	/* Move pointer */
	dev_open(DEV_TELOS_FLASH);
	dev_ioctl(DEV_TELOS_FLASH, DEV_SEEK, 3);
	dev_close(DEV_TELOS_FLASH);	

	/* Experiment#2 - flash is on, lock is free; aquire lock, read single data, 
	 * write single data and read multiple data from it */
	dev_open(DEV_TELOS_FLASH);
	dev_read(DEV_TELOS_FLASH, buffer, 1);
	printf("#2 : %c has been read\n"
		   "   : FLASH memory written\n", buffer[0]);
	dev_write(DEV_TELOS_FLASH, "l", 1);	
	count =	dev_read(DEV_TELOS_FLASH, buffer, 1);
	printf("   : %d bytes have been read from FLASH memory: ", count);
	for(index = 0; index < count; index++)
	{
		printf("%c ", buffer[index]);
	}
	printf("\n");
	dev_close(DEV_TELOS_FLASH);

	/* Release lock and free resources */
	dev_close(DEV_TELOS_FLASH);
	dev_mode(DEV_TELOS_FLASH, DEV_MODE_OFF);
	mos_mem_free(buffer);

	return;
}

