include $(MEDIOS_SRC_PATH)/Makefile.conf

ifeq ("$(TARGET_TYPE)","std")
SRC = main.c boot_error.c boot.S exit.c stack.S
endif

ifeq ("$(TARGET_TYPE)","avlo")
SRC = main.c boot.S exit.c stack.S
endif

all: do_build

include $(MEDIOS_SRC_PATH)/default.rule