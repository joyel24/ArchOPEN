include $(MEDIOS_SRC_PATH)/Makefile.conf

LDFLAGS = $(MEDIOS_SRC_PATH)/lib/codec_start.o $(MEDIOS_SRC_PATH)/lib/codec_stop.o -L$(MEDIOS_SRC_PATH)/lib -lmedios --emit-relocs

BUILD_DIR=./
TARGET_CODEC=wav

all: do_build
	@$(LD) -o $(TARGET_CODEC).cod $(OBJS) $(LDFLAGS) -Map $(TARGET_CODEC).map
	@-mv $(TARGET_CODEC).cod $(MEDIOS_SRC_PATH)/codec

include $(MEDIOS_SRC_PATH)/default.rule

