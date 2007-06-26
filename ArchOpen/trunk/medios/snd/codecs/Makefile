include $(MEDIOS_SRC_PATH)/Makefile.conf

SUBDIRS=wav tremor wavpack mad musepack mid tinysid
SUBDIRS_CLEAN=$(MEDIOS_SRC_PATH)/codec

all: codec_dir subdirs
	@cp -f ../dspcode/snd_dsp.out $(MEDIOS_SRC_PATH)/codec

include $(MEDIOS_SRC_PATH)/default.rule

codec_dir:
	@mkdir -p $(MEDIOS_SRC_PATH)/codec
