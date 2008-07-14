include $(MEDIOS_SRC_PATH)/Makefile.conf

ifeq ("$(TARGET_TYPE)","std")
SRC = irq.c malloc.c pipes.c printk.c timer.c trap_fct.c swi.c kdelay.c kernel.c \
      delay.S trap.S thread.c cmd_line.c cmd_line_fct.c console.c lang.c lang_data.S
endif

ifeq ("$(TARGET_TYPE)","avlo")
SRC = irq.c malloc.c pipes.c printk.c timer.c trap_fct.c kdelay.c kernel.c \
      delay.S trap.S thread.c console.c no_swi.c cmd_line.c cmd_line_fct.c
endif

ifeq ($(HAVE_EVT),1)
	SRC += evt.c
endif

ifeq ($(USE_GDB),1)
	SRC += bkpt_dummy.c
else
ifeq ($(HAVE_DBUG),1)
	SRC += bkpt.S
else
	SRC += bkpt_dummy.c
endif
endif

lang_data.o : lang_data.bin

all: do_build

include $(MEDIOS_SRC_PATH)/default.rule