/*
*   include/kernel/cache.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __CACHE_DSC25_H
#define __CACHE_DSC25_H

// no cache on this chip

#define CACHE_STATUS_CODE 0
#define CACHE_STATUS_DATA 0

#define CACHE_CLEAN() {}
#define CACHE_DISABLE(mode) {}
#define CACHE_ENABLE(mode) {}
#define CACHE_INVALIDATE(mode) {}
#define CACHE_INVALIDATE_ADDRESS(mode,address) {}
#define CACHE_STATUS(status) {status=0;}

#endif
