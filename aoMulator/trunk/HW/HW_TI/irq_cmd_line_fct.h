/*
*   irq_cmd_line_fct.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <core/cmd_line.h>

HW_IRQ * irq_obj;

int do_int_cmd_s(int argc,char ** argv)
{
    if(argc>0)
    {
        int num = my_atoi(argv[0]);
        irq_obj->do_INT(num);
    }
    return 0;
}

void init_irq_static_fct(HW_IRQ * irq)
{
    irq_obj = irq;
    add_cmd_fct("fiq",do_int_cmd_s,"Emulates irq/fiq int");
}
