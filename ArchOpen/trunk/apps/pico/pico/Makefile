include ../../../Makefile.conf

ASFLAGS  += -I../../../include  -I. -I../ -I../cyclone
GCCFLAGS += -I../../../include -I. -I../ -I../cyclone -DEMU_C68K -D_USE_DRZ80 -O3

BUILD_DIR = ../build
all: do_build

include ../../../default.rule
