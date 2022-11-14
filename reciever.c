
/**
 * \file
 *         Program that tests the RSSI for channels 11-26  
 * \author
 *         Malthe Tøttrup <201907882@post.au.dk>
 * 
 *         $ make TARGET=sky distclean 
 *         $ make TARGET=sky MOTES=/dev/ttyUSB0 reciever.upload login
 */

#include "contiki.h"
#include <stdio.h> /* For printf() */
#include <math.h>
#include <os/dev/radio.h>  
#include "net/nullnet/nullnet.h"
#include "net/netstack.h"

/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
AUTOSTART_PROCESSES(&hello_world_process);

void input_callback(const void *data, uint16_t len, const linkaddr_t *src, const linkaddr_t *dest)
{
    printf("123");
}
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(hello_world_process, ev, data)
{   
    PROCESS_BEGIN();

    static struct etimer et;
    
    etimer_set(&et, 2 * CLOCK_SECOND);

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));


    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
