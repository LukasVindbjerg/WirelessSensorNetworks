
/**
 * \file
 *         Program that tests the RSSI for channels 11-26  
 * \author
 *         Malthe Tøttrup <201907882@post.au.dk>
 * 
 *         $ make TARGET=sky distclean 
 *         $ make TARGET=sky MOTES=/dev/ttyUSB0 broard_caster.upload login
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
PROCESS(hello_world_process, "Hello world process");
AUTOSTART_PROCESSES(&hello_world_process);
/*---------------------------------------------------------------------------*/

void input_callback(const void *data, uint16_t len,
  const linkaddr_t *src, const linkaddr_t *dest)
{
  if(len == sizeof(unsigned)) {
    unsigned count;
    memcpy(&count, data, sizeof(count));
    LOG_INFO("Received %u from ", count);
    LOG_INFO_LLADDR(src);
    LOG_INFO_("\n");
  }
}

PROCESS_THREAD(hello_world_process, ev, data)
{   
    PROCESS_BEGIN();
    static int channel = 11;
    NETSTACK_RADIO.on();

    //Connect to the desired channel
    if(NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, channel) != RADIO_RESULT_OK) 
    {
        printf("failed channel set");
    }
    static unsigned count = 0;
    static struct etimer et;

    nullnet_buf = (uint8_t *)&count;
    nullnet_len = sizeof(count);
    nullnet_set_input_callback(input_callback);

    // static linkaddr_t mote186 =         {{ 0x34, 0xa3, 0xdf, 0x1c, 0x00, 0x74, 0x12, 0x00 }}; //Lukas
    // static linkaddr_t mote182 =         {{ 0x83, 0xac, 0xdf, 0x1c, 0x00, 0x74, 0x12, 0x00 }}; //Daniel
    //static linkaddr_t mote118 =         {{ 0x5d, 0xe7, 0x93, 0x1c, 0x00, 0x74, 0x12, 0x00 }}; //Malthe
    //static linkaddr_t mote170 =         {{ 0x25, 0xac, 0xdf, 0x1c, 0x00, 0x74, 0x12, 0x00}}; //Jakob 

    
    etimer_set(&et, SEND_INTERVAL);
    while(1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
        LOG_INFO("Sending %u to ", count);
        LOG_INFO_LLADDR(NULL);
        LOG_INFO_("\n");
        
        memcpy(nullnet_buf, &count, sizeof(count));
        nullnet_len = sizeof(count);
        
        NETSTACK_NETWORK.output(NULL);
        count++;
        
        etimer_reset(&et);
    }

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
