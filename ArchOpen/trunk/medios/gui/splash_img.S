/*
*   kernel/gfx/splash_img.S
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

.section ".text"
.asciz "SPLASHSCR"
.align 8
.globl splash_img
splash_img:
.ifdef SPLASH_320x240
.incbin "splash_320x240.img"
.else
.ifdef SPLASH_220x176
.incbin "splash_220x176.img"
.else
.space 320*240*4
.endif
.endif

