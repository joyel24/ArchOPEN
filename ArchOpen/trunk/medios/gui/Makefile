include $(MEDIOS_SRC_PATH)/Makefile.conf

ifeq ("$(TARGET_TYPE)","std")
SUBDIRS = file_browser widgets shell
# no SCR = ==> all source taken ! 
endif

ifeq ("$(TARGET_TYPE)","avlo")
#no subdirs
ifneq ("$(SPLASH_TYPE)","NONE")
SRC =  splash.c splash_img.S
else
SRC = 
endif
endif

all: do_build

include $(MEDIOS_SRC_PATH)/default.rule