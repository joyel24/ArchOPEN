/*
*   kernel/cmd_line.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

/***************************************
 * Main code for a command line based on
 * UART communication
 * command line function are define in the
 * file cmd_line_fct.c
 * this code listen on UART CMD_LINE_UART
 * defined in the file medios/include/target/chip/arch/arch_def.h
 * => specific to each arch
 ****************************************/

#include <lib/string.h>

#include <kernel/kernel.h>
#include <kernel/irq.h>
#include <kernel/malloc.h>
#include <kernel/cmd_line.h>

#include <driver/uart.h>
#include <driver/hardware.h>

#include <fs/stdfs.h>

#define MAX_CMD_LEN     100  /* lenght of the string that is read for one line of cmd*/
#define MAX_ARGS        10   /* size of the list of args for commands */

/* displayed prompt => after CR on UART or completion of a cmd*/
#define MEDIOS_PROMPT "mediOS"

/* list of cmd defined in cmd_line_fct.c*/
extern struct cmd_line_s cmd_tab[];

/* helper MACRO used for cmd line process*/

/* cleans head of the string from the unwanted space*/
#define RM_HEAD_SPC(ptr)      ({while(*ptr && *ptr==' ') ptr++;})
/* returns a ptr on next tocken, ie skip space*/
#define FIND_NXT_TOKEN(ptr)   ({char * __v=ptr; while(*__v && *__v!=' ') __v++; __v;})
/* find the cmd structure for a given cmd name */
#define FIND_CMD_LINE(PTR,CMD_TAB)                                                                 \
        ({                                                                                 \
            int __v=0;                                                                     \
            struct cmd_line_s * __w=NULL;                                                  \
            for(__v=0;CMD_TAB[__v].cmd!=NULL && strcmp(CMD_TAB[__v].cmd,PTR);__v++) ;    \
            if(CMD_TAB[__v].cmd!=NULL) __w=&CMD_TAB[__v];                                 \
            __w;                                                                           \
        })

unsigned char * cur_cmd;
int cur_pos;

unsigned char ** arg_list;

THREAD_INFO * cmdLineThread;

/* set to 1 if init went properly, 0 otherwise
 * this is used by enable/disable function in
 * order to know if something has to be done */
int cmd_line_init_ok;

char path[MAX_PATH];

/*******************************
 * INT handler on UART event
 * => enable cmd_line thread which should
 * be in an idle state
 *******************************/
void cmd_line_INT(int irq_num,struct pt_regs * regs)
{
    if(cmdLineThread->state!=THREAD_STATE_ENABLE)
        cmdLineThread->state=THREAD_STATE_ENABLE;
}

/*******************************
 * Main function that runs in a seperate
 * thread
 * It reads all available data from CMD_LINE_UART
 * when no more data are in uart buffer, it goes to idle state
 * it reads char and compose the cmd line string
 * when end of line is reached => call process_cmd
 *******************************/

void cmd_line_thread(void)
{
    unsigned char c;

    /* infinite loop :we are in a seperate thread => we should never exit from it */
    while(1)
    {
        while(uart_in(&c,CMD_LINE_UART)) /* read all available char from uart IN buffer */
        {
            if(c=='\n' || c=='\r')               /* end of line */
            {
                cur_cmd[cur_pos++]='\0';         /* => add \0 to end the string */
                process_cmd();                   /* call command process function */
                continue;
            }

            if(c>=0x20 && c<0xFF && c!=0x7F) /* we have a std char => append to cmd string */
            {
                    cur_cmd[cur_pos++]=c;
                    printk("%c",c); /* local echo */
            }
            else                                       /* special chars */
            {
                switch(c)
                {
                    case 0x1B:                      /* ESC */
                        if(uart_in(&c,CMD_LINE_UART))
                        {
                            if(c=='[')
                            {
                                if(uart_in(&c,CMD_LINE_UART))
                                {
                                    /*switch(c)
                                    {
                                        case 0x41:
                                            printk("\nUP\n");
                                            break;
                                        case 0x42:
                                            printk("\nDOWN\n");
                                            break;
                                        case 0x43:
                                            printk("\nRIGHT\n");
                                            break;
                                        case 0x44:
                                            printk("\nLEFT\n");
                                            break;
                                    }*/
                                }
                            }
                        }
                        break;
                    case 0x08:                      /* bckspc */
                    case 0x7F:                      /* del */
                        if(cur_pos>0)
                        {
                            cur_pos--;              /* removing one char from string */
                            cur_cmd[cur_pos]='\0';
                            fs_pwd(path);
                            printk("\n%s-%s>%s",MEDIOS_PROMPT,path,cur_cmd);
                        }
                        break;
                    default:
                        break;
                }
            }

            if(cur_pos>MAX_CMD_LEN)            /* can't add more chars => reset everything */
            {
                cur_cmd[0]='\0';
                cur_pos=0;
                printk("\nLine too long\n%s-%s>",MEDIOS_PROMPT,path);
            }
        }
        THREAD_PAUSE();
    }
}

