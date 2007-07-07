/*
*   include/kernel/cache_DM270.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __CACHE_DM270_H
#define __CACHE_DM270_H

#include <driver/cache.h>
#include <kernel/io.h>

#define CACHE_BASE                      0x00030e00

#define CACHE_SETUP                     (CACHE_BASE+0x0000)

#define CACHE_STP_ENABLE                0x0001
#define CACHE_STP_DISABLE               0x0000
#define CACHE_STP_CLEAR                 0x0002
#define CACHE_STP_4WORDS                0x0100

#define CACHE_STATUS_CODE               CACHE_STP_ENABLE
#define CACHE_STATUS_DATA               0

#define CACHE_CLEAN() {}

#define CACHE_DISABLE(mode) {                                                  \
    if(mode&CACHE_CODE){                                                       \
        outw(CACHE_STP_DISABLE,CACHE_SETUP);                                   \
    }                                                                          \
}

#define CACHE_ENABLE(mode) {                                                   \
    if(mode&CACHE_CODE){                                                       \
        outw(CACHE_STP_ENABLE|CACHE_STP_4WORDS,CACHE_SETUP);                   \
    }                                                                          \
}

#define CACHE_INVALIDATE(mode) {                                               \
    if (mode&CACHE_CODE){                                                      \
        int __mode;                                                            \
                                                                               \
        __mode=inw(CACHE_SETUP);                                               \
                                                                               \
        outw(CACHE_STP_CLEAR,CACHE_SETUP);                                     \
        while(inw(CACHE_SETUP)&CACHE_STP_CLEAR) /* wait */;                    \
                                                                               \
        outw(__mode,CACHE_SETUP);                                              \
    }                                                                          \
}

#define CACHE_INVALIDATE_ADDRESS(mode,address) {}

#define CACHE_STATUS(status) {                                                 \
    status=inw(CACHE_SETUP);                                                   \
}

#endif
