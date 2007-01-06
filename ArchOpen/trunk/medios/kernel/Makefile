include $(MEDIOS_SRC_PATH)/Makefile.conf

SRC = irq.c malloc.c pipes.c printk.c timer.c trap_fct.c swi.c kdelay.c kernel.c \
      delay.S trap.S thread.c cmd_line.c cmd_line_fct.c console.c


ifeq ($(HAVE_EVT),1)
	SRC += evt.c
endif

ifeq ($(HAVE_DBUG),1)
	SRC += bkpt.S
else
	SRC += bkpt_dummy.c
endif


all: do_build

include $(MEDIOS_SRC_PATH)/default.rule