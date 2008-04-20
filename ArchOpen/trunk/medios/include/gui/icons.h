/*
* include/gui/icons.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __K_ICONS_H
#define __K_ICONS_H

#include <gui_user/icons.h>

#include <gui/shell.h>

#define ICON_DIR SHELL_DIR"/icons"
#define ICON_OTHER_DIR SHELL_DIR"/icons/others"

void icon_init(void);
void icon_kernelInit(void);

extern int folderType;

struct icon_elem * icon_load(char * filename);
struct icon_elem * icon_loadForce(char * filename);
struct icon_elem * icon_loadOther(char * filename);
struct icon_elem * icon_loadFlag(char * filename,int force, int other);
struct icon_elem * icon_add(char * name,unsigned char * data,int w,int h);
struct icon_elem * icon_get(char * name);

void icon_setPath(void);

#define ICON_SMALL    0
#define ICON_STD      1
#define ICON_BIG      2

#define ICON_DIR_SMALL "/small"
#define ICON_DIR_STD   "/std"
#define ICON_DIR_BIG   "/big"

/* icon list
- linavLogo
- usbIcon
- powerIcon
- upBitmap
- dwBitmap
- dirBitmap
- mp3Bitmap
- textBitmap
- imageBitmap
- MsgBoxExclamationBitmap
- MsgBoxQuestionBitmap
- MsgBoxWarningBitmap
- MsgBoxInformationBitmap
- MsgBoxErrorBitmap
*/

#endif
