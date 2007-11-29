include $(MEDIOS_SRC_PATH)/Makefile.conf

ifeq ("$(TARGET_TYPE)","std")
SUBDIRS = kernel driver gui init
endif

ifeq ("$(TARGET_TYPE)","avlo")
SUBDIRS = kernel driver init
endif

SUBDIRS_CLEAN = build

all: builddir subdirs 

builddir:
	@mkdir -p build
        
include $(MEDIOS_SRC_PATH)/default.rule