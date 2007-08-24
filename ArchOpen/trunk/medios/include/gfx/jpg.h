/*
*   gfx/jpg.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __JPG_H
#define __JPG_H

#include <jpeglib.h>

MED_RET_T gfx_loadJpg(char * filename,JMETHOD(void, progress_draw, (j_common_ptr)));

#endif
