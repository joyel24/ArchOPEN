include ../../../Makefile.conf

GPPFLAGS += -I../../../include -I./

BUILD_DIR = ./build/



SUBDIRS = $(TARGET_DEVICE)

all: builddir do_build

builddir:
	@mkdir -p build

clean: clean-subdirs
	@rm -f *.o *~ build/* *.d
                        
include ../../../default.rule