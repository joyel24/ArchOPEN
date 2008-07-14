/*
*   kernel/core/swi.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <lib/string.h>

#include <kernel/kernel.h>
#include <kernel/swi.h>

#include "api_fct_list.h"

void badFctName(void)
{
    printk("BAD fct \n");
}
    
unsigned int swi_handler (
	unsigned long nParam1,
	unsigned long nParam2,
	unsigned long nParam3,        
	unsigned long nCmd)
{
    if(nCmd == nAPI_INIAPI)
    {
        int i=0;
        char * name = (char*)nParam1;
        while(fct_array[i].name!=NULL)
        {
            if(!strcmp(fct_array[i].name,name))
                return fct_array[i].ptr;
            i++;
        }
        printk("fct %s not found\n",name);
        return (unsigned int)badFctName;
    }
    return 0;
}
