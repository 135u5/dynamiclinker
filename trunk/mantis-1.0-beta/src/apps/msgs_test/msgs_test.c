/**
 * This simple sample application is used to test the handshake capabilities that
 * can occur between the host and the sensor node, using USB line.
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
#define MAX_DATA_BUFFER_SIZE 5

/******** Function prototypes ********/
void led_status_blink(void);

/******* Global variables ********/

/* Ack static variable */
static comBuf *msg;

/*
 * This has to be considered as the main function of the module, invoked upon bootstrap'ing
 * within the on-chip memory
 */
void start(void)
{		
	/* Initialize com interface */
	com_mode(IFACE_SERIAL2, IF_LISTEN);

	/* Start thread */
	mos_thread_new(led_status_blink, 128, PRIORITY_NORMAL);		
	
	/* Wait next incoming request */
	printf("**** NODE : Waiting for incoming requests... ");
	msg = com_recv(IFACE_SERIAL2);
	printf("done --> msg.data contains {%s} bytes\n", msg->data);	

	/* Send back a message to the host */
	msg->size = sizeof("back");
	memset((char *)msg->data, 'A', COM_DATA_SIZE);
	printf("**** NODE : Sending back ACK message... ");
	com_send(IFACE_SERIAL2, msg);
	printf("done.\n");
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

