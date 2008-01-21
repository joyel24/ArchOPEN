include Makefile.conf

SUBDIRS = medios
SUBDIRS_CLEAN = $(MEDIOS_SRC_PATH)/lib  
SUBDIRS_FULL_CLEAN = $(MEDIOS_SRC_PATH)/codec

ifeq ("$(TARGET_TYPE)","std")
TARGET = medios_shell
endif

ifeq ("$(TARGET_TYPE)","avlo")
TARGET = avlo
endif

KERNEL_OBJS = medios/build/*.o medios/target/$(CHIP_TYPE)/build/*.o medios/target/$(CHIP_TYPE)/$(TARGET_DEVICE)/build/*.o

export MEDIOS_SRC_PATH=$(shell pwd)

all: $(TARGET)

lib_dir:
	@mkdir -p $(MEDIOS_SRC_PATH)/lib

mk_api_file:
	@make -C tools/api_files/ --no-print-directory

mk_version:
	@chmod 777 ./tools/version/mkVersion
	@./tools/version/mkVersion medios/include/kernel/version_svn.h

mk_lang:
	@make -C tools/lang/ --no-print-directory

inc_dir:
	@make -C medios/include/target --no-print-directory inc_dir
	@make -C medios/include/target/chip --no-print-directory inc_dir
	@make -C medios/include/target/chip/arch --no-print-directory memDef

                
mk_build_tools:
	@make -C tools/buildTools --no-print-directory
                
medios_shell: medios.bin medios.wav medios.avi
        
medios.bin: inc_dir lib_dir mk_api_file mk_build_tools mk_lang subdirs
	@echo '-----   Linking medios.bin 1/2'
	@chmod 777 ./tools/lds_files/create_lds
	@./tools/lds_files/create_lds init
	@$(LD) -N -Tmedios.lds -o medios.elf $(KERNEL_OBJS) $(LDFLAGS) -Map medios.map
	@echo '-----   Linking medios.bin 2/2'
	@./tools/lds_files/create_lds final
	@$(LD) -N -Tmedios.lds -o medios.elf $(KERNEL_OBJS) $(LDFLAGS) -Map medios.map
	@$(OBJCOPY) -O binary medios.elf medios.bin
	@$(ADDSIZE) medios.bin

medios_lib: inc_dir mk_api_file mk_lang
	@make -C $(MEDIOS_SRC_PATH)/medios --no-print-directory -s medios_lib

aoboy: APP_NAME = aoboy
aoboy: do_build_app
        
aodoom: LD_BIN_INC = apps/aodoom/dspcode/doom_dsp.out
aodoom: APP_NAME = aodoom
aodoom: do_build_app

aones: LD_BIN_INC = apps/aones/dspcode/aones_dsp.out
aones: APP_NAME = aones
aones: do_build_app

aosms: LD_BIN_INC = apps/aosms/dspcode/aosms_dsp.out
aosms: APP_NAME = aosms
aosms: do_build_app

pico: LD_BIN_INC = apps/pico/dspcode/pico_dsp.out
pico: APP_NAME = pico
pico: do_build_app

avlo: APP_NAME = avlo
avlo: do_build_app

include default.rule
