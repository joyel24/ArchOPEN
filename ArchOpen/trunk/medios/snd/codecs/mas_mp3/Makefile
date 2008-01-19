include $(MEDIOS_SRC_PATH)/Makefile.conf

LDFLAGS = $(MEDIOS_SRC_PATH)/lib/codec_start.o $(MEDIOS_SRC_PATH)/lib/codec_stop.o -L$(MEDIOS_SRC_PATH)/lib -lmedios --emit-relocs -Map $(TARGET_CODEC).map 

BUILD_DIR=./
TARGET_CODEC=masMp3

all: do_build
	@$(LD) -o $(TARGET_CODEC).bin $(OBJS) $(LDFLAGS)
	@cat codec.info $(TARGET_CODEC).bin > $(TARGET_CODEC).cod
	@rm -f $(TARGET_CODEC).bin
	@mv $(TARGET_CODEC).cod $(MEDIOS_SRC_PATH)/codec

include $(MEDIOS_SRC_PATH)/default.rule