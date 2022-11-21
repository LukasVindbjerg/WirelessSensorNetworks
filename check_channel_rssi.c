
/**
 * \file
 *         Program that tests the RSSI for channels 11-26  
 * \author
 *         Malthe Tøttrup <201907882@post.au.dk>
 * 
 *         $ make TARGET=sky distclean 
 *         $ make TARGET=sky MOTES=/dev/ttyUSB0 check_channel_rssi.upload login
 */

#include "contiki.h"
#include <stdio.h> /* For printf() */
#include <math.h>
#include <os/dev/radio.h>  
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

    radio_value_t RSSI;

    static int channel = 11;
    NETSTACK_RADIO.on();

    //Connect to the desired channel
    if(NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, channel) != RADIO_RESULT_OK) 
    {
        printf("failed channel set");
    }

    //Listen to the RSSI for 15 seconds of particular channel. Measure the RSSI every 1 second.
    while(1){

        etimer_set(&et, CLOCK_SECOND);                  //start 1 second timer.
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et)); 

        if(NETSTACK_RADIO.get_value(RADIO_PARAM_RSSI, &RSSI) != RADIO_RESULT_OK) 
        {
            printf("failed get RSSI value");
        }
        printf("RSSI for channel %d = %d \n", channel, RSSI);

    }    

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
