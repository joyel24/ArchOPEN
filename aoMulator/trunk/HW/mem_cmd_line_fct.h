/*
*   mem_cmd_line_fct.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <cpu.h>

int do_cmd_dump_s(int argc,char ** argv)
{
    return mem->do_cmd_dump(argc,argv);
}


int mem_space::do_cmd_dump(int argc,char ** argv)
{
    uint32_t start_addr=0;
    int reg_num;
    int size = 0x200;
    if(argc>0)
    {
        if(argv[0][0] == 'R' || argv[0][0] == 'r')
        {
            argv[0]++;
            reg_num=my_atoi(argv[0]);
            if(reg_num>=0 && reg_num<=15)
            {
                start_addr=read_reg(reg_num);
            }
            else
            {
                printf("Wrong reg number: %d\n",reg_num);
                return 0;
            }
        }
        else
            start_addr = my_atoi(argv[0]);
        if(argc>1)
        {
            size = my_atoi(argv[1]);
        }
        my_print_mem(this,(char*)start_addr,size);
    }
    return 0;
}

int do_cmd_add_bk_mem_s(int argc,char ** argv)
{
    return mem->do_cmd_add_bk_mem(argc,argv);
}

int mem_space::do_cmd_add_bk_mem(int argc,char ** argv)
{
    /*get first arg*/
    uint32_t i=0;
    uint32_t size=0;
    bool do_del = false;
    if(argc>0)
    {
        if(argv[0][0] == '-')
        {
            do_del=true;
            argv[0]++;
        }
        i = my_atoi(argv[0]);
        if(do_del)
            del(bkpt,i);
        else
        {
            if(argc>1)
                size=my_atoi(argv[1]);
            add(bkpt,i,size);
        }
    }
    else
        print_bkpt_list(bkpt);
    return 0;
}

int do_cmd_dumpsdram_s(int argc,char ** argv)
{
    FILE * fd = fopen("sd.out", "wb");
    int size;
    if (!fd)
            printf("Error: Opening sd.out\n");
    else
    {
        size=fwrite(mem->sd->mem, 1, 0x01000000,fd );
        printf("Wrote %d to sd.out\n",size);
        fclose(fd);
    }
    
    return 0;
}

int do_cmd_dumpiram_s(int argc,char ** argv)
{
    FILE * fd = fopen("iram.out", "wb");
    int size;
    if (!fd)
            printf("Error: Opening iram.out\n");
    else
    {
        size=fwrite(mem->iram->mem, 1, 0x08000-0x4,fd );
        printf("Wrote %d to iram.out\n",size);
        fclose(fd);
    }
    
    return 0;
}

int do_cmd_dump_dsp_s(int argc,char ** argv)
{
    return mem->do_cmd_dump_dsp();
}

int mem_space::do_cmd_dump_dsp(void)
{
    FILE * fd = fopen("dsp.out", "wb");
    int size;
    if (!fd)
            printf("Error: Opening dsp.out\n");
    else
    {
        size=fwrite(dsp_mem->mem, 1, 0x10000,fd );
        printf("Wrote %d to dsp.out\n",size);
        fclose(fd);
    }
    return 0;
}



int do_cmd_write_mem_s(int argc,char ** argv)
{
    return mem->do_cmd_write_mem(argc,argv);
}

int mem_space::do_cmd_write_mem(int argc,char ** argv)
{
    printf("argc=%d\n",argc);
    if(argc==3)
    {
        uint32_t addr = my_atoi(argv[0]);
        uint32_t val = my_atoi(argv[1]);
        int size = my_atoi(argv[2]);
        printf("[wmem] about to write @%x val=%x size=%x\n",addr,val,size);
        this->write(addr,val,size);
    }
    else
        printf("[wmem] error: args= addr value\n");
    return 0;
}

int do_cmd_initMem_s(int argc, char ** arg)
{
    printf("Init mem\n");
    mem->write(0x95faf6,0x1c00,2);
    mem->write(0x93283c,0x1c07,2);
    return 0;
}

void init_mem_static_fct(mem_space * mem_obj)
{
    mem = mem_obj;
    add_cmd_fct("dumpdsp",do_cmd_dump_dsp_s,"Dump dsp mem to file dsp.out");
    add_cmd_fct("dump",do_cmd_dump_s,"Dump mem to screen");
    add_cmd_fct("bkm",do_cmd_add_bk_mem_s,"Manage mem bkpt");
    add_cmd_fct("wmem",do_cmd_write_mem_s,"Write to mem: addr value size(1=byte,2,4)");
    add_cmd_fct("imem",do_cmd_initMem_s,"Special init needed for Av4 running archos FW");
    add_cmd_fct("dumpsdram",do_cmd_dumpsdram_s,"Dump sdram mem to file sd.out");
    add_cmd_fct("dumpiram",do_cmd_dumpiram_s,"Dump iram mem to file sd.out");
}
