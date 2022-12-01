
/**
 * \file
 *         Program that tests the RSSI for channels 11-26  
 * \author
 *         Malthe Tøttrup <201907882@post.au.dk>
 * 
 *         $ make TARGET=sky distclean 
 *         $ make TARGET=sky MOTES=/dev/ttyUSB0 node1.upload login
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
#define PACKET_LEN 120

/*
typedef struct{
     char data[PACKET_LEN];
}packet_t;
*/
static int packet_recieved = 0;
static int change_channel = 0;

/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
PROCESS(measure_process, "measure process");
AUTOSTART_PROCESSES(&hello_world_process,&measure_process);

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
    change_channel = 0;
    packet_recieved++;
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

/*
    packet_t jpacket;
    memset(&jpacket, 0, sizeof(packet_t));
    strcpy(jpacket.data, "Antonio Gonga is taking down your network.");
*/

    //nullnet_buf = (void*)&jpacket;
    //nullnet_len = sizeof(packet_t);
    nullnet_buf = (uint8_t *)&count;
    nullnet_len = sizeof(count);
    nullnet_set_input_callback(input_callback);

    //static linkaddr_t mote186 =         {{ 0x34, 0xa3, 0xdf, 0x1c, 0x00, 0x74, 0x12, 0x00 }}; //Lukas
    //static linkaddr_t mote182 =         {{ 0x83, 0xac, 0xdf, 0x1c, 0x00, 0x74, 0x12, 0x00 }}; //Daniel
    //static linkaddr_t mote118 =         {{ 0x5d, 0xe7, 0x93, 0x1c, 0x00, 0x74, 0x12, 0x00 }}; //Malthe
    //static linkaddr_t mote170 =         {{ 0x25, 0xac, 0xdf, 0x1c, 0x00, 0x74, 0x12, 0x00}}; //Jakob 
    //static linkaddr_t node1 =           {{ 0x01, 0x01, 0x01, 0x00, 0x01, 0x74, 0x12, 0x00}};
    static linkaddr_t node2 =           {{ 0x02, 0x02, 0x02, 0x00, 0x02, 0x74, 0x12, 0x00}};
    //static linkaddr_t node3 =           {{ 0x03, 0x03, 0x03, 0x00, 0x03, 0x74, 0x12, 0x00}};

    
    etimer_set(&et, SEND_INTERVAL);

    while(1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
        LOG_INFO("Sending %d to ", count);
        LOG_INFO_LLADDR(&node2);
        LOG_INFO_("\n");
        
        // NETSTACK_NETWORK.output(&mote186);
        // NETSTACK_NETWORK.output(&mote182);
        NETSTACK_NETWORK.output(&node2);

        count++;
        change_channel++;

        
        etimer_reset(&et);

        if(change_channel >= 5){
          NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, 17);
          printf("changed to channel 17\n");
          change_channel = 0;
        }
    }

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------*/

PROCESS_THREAD(measure_process, ev, data)
{
  PROCESS_BEGIN();
  NETSTACK_RADIO.on();
  radio_value_t RSSI;
  radio_value_t TXPOWER;
  static struct etimer et;

  etimer_set(&et, 1*CLOCK_SECOND);

  while (1)
  {

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    if(NETSTACK_RADIO.get_value(RADIO_PARAM_RSSI, &RSSI) != RADIO_RESULT_OK) 
        {
            printf("failed get RSSI value");
        }
    printf("RSSI for channel %d = %d \n", 16, RSSI);
    
    
    if(NETSTACK_RADIO.get_value(RADIO_PARAM_TXPOWER, &TXPOWER) != RADIO_RESULT_OK) 
        {
            printf("failed get TXPOWER value");
        }
    printf("Power = %d \n", TXPOWER);
    
    etimer_reset(&et); 
  }
  
  PROCESS_END();
}