/*
*   include/kernel/disk.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
* Part of this code is from Rockbox project
* Copyright (C) 2002 by Bj�n Stenberg
*/


#ifndef __DISK_H
#define __DISK_H

#include <sys_def/types.h>
#include <sys_def/errors.h>

#define HD_DISK      0
#define CF_DISK      1

#define DISK_PART_0   0
#define DISK_PART_1   1
#define DISK_PART_2   2
#define DISK_PART_3   3

#define FLUSH         1
#define NOFLUSH       0

#define NB_DISK      2

struct partition_info {
    int start;
    int size;
    int type;
    char strType[20];
    int active;
    int mounted;
};

struct hd_info_s {
	char serial[21];
	char firmware[9];
	char model [41];
    unsigned long size;
    int multi_sector;
    int has_security_features;
    int pio_mode;
    unsigned int features_support[3];
    struct partition_info * partition_list;
};

extern struct hd_info_s * disk_info[NB_DISK];

MED_RET_T disk_init(void);
int disk_initDone(void);

MED_RET_T disk_add(int disk);
MED_RET_T disk_rm(int disk);

MED_RET_T disk_rmAll(void);
MED_RET_T disk_addAll(void);

char * disk_getName(int id);

MED_RET_T disk_readPartition(int disk);

extern char * disk_name[];

#endif
