CONTIKI_PROJECT = WirelsesSensorNetworks
all: $(CONTIKI_PROJECT)

MAKE_NET = MAKE_NET_NULLNET

CONTIKI = ..
include $(CONTIKI)/Makefile.include
TARGET_LIBFILES = -lm
