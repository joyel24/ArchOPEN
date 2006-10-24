/*
*   arch mem_space.cpp
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

#include <HW/mem_space.h>

void mem_space::arch_init(void)
{
    char * str="DeBuGuNlOcKeD_42";
    for(int i=0;*(str+i);i++)
        write((0x107FF0+i),(char)*(str+i),1);
}
