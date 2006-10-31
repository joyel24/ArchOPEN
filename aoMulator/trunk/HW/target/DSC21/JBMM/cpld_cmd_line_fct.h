/*
*   cpld_cmd_line_fct.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

HW_cpld * cpld_obj;

/*int do_cmd_ch_module_s(int argc,char ** argv)
{
    if(argc>0)
    {
        int num = my_atoi(argv[0]);
        cpld_obj->setModule(num&0xF);
    }
    return 0;
}*/

int do_cmd_cpld_state_s(int argc,char ** argv)
{
    cpld_obj->cpld_print_state();
    return 0;
}


void init_cpld_static_fct(HW_cpld * cpld)
{
    cpld_obj = cpld;

    //add_cmd_fct("ch_module",do_cmd_ch_module_s,"Change the module type (0xF) => no module");
    add_cmd_fct("cpld_state",do_cmd_cpld_state_s,"Print current state");
}
