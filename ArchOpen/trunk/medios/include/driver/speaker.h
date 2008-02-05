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

#ifdef HAS_SPKR
#include <target/chip/arch/speaker.h>
#else
#define SPKR_ON()
#define SPKR_OFF()
#define SPKR_STATE()     (0)
#define SPKR_AVAILABLE() (0)
#endif

extern int spkrState; /*used if there is no way to know from HW speacker state*/

void speaker_enable(int enable);
int  speaker_state(void);
int  speaker_available(void);
