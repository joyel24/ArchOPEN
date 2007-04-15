//  Gontactor
//   apps for MediOS project
//   Copyright (c) 2006 by Captain H@dock (tanguycano@hotmail.com)
//
// All files in this archive are subject to the GNU General Public License.
// See the file COPYING in the source tree root for full license agreement.
// This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
// KIND, either express of implied.

//------------------------------------------------------------------------------
//  File : printscreen.h		                         Author : Captain H@dock
//  Last change : 13/12/2006                    Created : 11/12/2006
//
//  Save current screen in a file
//------------------------------------------------------------------------------


#ifndef PRINTSCREEN_H
#define PRINTSCREEN_H

//Constant
//~ #define LCD_WIDTH		220
//~ #define LCD_HEIGTH		176

//Prototypes
void gui_printscreen(char *filename);
void pal_to_BGR(int pix, char *buff);

#endif

