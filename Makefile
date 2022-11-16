CONTIKI_PROJECT = WirelsesSensorNetworks
all: $(CONTIKI_PROJECT)

MAKE_NET = MAKE_NET_NULLNET


CONTIKI = ..
#include $(CONTIKI)/Makefile.include
#TARGET_LIBFILES = -lm


PLATFORMS_EXCLUDE = nrf52dk

#use this to enable TSCH: MAKE_MAC = MAKE_MAC_TSCH
MAKE_MAC ?= MAKE_MAC_CSMA
MAKE_NET = MAKE_NET_NULLNET
include $(CONTIKI)/Makefile.include