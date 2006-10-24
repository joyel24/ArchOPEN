include ../Makefile.conf

GPPFLAGS += -I../include -I./

SUBDIRS = HW_TI target gpio

BUILD_DIR = ../build/

all: do_build

clean: clean-subdirs
	@rm -f *.o *~ *.d
                        
include ../default.rule
