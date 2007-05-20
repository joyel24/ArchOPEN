include ../../Makefile.conf

ASFLAGS  += -I../../include -I../../medios/include -I. -Ipico -Icyclone -DLSB_FIRST -DEMU_C68K -D_USE_DRZ80

GCCFLAGS += -I../../include -I../../medios/include -I. -Ipico -Icyclone -DLSB_FIRST -DEMU_C68K -D_USE_DRZ80 -O3

TARGET = pico

SUBDIRS = pico drz80 cyclone

LIBS = ../../lib/libmedios.a

LDFLAGS =  -nostdlib --emit-relocs -L../../lib -lmedios -lfloat

BUILD_DIR = build

all: builddir $(LIBS) do_build
	@$(LD) -b binary dspcode/pico_dsp.out -b elf32-littlearm \
	 build/*.o ../../lib/*.o -o $(TARGET).med $(LDFLAGS) -Map pico.map
	@rm -f .elf *.gdb *.elf2flt

stdalone: GCCFLAGS += -DBUILD_STDALONE
stdalone: builddir do_build

$(LIBS):
	make -C ../../kernel lib_medios

builddir:
	@mkdir -p build

include ../../default.rule
