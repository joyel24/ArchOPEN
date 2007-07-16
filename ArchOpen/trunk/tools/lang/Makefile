include $(MEDIOS_SRC_PATH)/Makefile.conf

BASE_FILE=./$(MEDIOS_LANG).lang
LANG_H_DEST="../../include/sys_def/lang.h"
LANG_B_DEST="../../medios/kernel/lang_data.bin"

SUBDIRS_CLEAN_OBJS=*.lng

all:
	@echo "Creating lang files"
	@chmod 777 ./script/genSource
	@chmod 777 ./script/genBin
	@chmod 777 ./script/mkBin
	@./script/genSource $(BASE_FILE) $(LANG_H_DEST)
	@./script/mkBin
	@cp ./$(MEDIOS_LANG).lng $(LANG_B_DEST)