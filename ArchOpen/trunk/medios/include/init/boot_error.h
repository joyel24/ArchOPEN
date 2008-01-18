/*
*   include/init/boot_error.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __BOOT_ERROR_H
#define __BOOT_ERROR_H

/* type definition */

#define BOOT_ERROR 1
#define BOOT_WARN  2


/* Error definition */

#define MISSING_MEDIOS_ERROR 0
#define MISSING_CODEC_ERROR 1
#define DSP_CODEC_ERROR 2
#define MISSING_LANG_FOLDER_ERROR 3
#define MISSING_LANG_FILE_ERROR 4
#define MISSING_ICON_FOLDER_ERROR 5
#define OLD_ICON_FOLDER_ERROR 6
#define MISSING_HANDLER_FILE_ERROR 7
#define MISSING_MENU_FILE_ERROR 8
#define MISSING_IMG_FILE_ERROR 9
#define WRONG_LANG_VERSION_ERROR 10

/* function declaration */

void gui_bootError(int errorNumber,int type);

#endif
