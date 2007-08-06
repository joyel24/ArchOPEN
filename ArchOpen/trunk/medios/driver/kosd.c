/*
*   driver/kosd.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <driver/osd.h>

int osd_getTrspBit()
{
    return OSD_BITMAP_TRSP_ENABLE_BIT;
}

int osd_getBlendFactorBit(int factor)
{
    return OSD_BITMAP_BLEND_FACTOR(factor);
}

