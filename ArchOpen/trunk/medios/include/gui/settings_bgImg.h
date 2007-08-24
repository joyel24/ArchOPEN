/*
* include/gui/settings_bgImg.h
*
* MediOS project
* Copyright (c) 2005 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#ifndef __SETTINGS_BGIMG_H
#define __SETTINGS_BGIMG_H

extern int has_bgImg;
extern int bgImg_enable;
extern int blendMode;
extern int blendFactor;

void bgImg_setting(void);
void bgImg_loadPref(void);

#endif
