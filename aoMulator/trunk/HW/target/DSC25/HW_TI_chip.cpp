/*
*   HW_TI.cpp
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/
#include <stdlib.h>
#include <stdio.h>

#include <emu.h>

#include <HW/HW_TI.h>
#include <HW/HW_null.h>
#include <target/chip/HW_MemCfg.h>

void HW_TI::chip_init(void)
{
     add_item(new HW_MemCfg());
     //add_item(new HW_null(0x30a1a,0x30a1a+0x2,"Internal WDT",0));
}


