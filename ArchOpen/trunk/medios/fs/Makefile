include $(MEDIOS_SRC_PATH)/Makefile.conf

ifeq ("$(TARGET_TYPE)","std")
SRC=bflat.c fat_lowLevel.c vfs.c vfs_fsManip.c disk.c file_type.c vfs_dops.c vfs_node.c fat.c med.c vfs_fops.c \
    vfs_pathname.c bin_load.c cfg_file.c csv_file.c
endif
   
ifeq ("$(TARGET_TYPE)","avlo")
SRC=fat_lowLevel.c vfs.c vfs_fsManip.c disk.c file_type.c vfs_dops.c vfs_node.c fat.c vfs_fops.c \
    vfs_pathname.c
endif
    
all: do_build

include $(MEDIOS_SRC_PATH)/default.rule