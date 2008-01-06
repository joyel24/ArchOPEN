include $(MEDIOS_SRC_PATH)/Makefile.conf

ifeq ("$(TARGET_TYPE)","std")
SUBDIRS = api init kernel lib driver fs gfx gui snd target
endif

ifeq ("$(TARGET_TYPE)","avlo")
SUBDIRS = init kernel lib driver fs gfx gui target
endif

SUBDIRS_CLEAN = build

all: builddir subdirs

medios_lib: SUBDIRS = api
medios_lib: subdirs

builddir:
	@mkdir -p $(BUILD_DIR)
        
include $(MEDIOS_SRC_PATH)/default.rule