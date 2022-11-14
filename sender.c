
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
/*---------------------------------------------------------------------------*/
PROCESS_THREAD(hello_world_process, ev, data)
{   
    PROCESS_BEGIN();

    static struct etimer et;
    
    etimer_set(&et, 2 * CLOCK_SECOND);

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));

    uint8_t payload[64] = { 0 };
    nullnet_buf = payload;
    nullnet_len = 2;

    static linkaddr_t dest_addr =         {{ 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }};

    NETSTACK_NETWORK.output(dest_addr);
    

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
