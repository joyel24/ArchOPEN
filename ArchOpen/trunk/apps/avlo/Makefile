include ../../Makefile.conf

#GCCFLAGS = -Wall -mlong-calls  -nostdlib -fno-builtin  -mcpu=arm7tdmi -Os

ASFLAGS  += -I./include -I../../include -I. 
GCCFLAGS += -I./include -I../../include -I. 

TARGET = avlo

LIBS = ../../lib/libmedios.a

SUBDIRS_CLEAN = build

SUBDIRS = target
#target

BUILD_DIR = build

SRC = avlo.c parse_cfg.c resetMem.S

#ifeq ($(HAVE_IMG),1)
#OBJS += img.S
#endif
       
$(LIBS):
	make -C ../../ lib_medios
        
stdalone: GCCFLAGS += -DBUILD_STDALONE
stdalone: builddir incdir do_build

builddir:
	@mkdir -p build
	
incdir:
	make -C include/ all
	make -C target/ incdir_$(TARGET_DEVICE)

include ../../default.rule
