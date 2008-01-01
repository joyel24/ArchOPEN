/*
*   kernel/core/ti_wdt.c
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
#include <sys_def/types.h>

#include <kernel/io.h>
#include <kernel/kernel.h>

#include <driver/hardware.h>
#include <driver/wdt.h>

void wdt_init(void)
{
#ifndef NO_WDT
  wdt_stop();
  arch_wdt_init();
  printk("[init] TI watchdog timer\n");
#endif
}

void wdt_setup(int prescaler,int divider,bool doreset)
{
  outw(prescaler,WDT_SCAL);
  outw(divider,WDT_DIV);

  outw(( inw(WDT_MODE)&(~WDT_MODE_ARM_RESET) ) | ( (doreset)?WDT_MODE_ARM_RESET:0 ),WDT_MODE);
  //WDT_EXT_RESET does not exist in DSC25
  //outw(( inw(WDT_EXT_RESET)&(~WDT_EXTRST_ARM_RESET) ) | ( (doreset)?WDT_EXTRST_ARM_RESET:0 ),WDT_EXT_RESET);
}

void wdt_stop(void)
{
#ifndef NO_WDT
  outw((inw(WDT_MODE)&(~WDT_MODE_ENABLED)),WDT_MODE);
#endif
}

void wdt_start(void)
{
#ifndef NO_WDT
  outw((inw(WDT_MODE)|WDT_MODE_ENABLED),WDT_MODE);
#endif
}

