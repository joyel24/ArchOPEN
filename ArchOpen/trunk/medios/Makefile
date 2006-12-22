include $(MEDIOS_SRC_PATH)/Makefile.conf

SUBDIRS = api init kernel lib driver fs gfx gui target

SUBDIRS_CLEAN = build

all: builddir subdirs

medios_lib: SUBDIRS = api
medios_lib: subdirs

builddir:
	@mkdir -p $(BUILD_DIR)
        
include $(MEDIOS_SRC_PATH)/default.rule