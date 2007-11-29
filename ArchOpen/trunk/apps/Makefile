MEDIOS_SRC_PATH_TMP = $(shell pwd)
export MEDIOS_SRC_PATH = $(dir $(MEDIOS_SRC_PATH_TMP))

include $(MEDIOS_SRC_PATH)/Makefile.conf

SUBDIRS = aones aoboy bluecube aodoom aosms pacman Gontactor pico avlo

MED_LIST = $(patsubst %.c,%.med,$(wildcard *.c))

LIBS = $(MEDIOS_SRC_PATH)/lib/libmedios.a

LDFLAGS = $(MEDIOS_SRC_PATH)/lib/apps_start.o $(MEDIOS_SRC_PATH)/lib/apps_stop.o -L$(MEDIOS_SRC_PATH)/lib -lmedios --emit-relocs

$(LIBS):
	@make -C $(MEDIOS_SRC_PATH) --no-print-directory medios_lib

all_med: bluecube pacman Gontactor
	@for p in $(MED_LIST); do \
		make $$p; \
	done
        
        
%.med: %.o $(LIBS)
	@$(LD) -o $@ $< $(LDFLAGS)

include ../default.rule