include $(MEDIOS_SRC_PATH)/Makefile.conf


JPEG_LIB_VERSION = 62

# Put here the object file name for the correct system-dependent memory
# manager file.  For Unix this is usually jmemnobs.o, but you may want
# to use jmemansi.o or jmemname.o if you have limited swap space.
SYSDEPMEM= jmemnobs.o

# library (.a) file creation command
AR= arm-linux-ar rc
# second step in .a creation (use "touch" if not needed)
AR2= arm-linux-ranlib

# End of configurable options.

COMOBJECTS= jcomapi.o jutils.o jerror.o jmemmgr.o $(SYSDEPMEM)

# decompression library object files
DLIBOBJECTS= jdapimin.o jdapistd.o jdtrans.o jdatasrc.o \
        jdmaster.o jdinput.o jdmarker.o jdhuff.o jdphuff.o \
        jdmainct.o jdcoefct.o jdpostct.o jddctmgr.o \
        jidctfst.o jidctflt.o jidctint.o jidctred.o \
        jdsample.o jdcolor.o jquant1.o jquant2.o jdmerge.o
# These objectfiles are included in libjpeg.a
MEDIOSOBJECTS= $(DLIBOBJECTS) $(COMOBJECTS)

all:  libmedjpeg.a 

libmedjpeg.a: $(MEDIOSOBJECTS)
	@$(RM) -f libmedjpeg.a
	@$(AR) libmedjpeg.a  $(MEDIOSOBJECTS)
	@$(RANLIB) libmedjpeg.a
	@cp libmedjpeg.a ../../../

include $(MEDIOS_SRC_PATH)/default.rule