/********************************************
 * Process the string cur_cmd
 * Try to find the fction associated with the command
 * Parse the args, check if the number of args is OK
 * launch fction with the args
 * *****************************************/

void process_cmd(void)
{
    int nb_args=0;
    struct cmd_line_s * cmd_line;
    unsigned  char * ptr;

    if(cur_pos==1) /* empty cur_cmd => only redraw prompt*/
    {
        fs_pwd(path);
        printk("\n%s-%s> ",MEDIOS_PROMPT,path);
    }
    else
    {
        /* processing the cmd */
        RM_HEAD_SPC(cur_cmd);

        /* let's find the cmd name */
        ptr=FIND_NXT_TOKEN(cur_cmd);

        if(!*ptr)   /* cmd with no args */
        {
            cmd_line=FIND_CMD_LINE(cur_cmd,cmd_tab);
            if(!cmd_line)
            {
                printk("Unknown command: %s\nType help to have the list of command\n",cur_cmd);
                cur_cmd[0]='\0';
                cur_pos=0;
                fs_pwd(path);
                printk("%s-%s> ",MEDIOS_PROMPT,path);
                return;
            }
        }
        else
        {
            *ptr='\0';
            cmd_line=FIND_CMD_LINE(cur_cmd,cmd_tab);
            if(!cmd_line)
            {
                printk("Unknown command: %s\nType help to have the list of command\n",cur_cmd);
                cur_cmd[0]='\0';
                cur_pos=0;
                fs_pwd(path);
                printk("%s-%s> ",MEDIOS_PROMPT,path);
                return;
            }

            cur_cmd = ptr+1;
            /* parse args */
            while(*cur_cmd)
            {
                RM_HEAD_SPC(cur_cmd);
                if(*cur_cmd)
                {
                    arg_list[nb_args]=cur_cmd;
                    nb_args++;
                    cur_cmd=FIND_NXT_TOKEN(cur_cmd);
                    if(*cur_cmd)
                    {
                        *cur_cmd='\0';
                        cur_cmd++;
                    }
                }
            }
        }

        /* launch the cmd if we have enough params */
        if(nb_args>=cmd_line->nb_args)
        {
            cmd_line->cmd_action(arg_list);
        }
        else
            printk("%s need more args:\n%s-%s\n",cmd_line->cmd,MEDIOS_PROMPT,cmd_line->help_str);

        /* put back the prompt */
        fs_pwd(path);
        printk("%s-%s> ",MEDIOS_PROMPT,path);
    }

    /* Ready to get a new cmd */
    cur_cmd[0]='\0';
    cur_pos=0;

}

/********************************************
 * Init the command line
 * -> variable : cur_cmd, arg_list
 * -> launch the thread
 * -> enable the cmd line
 * *****************************************/

void init_cmd_line(void)
{
    int pid;
    cur_pos=0;
    cmd_line_init_ok=0;

    arg_list = (unsigned char**)kmalloc(sizeof(unsigned char*)*MAX_ARGS);
    if(!arg_list)
    {
        printk("[init] cmd line, can't allocate memory for args\n");
        return;
    }

    cur_cmd = (unsigned char*)kmalloc(sizeof(unsigned char)*MAX_CMD_LEN);
    if(!cur_cmd)
    {
        kfree(arg_list);
        printk("[init] cmd line, can't allocate memory for args\n");
        return;
    }

    cur_cmd[0]='\0';

    /* launch cmd line thread with the function cmd_line_thread */
    pid=thread_startFct(&cmdLineThread,cmd_line_thread,"cmd line",
                         THREAD_STATE_DISABLE,PRIO_HIGH,THREAD_USE_OTHER_STACK);

    if(pid<0) /* couldn't create the thread */
    {
        printk("Error creating cmd line thread: %d\n",-pid);
        kfree(cur_cmd);
        kfree(arg_list);
        return;
    }

    cmd_line_init_ok=1; /* init is ok => save the status */
    cmd_line_enable();  /* enable the cmd line system */
    printk("[init] cmd line (thread pid:%d)\n",pid);
}

void cmd_line_enable(void)
{
    char c;

    if(cmd_line_init_ok) /* was init ok ? */
    {
        uart_need(CMD_LINE_UART);              /* request UART */
        uart_changeSpeed(115200,CMD_LINE_UART);

        irq_changeHandler(UART_IRQ_NUM(CMD_LINE_UART),cmd_line_INT);
            /* clear uart buffer in */
        while(uart_in(&c,CMD_LINE_UART)) /*nothing*/;
        irq_enable(UART_IRQ_NUM(CMD_LINE_UART));
    }
}

void cmd_line_disable(void)
{
    if(cmd_line_init_ok) /* was init ok ? */
    {
        uart_restoreIrqHandler(UART_IRQ_NUM(CMD_LINE_UART)); /* put back original irq handler */
        irq_disable(UART_IRQ_NUM(CMD_LINE_UART));
    }
}

