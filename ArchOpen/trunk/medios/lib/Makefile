include $(MEDIOS_SRC_PATH)/Makefile.conf

ifeq ("$(TARGET_TYPE)","std")
SUBDIRS += jpeg-6b
endif

ifeq ($(USE_GDB),1)
SUBDIRS += gdb
endif

ifeq ("$(TARGET_TYPE)","avlo")
SRC = ctype.c memchr.S memcpy.S memset.S memzero.S qsort.c strchr.S random.c string.c \
	strrchr.S strtod.c vsprintf.c
endif


all: do_build

include $(MEDIOS_SRC_PATH)/default.rule