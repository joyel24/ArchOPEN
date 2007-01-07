/*
*   include/target/driver/speaker.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#include <driver/cpld.h>

#define SPKR_ON()         {CPLD_CLEAR_PORT3(CPLD_SPKR);}
#define SPKR_OFF()        {CPLD_SET_PORT3(CPLD_SPKR);}
#define SPKR_STATE()      (cpld_read(CPLD3)&0x2?0:1)
#define SPKR_AVAILABLE()  (1)
