
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
#include "cc2420.h"
#include "os/storage/cfs/cfs.h"


#define LOG_MODULE "App"
#define LOG_LEVEL LOG_LEVEL_INFO
#define SEND_INTERVAL (2 * CLOCK_SECOND)
#define mote186 {{ 0x34, 0xa3, 0xdf, 0x1c, 0x00, 0x74, 0x12, 0x00 }} //Lukas
#define mote182 {{ 0x83, 0xac, 0xdf, 0x1c, 0x00, 0x74, 0x12, 0x00 }} //Daniel
#define mote118 {{ 0x5d, 0xe7, 0x93, 0x1c, 0x00, 0x74, 0x12, 0x00 }} //Malthe
#define mote170 {{ 0x25, 0xac, 0xdf, 0x1c, 0x00, 0x74, 0x12, 0x00 }} //Jakob 
#define node1   {{ 0x01, 0x01, 0x01, 0x00, 0x01, 0x74, 0x12, 0x00 }}
#define node2   {{ 0x02, 0x02, 0x02, 0x00, 0x02, 0x74, 0x12, 0x00 }}
#define node3   {{ 0x03, 0x03, 0x03, 0x00, 0x03, 0x74, 0x12, 0x00 }}

static const linkaddr_t network[3] = {node1, node2, node3};
int change_channel[2] = {0, 0};
static int packet_recieved = 0;

/*---------------------------------------------------------------------------*/
PROCESS(hello_world_process, "Hello world process");
PROCESS(moveing_average_process, "moveing average process");
AUTOSTART_PROCESSES(&hello_world_process,&moveing_average_process);

/*---------------------------------------------------------------------------*/

void input_callback(const void *data, uint16_t len,
  const linkaddr_t *src, const linkaddr_t *dest)
{
  if(len == sizeof(unsigned)) {
    unsigned count;
    memcpy(&count, data, sizeof(count));
    // LOG_INFO("Received %u from ", count);
    // LOG_INFO_LLADDR(src);
    // LOG_INFO_("\n");
    if(memcmp(src, &network[0], sizeof(src))){
      change_channel[0] = 0;
    }
    else if(memcmp(src, &network[1], sizeof(src))) {
      change_channel[1] = 0;
    }
    
    packet_recieved++;
  }
}

//write to file
void write_to_file(int value, char *filename){
  int file = cfs_open(filename, CFS_WRITE);
  cfs_write(file, (uint8_t *)&value, sizeof(value));
  cfs_close(file);
}


// Define a function that takes a pointer to an array and its length
int next_channel(const int *array, int cycle_len) {
  // Create a static variable to keep track of the current position in the array
  static int position = 0;

  // Increment the position
  position++;

  // If we've reached the end of the array, reset the position to 0
  if (position >= cycle_len) {
    position = 0;
  }

  // Return the element at the current position
  printf("changed to channel %d because of message shortage\n", array[position]);
  return array[position];
}

static const int channel_cycle[4] = {11, 17, 18, 19};



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


    etimer_set(&et, SEND_INTERVAL);

    while(1) {
        PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
        LOG_INFO("Send/recieved : %u/%u \n ", count, packet_recieved);

        NETSTACK_NETWORK.output(&network[0]);
        NETSTACK_NETWORK.output(&network[1]);

        count++;
        for (int i = 0; i < 2; i++){
          change_channel[i]++;
        }
        
        etimer_reset(&et);

        if(change_channel[0] >= 5 || change_channel[1] >= 5){
           NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, next_channel(channel_cycle, 4));
          for (int i = 0; i < 2; i++){
            change_channel[i] = 0;
          }
        }
    }

    PROCESS_END();
}
/*---------------------------------------------------------------------------*/



PROCESS_THREAD(moveing_average_process, ev, data)
{
  PROCESS_BEGIN();
  NETSTACK_RADIO.on();
  static radio_value_t RSSI;
  static struct etimer et;

  etimer_set(&et,0.1*CLOCK_SECOND);

  static int mavg_short = 20;
  static int mavg_long = 100;
  
  static int short_average_index = 0;
  static int short_average_array[20] = {0};
  static int short_total = 0;
  static int short_average = 0;

  static int long_average_index = 1;
  static int long_average_array[100] = {0};
  static int long_total = 0;
  static int long_average = 0;

  static int fill = 0;
  while(fill < mavg_long){
        if(NETSTACK_RADIO.get_value(RADIO_PARAM_RSSI, &RSSI) != RADIO_RESULT_OK) 
        {
            printf("failed get RSSI value");
        }
        long_average_array[fill] = RSSI;
        short_average_array[fill%20] = RSSI;
        fill++;
  }

  while (1)
  {
    

    PROCESS_WAIT_EVENT_UNTIL(etimer_expired(&et));
    if(NETSTACK_RADIO.get_value(RADIO_PARAM_RSSI, &RSSI) != RADIO_RESULT_OK) 
        {
            printf("failed get RSSI value");
        }
    
    long_average_array[long_average_index] = RSSI;
    for(int i = 0; i < mavg_long; i++){;
      long_total += long_average_array[i];
    }

    long_average = long_total/mavg_long;
    long_total = 0;

    if(long_average_index == mavg_long){
      long_average_index = -1;
    }
    long_average_index++;


    short_average_array[short_average_index] = RSSI;
    for(int i = 0; i < mavg_short; i++){
      short_total += short_average_array[i];
    }
    short_average = short_total/mavg_short;
    short_total = 0;

    if(short_average_index == mavg_short){
      short_average_index = -1;
    }
    short_average_index++;

    //printf("%d \t %d \t %d\n", RSSI, long_average, short_average);
    
    if( short_average *1.1 > long_average){
          NETSTACK_RADIO.set_value(RADIO_PARAM_CHANNEL, 17);
          printf("changed to channel 17 because of RSSI\n");
        
    }
    
    etimer_reset(&et); 
  }
  
  PROCESS_END();
}