/*
*   include/buttons.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __KBUTTONS_H
#define __KBUTTONS_H

#include <kernel/kernel.h>

#include <driver/hardware.h>
#include <driver/gio.h>

#include <sys_def/buttons.h>

#define DEFAULT_INIT_DELAY      20
#define DEFAULT_SECOND_DELAY    10
#define DEFAULT_MIN_DELAY       2
#define DEFAULT_DEC_VALUE       1

#include <target/chip/arch/buttons.h>

#ifndef MAX_OFF
#define MAX_OFF                     200
#endif

#define NB_BUTTONS                  0x10

extern const char * btn_name [NB_BUTTONS];

void btn_processPress(int val);
void btn_init(void);

int btn_readState(void);
int arch_btn_readHardware(void);

extern int nb_off_press;
extern int nb_pressed[NB_BUTTONS];
extern int press_step[NB_BUTTONS];
extern int mx_press[NB_BUTTONS];
extern int need_clean;
extern int btn_state;

struct btn_repeatParam {
    int init_delay;
    int second_delay;
    int min_delay;
    int dec_value;
};



#endif
