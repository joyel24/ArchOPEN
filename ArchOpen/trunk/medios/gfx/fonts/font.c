/*
*   kernel/gfx/font.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <sys_def/stddef.h>

#include <gfx/kfont.h>

FONT font_table[NBFONT] ;

#define fnt_initFont(name)  extern FONT name; font_table[name->num]=name;

void fnt_init(void)
{
    int i;
    for(i=0;i<NBFONT;i++)
        font_table[i]=NULL;

#ifdef _4x6
    fnt_initFont(std4x6)
#endif
#ifdef _4x8
    fnt_initFont(std4x8)
#endif
#ifdef _5x7
    fnt_initFont(std5x7)
#endif
#ifdef _5x8
    fnt_initFont(std5x8)
#endif
#ifdef _6x9
    fnt_initFont(std6x9)
#endif
#ifdef _6x10
    fnt_initFont(std6x10)
#endif
#ifdef _6x12
    fnt_initFont(std6x12)
#endif
#ifdef _6x13
    fnt_initFont(std6x13)
#endif
#ifdef _7x13
    fnt_initFont(std7x13)
#endif
#ifdef _7x14
    fnt_initFont(std7x14)
#endif
#ifdef _8x13
    fnt_initFont(std8x13)
#endif
#ifdef _cursive_9x15
    fnt_initFont(cursive)
#endif
#ifdef _dagger_9x14
    fnt_initFont(dagger)
#endif
#ifdef _inkblot_9x13
    fnt_initFont(inkblot)
#endif
#ifdef _radon_8x12
    fnt_initFont(radon)
#endif
#ifdef _radonWide_9x12
    fnt_initFont(radonWide)
#endif
#ifdef _shadow_12x18
    fnt_initFont(shadow)
#endif
#ifdef _shadowBold_12x18
    fnt_initFont(shadowBold)
#endif
}

FONT fnt_fontFromId(int id){
    return font_table[id];
}
