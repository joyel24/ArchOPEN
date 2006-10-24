include ../../Makefile.conf

GPPFLAGS += -I../../include -I./

ifeq ("$(TARGET_DEVICE)","AV3XX")
SRC=i2c_DVR.cpp
else
SRC="NOTHING"
endif

BUILD_DIR = ../../build/

all: do_build

clean: clean-subdirs
	@rm -f *.o *~ *.d
                        
include ../../default.rule