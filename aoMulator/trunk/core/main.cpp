/*
*   main.cpp
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

#include <signal.h>

#include <emu.h>
#include <HW/mem_space.h>
#include <cpu.h>
#include <core/cmd_line.h>

#include <HW/HW_btn.h>
#include <HW/HW_TI.h>
#include <HW/HW_connection.h>
#include <HW/i2c_gpio.h>

HW_connection * hw_connection;
HW_btn * btn;
i2c_master * i2c;
extern mem_space * mem;

enum cmd_type {NO_CMD=0x0,F_SDRM='s',F_FLASH='f',C_HELP='h',C_DECODE_THUMB='t',C_DECODE_ARM='a'};

void usage(char * name)
{
    printf("Usage: %s -f name|-s name|-h\n-f : file to be loaded in flash\n-s : file to be loaded in sdram\n-h: help\n",name);
    printf("Decode one instruction:\n-t thumb instruction\n-a arm instruction\n");
    exit(0);
}

void cmd_line(void);

void signal_handler( int num ) {
   if(num == SIGINT)
   {
       sigint();
   }
}

void arch_init_main(void);

int main(int argc, char* argv[])
{
    int i;
    char * sdram_file = NULL;
    char * flash_file = NULL;

    unsigned int decode_inst= 0;
    int decode_thumb = 0;
    int decode_arm = 0;

    mem_space * mem;

    int cur_cmd = NO_CMD;

    for(i=1;i<argc;i++)
    {
        printf("cur_cmd= %c\n",cur_cmd==0?' ':cur_cmd);
        if(cur_cmd!=NO_CMD)
        {
            switch(cur_cmd)
            {
                case F_SDRM:
                    sdram_file = argv[i];
                    break;
                case F_FLASH:
                    flash_file = argv[i];
                    break;
                case C_DECODE_THUMB:
                case C_DECODE_ARM:
                    decode_inst = my_atoi(argv[i]);
            }
            cur_cmd = NO_CMD;
        }
        else
        {
            if(argv[i][0] == '-')
            {
                switch(argv[i][1])
                {
                    case F_SDRM:
                    case F_FLASH:
                        cur_cmd = argv[i][1];
                        break;
                    case C_HELP:
                        usage(argv[0]);
                    case C_DECODE_THUMB:
                        decode_thumb = 1;
                        decode_arm = 0;
                        cur_cmd = argv[i][1];
                        break;
                    case C_DECODE_ARM:
                        decode_thumb = 0;
                        decode_arm = 1;
                        cur_cmd = argv[i][1];
                        break;
                    default:
                        cur_cmd = NO_CMD;
                }
            }
        }
    }

    init_cmd_line();

    mem = new mem_space(flash_file,sdram_file);

    btn=new HW_btn();

    hw_connection=new HW_connection();

    i2c=new i2c_master(mem->hw_TI->gpio);

    arch_init_main();



    i2c->print_i2c_list();

    init_cpu();

    signal(SIGINT,&signal_handler);

    if(decode_arm)
    {
            printf("decoding ARM: %x\n",decode_inst);
            doARM(decode_inst);
            delete(mem);
            exit(0);
    }

    if(decode_thumb)
    {
            printf("decoding ARM: %x\n",decode_inst);
            doThumb(decode_inst);
            delete(mem);
            exit(0);
    }



    go(START_ADDR,STACK_INIT);

    delete(mem);
}




