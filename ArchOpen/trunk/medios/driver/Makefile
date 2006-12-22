include $(MEDIOS_SRC_PATH)/Makefile.conf

SRC =  ata.c cpld.c i2c.c rtc.c buttons.c usb_fw.c bat_power.c clkc.c cache.c lcd.c \
    fm_remote.c uart.c wdt.c dsp.c videnc.c

ifeq ("$(TARGET_DEVICE)","AV3XX")
	SRC += tsc2003.c
endif

ifeq ("$(TARGET_DEVICE)","AV4XX")
	SRC += tsc2003.c
endif

ifeq ($(HAVE_EXT_MODULE),1)
	SRC += ext_module.c dvr_module.c ir_remote.c cf_module.c
endif

ifeq ($(HAVE_MAS_SOUND),1)
	SRC +=  mas.c
endif

ifeq ($(HAVE_AIC23_SOUND),1)
	SRC += aic23.c
endif

all: do_build

include $(MEDIOS_SRC_PATH)/default.rule