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


/* AV5 definition */
#include <kernel/io.h>
#include <driver/gio.h>

#define SPKR_ON()         {GIO_DIRECTION(GIO_SPKR,GIO_OUT); GIO_SET(GIO_SPKR);spkrState=1;}
#define SPKR_OFF()        {GIO_DIRECTION(GIO_SPKR,GIO_OUT); GIO_CLEAR(GIO_SPKR);spkrState=0;}
/* we have no way to know what is current speacker state ==> let's use medios internal car*/
#define SPKR_STATE()      (spkrState&0x1)
#define SPKR_AVAILABLE()  (1)

