include $(MEDIOS_SRC_PATH)/Makefile.conf


JPEG_LIB_VERSION = 62

# Put here the object file name for the correct system-dependent memory
# manager file.  For Unix this is usually jmemnobs.o, but you may want
# to use jmemansi.o or jmemname.o if you have limited swap space.
SYSDEPMEM= jmemnobs.o

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

all:  libjpeg.a

libjpeg.a: $(MEDIOSOBJECTS)
	@$(RM) -f libjpeg.a
	@$(AR) rc libjpeg.a  $(MEDIOSOBJECTS)
	@$(RANLIB) libjpeg.a
	@cp libjpeg.a $(LIB_DIR)

include $(MEDIOS_SRC_PATH)/default.rule
