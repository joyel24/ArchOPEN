include Makefile.conf

GPPFLAGS += -I./include -I./

SUBDIRS = arm_CPU cmd_line core HW

TARGET = emu

all: clean_exe incdir builddir subdirs
	@echo '-----   building $(TARGET)'
	@$(GPP) $(GPPFLAGS) -o $(TARGET) $(LDFLAGS) build/*.o HW/target/$(TARGET_CHIP)/build/*.o  \
		HW/target/$(TARGET_CHIP)/$(TARGET_DEVICE)/build/*.o -lreadline -lncurses -L/usr/X11R6/lib -lX11

incdir:
	@make -C include/target --no-print-directory incdir
	
clean:  clean-subdirs
	@echo '-----   cleaning MAIN'
	@rm -f *.o *~  emu include/*~ build/* *.d
        
clean_exe:
	@rm -f emu.exe emu
	
builddir:
	@mkdir -p build
                
include default.rule