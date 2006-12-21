include ../../Makefile.conf

GPPFLAGS += -I../../include -I./

BUILD_DIR = ../../build/

SRC = i2c_device.cpp i2c_gpio.cpp

ifeq ("$(TARGET_DEVICE)","AV3XX")
SRC += mas.cpp i2c_TSC.cpp i2c_RTC.cpp gio_vid_uart1RX.cpp gio_spdif_uart1TX.cpp gio_lcd.cpp
endif

ifeq ("$(TARGET_DEVICE)","AV1XX")
SRC += mas.cpp i2c_RTC.cpp gio_bat_lvl.cpp
#gio_vid_uart1RX.cpp gio_spdif_uart1TX.cpp gio_lcd.cpp
endif

ifeq ("$(TARGET_DEVICE)","JBMM")
SRC += i2c_RTC.cpp gio_lcd.cpp mas.cpp gio_bat_lvl.cpp
endif

ifeq ("$(TARGET_DEVICE)","AV4XX")
SRC += i2c_RTC.cpp gio_connection.cpp
endif

ifeq ("$(TARGET_DEVICE)","Gmini400")
SRC += i2c_RTC.cpp gio_connection.cpp
endif


all: do_build

clean: clean-subdirs
	@rm -f *.o *~ *.d
                        
include ../../default.rule