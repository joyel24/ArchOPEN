/*
*   kernel/driver/cache.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <driver/cache.h>

void cache_clean(){
    CACHE_CLEAN();
}

void cache_enable(int mode, bool enable){
    if(enable){
        CACHE_ENABLE(mode);
    }else{
        // clean DCache before disabling it so data is written back to memory
        if (mode&CACHE_DATA){
            CACHE_CLEAN();
        }

        CACHE_DISABLE(mode);
    }
}

void cache_invalidate(int mode){
    // clean DCache before invalidating it so data is written back to memory
    if (mode&CACHE_DATA){
        CACHE_CLEAN();
    }

    CACHE_INVALIDATE(mode);
}

void cache_invalidateRange(int mode, void * start,int size){
    int startMVA,endMVA,i;

    startMVA=(unsigned int)start & 0xffffffe0;
    endMVA=((unsigned int)start+size)& 0xffffffe0;

    for(i=startMVA;i<=endMVA;i+=32){
        CACHE_INVALIDATE_ADDRESS(mode,i);
    }
}

bool cache_enabled(int mode){
    int status;

    CACHE_STATUS(status);

    return ((mode&CACHE_CODE) && (status&CACHE_STATUS_CODE)) ||
           ((mode&CACHE_DATA) && (status&CACHE_STATUS_DATA));
}

