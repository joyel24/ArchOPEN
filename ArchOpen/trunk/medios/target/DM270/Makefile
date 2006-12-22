include $(MEDIOS_SRC_PATH)/Makefile.conf

SUBDIRS = driver gfx $(TARGET_DEVICE)

SUBDIRS_CLEAN = build

all: builddir subdirs 
        
builddir:
	@mkdir -p build

include $(MEDIOS_SRC_PATH)/default.rule