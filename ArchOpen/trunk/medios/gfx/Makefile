include $(MEDIOS_SRC_PATH)/Makefile.conf

ifeq ("$(TARGET_TYPE)","std")
SRC = graphics_32.c graphics.c resize.c screens.c jpg.c bmp.c
endif

ifeq ("$(TARGET_TYPE)","avlo")
SRC = graphics_32.c graphics.c screens.c
endif


SUBDIRS = fonts

all: do_build

include $(MEDIOS_SRC_PATH)/default.rule