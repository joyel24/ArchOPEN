//  Gontactor
//   apps for MediOS project
//   Copyright (c) 2006 by Captain H@dock (tanguycano@hotmail.com)
//
// All files in this archive are subject to the GNU General Public License.
// See the file COPYING in the source tree root for full license agreement.
// This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
// KIND, either express of implied.

//------------------------------------------------------------------------------
//  File : bmp_load.h                  		Author : Captain H@dock
//  Last change : 30/12/2006            	Created : 30/12/2006
//
//  Function to load and print bmp from disk to screen
//------------------------------------------------------------------------------
#ifndef BMP_LOAD
#define BMP_LOAD

//Prototypes
int print_pal_bmp32(char *filepath, int x, int y);
int BGR_to_pal(char *buff);


#endif
