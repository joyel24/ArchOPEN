include $(MEDIOS_SRC_PATH)/Makefile.conf

ifeq ("$(TARGET_TYPE)","std")
SRC =  ata.c cpld.c i2c.c buttons.c usb_fw.c batDc.c energy.c clkc.c cache.c lcd.c \
    fm_remote.c uart.c wdt.c dsp.c videnc.c speaker.c time.c kosd.c
endif

ifeq ("$(TARGET_TYPE)","avlo")
SRC =  ata.c cpld.c i2c.c buttons.c usb_fw.c batDc.c energy.c clkc.c cache.c lcd.c \
    fm_remote.c uart.c wdt.c time.c kosd.c
endif


ifeq ("$(TARGET_DEVICE)","AV3XX")
	SRC += tsc2003.c
endif

ifeq ("$(TARGET_DEVICE)","AV4XX")
	SRC += tsc2003.c  
endif

ifeq ($(HAVE_ST_RTC),1)
    SRC += rtc_ST.c 
endif

ifeq ($(HAVE_EXT_MODULE),1)
	SRC += ext_module.c dvr_module.c ir_remote.c cf_module.c
else

ifeq ($(HAVE_CF),1)
	SRC += cf.c
endif
endif

ifeq ($(HAVE_MAS_SOUND),1)
	SRC +=  mas.c
else
	SRC +=  mas_dummy.c
endif

ifeq ($(HAVE_AIC23_SOUND),1)
	SRC += aic23.c
endif



all: do_build

include $(MEDIOS_SRC_PATH)/default.rule