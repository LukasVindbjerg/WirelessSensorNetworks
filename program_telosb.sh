#!/bin/bash

if [[ -z $1 ]];
then
    echo "No ihex file passed."
    echo "Use: ./program.sh <ihex>"
    exit 1
fi

IHEX=$(pwd)/$1

/home/user/contiki-ng/tools/sky/msp430-bsl-linux --telosb -c /dev/ttyUSB0 -e #erase
/home/user/contiki-ng/tools/sky/msp430-bsl-linux --telosb -c /dev/ttyUSB0 -I -p ${IHEX} #program
/home/user/contiki-ng/tools/sky/msp430-bsl-linux --telosb -c /dev/ttyUSB0 -r #reboot