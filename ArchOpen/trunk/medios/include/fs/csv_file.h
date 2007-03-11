/*
* include/medios/fs/csv_file.h
*
* MediOS project
* Copyright (c) 2007 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#ifndef __CSV_FILE_H
#define __CSV_FILE_H

#include <sys_def/errors.h>

MED_RET_T csv_newFile(char * filename);
MED_RET_T csv_end(void);
MED_RET_T csv_line2Array(int nbItem,char ** item_array,char sepChar);
MED_RET_T csv_readLine(void * data,char * formatStr,char sepChar);

#endif
