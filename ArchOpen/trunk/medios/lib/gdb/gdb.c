/*
*   lib/gdb.c
*
*   MediOS project
*   Copyright (c) 2008 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#include <kernel/kernel.h>
#include <kernel/thread.h>
#include <kernel/swi.h>

#include <lib/string.h>
#include <lib/gdb.h>
#include <lib/breakpoints.h>
#include <lib/opcode_decode.h>

#include <driver/buttons.h>
#include <driver/uart.h>
#include <driver/cache.h>

#define BUFF_SIZE 1024

#define GDB_UART CMD_LINE_UART

#define GDB_DO_CHECK 1
#define GDB_NO_CHECK 0

#define GDB_NO_CANCEL 0
#define GDB_WITH_CANCEL 1

static char inBuff[BUFF_SIZE];
static char outBuff[BUFF_SIZE];

static const char hexchars[] = "0123456789abcdef";

static int debugActive;

static int blockingUartPoll=0;

/** The pool of breakpoint descriptors */
struct breakpoint_descr breakpoint_pool[MAX_BREAKPOINTS];
/** the pool of free breakpoint descriptors */
struct breakpoint_descr *free_breakpts;
/** the active breakpoint descriptors */
struct breakpoint_descr *active_breakpts;
/** the active stepping breakpoint descriptors */
struct breakpoint_descr *stepping_breakpts;
/** the breakpoint descriptors of breakpoint disabled by stepping */
struct breakpoint_descr *disabled_breakpts;

/****************************/
/* UART Interuption handler */
/****************************/

void gbd_uartInt(int irq_num,struct pt_regs * regs)
{
    //gdb_getPacket();
}

/**************************/
/* Hex managing functions */
/**************************/

int gdb_hex(char ch) 
{
    if ((ch >= 'a') && (ch <= 'f'))
        return ch - 'a' + 10;
    if ((ch >= '0') && (ch <= '9'))
        return ch - '0';
    if ((ch >= 'A') && (ch <= 'F'))
        return ch - 'A' + 10;
    return -1;
}

void gdb_hexByte(char *s, int byte) 
{
    s[0] = hexchars[(byte >> 4) & 0xf];
    s[1] = hexchars[byte & 0xf];
}

int gdb_getHexByte(char *s)
{
    return (gdb_hex(s[0]) << 4) + gdb_hex(s[1]);
}

void gdb_hexWord(char *s, unsigned long val)
{
    int i;
    for (i = 0; i < 4; i++)
        gdb_hexByte(s + i * 2, (val >> (i * 8)) & 0xff);
}

unsigned long gdb_getHexWord(char *s) 
{
    int i;
    unsigned long res = 0;
    for (i = 3; i >= 0; i--)
        res = (res << 8) + gdb_getHexByte(s + i * 2);
    return res;
}

/*********************/
/* Packet read/write */
/*********************/

int gdb_getPacket(void)
{
    char c;
    int stop;
    int escaped;
    int chk,count,chk2;

    /* read all available char from uart IN buffer */
  
gdbPacketRestart:
    stop=0;
    
    while(1) /*wait for start char*/
    {
        if(!blockingUartPoll && (arch_btn_readHardware() & BTMASK_F1)) return 0;
        if(!uart_in(&c,GDB_UART)) continue;
        if(c=='$') break;
    }
        
    chk=count=escaped=0;
    
    while(!stop)
    {        
        if(!blockingUartPoll && (arch_btn_readHardware() & BTMASK_F1)) return 0;
        if(!uart_in(&c,GDB_UART)) continue;
        if(count>=BUFF_SIZE)
        {
            uart_out('-',GDB_UART);
            goto gdbPacketRestart;
        }
        
        if(!escaped)
        {
            switch(c)
            {
                case '$':
                    chk=count=0;
                    break;
                case '#':
                    stop=1;
                    break;
                case 0x7d:
                    chk += c;
                    escaped=1;
                    break;
                default:
                    chk += c;
                    inBuff[count] = c;
                    count++;
                    break;
            }  
        }
        else
        {
            escaped=0;
            chk += c;
            inBuff[count] = c ^ 0x20;
            count++;
        }
    }
    
    inBuff[count]='\0';
    
    while(!uart_in(&c,GDB_UART)) if(!blockingUartPoll && (arch_btn_readHardware() & BTMASK_F1)) return 0;    
    chk2=(gdb_hex(c) << 4);
    while(!uart_in(&c,GDB_UART)) if(!blockingUartPoll && (arch_btn_readHardware() & BTMASK_F1)) return 0;    
    chk2 += gdb_hex(c);
    if ((chk & 0xff) != chk2)
    {
        uart_out('-',GDB_UART);
        DEBUG_GDB("UART|Bad CRC: %s (%x instead of %x)\n",inBuff,chk&0xff,chk2);
        goto gdbPacketRestart;
    }
    else
    {
        uart_out('+',GDB_UART);
        DEBUG_GDB("UART|New cmd:%s\n",inBuff);
    }
    return 1;
}

void gdb_putPacket(int check)
{
    int i, checksum;
    char c;
    char tmp[4];

    do {
        uart_out('$',GDB_UART);

        checksum = 0;
        for (i = 0; outBuff[i]; i++)
            checksum +=outBuff[i];

        uart_outString(outBuff,GDB_UART);

        tmp[0] = '#';
        gdb_hexByte(tmp + 1, checksum & 0xff);
        tmp[3]='\0';
        uart_outString(tmp,GDB_UART);

        
        i=0;
        
        if(!blockingUartPoll) return;
        
        while(!uart_in(&c,GDB_UART))
        {
            if(!check && i>1000)
                return;
            i++;
        }

    } while (c != '+');
}

/******************************/
/* Gdb status cmd             */
/******************************/

void gdb_replyOK(void)
{
    outBuff[0]='O';
    outBuff[1]='K';
    outBuff[2]='\0';
    gdb_putPacket(GDB_DO_CHECK);
}
void gdb_replyKO(int errNum) 
{
    outBuff[0] = 'E';
    gdb_hexByte(outBuff + 1, errNum);
    outBuff[3] = '\0';
    gdb_putPacket(GDB_DO_CHECK);
}

/******************************/
/* Gdb commands               */
/******************************/

void gdb_sendSig(int sigNum)
{
    outBuff[0]='S';
    gdb_hexByte(outBuff + 1, sigNum);
    outBuff[3]='\0';
    gdb_putPacket(GDB_DO_CHECK);
}

void gdb_processBkpt(int bkptType)
{
    if(debugActive)
    {
        DEBUG_GDB("GDB already active\n");
        return;
    }
    
    if(bkptType==GDB_STEP_BKPT)
        threadCurrent->regs[PC]-=4;
    DEBUG_GDB("Gdb bkpt at %x\n",threadCurrent->regs[PC]);
    debugActive=1;
    gdb_sendSig(5);
    gdb_doCmd();
    debugActive=0;
    /*DEBUG_GDB("Gdb bkpt exit: PC=%x (%x) nxt PC (%x)\n",threadCurrent->regs[PC],
              *((unsigned int*)threadCurrent->regs[PC]),
              *((unsigned int*)threadCurrent->regs[PC]+1));*/
}

void gdb_getReg(void)
{
    int num;
    if (sscanf(inBuff+1, "%x", &num) != 1) {
        gdb_replyKO(0);
        return;
    }
    if(num>=0 && num<16)
    {
        gdb_hexWord(outBuff,threadCurrent->regs[num+1]);
        outBuff[8]='\0';
    }
    /*else if(num==15)
    {
        gdb_hexWord(outBuff,threadCurrent->regs[num+1]);
        outBuff[8]='\0';
    }*/
    else if(num==25) /*CPSR*/
    {
        gdb_hexWord(outBuff,threadCurrent->regs[0]);
        outBuff[8]='\0';
    }
    else
    {
        gdb_hexWord(outBuff,0);
        outBuff[8]='\0';
    }
    gdb_putPacket(GDB_DO_CHECK);
    DEBUG_GDB("Cmd get reg %x\n",num);
}

void gdb_setReg(void)
{
    int num, pos;
    unsigned long val;
    char * buff=inBuff+1;
    
    pos = -1;
    sscanf(buff, "%x=%n", &num, &pos);
    if (pos == -1)
    {
        gdb_replyKO(0);
        return;
    }
    
    val = gdb_getHexWord(buff+pos);
    if (num >= 0 && num < 16)
    {
        threadCurrent->regs[num+1]=val;
    }
    else if (num == 25)
    {
        threadCurrent->regs[0]=val;
    }
    gdb_replyOK();
    DEBUG_GDB("Cmd set reg %x to %x\n",num,val);
}

void gdb_getRegs(void)
{
    unsigned long * regs = threadCurrent->regs;
    int i;
    char * ptr=outBuff;
    
    for(i=0;i<16;i++)
    {
        gdb_hexWord(ptr,regs[i+1]); /*cpsr is reg[0]*/
        ptr+=8;
    }

    for (i = 0; i < 8; i++) {
        memset(ptr, 0, 16);
        ptr += 16;
    }

    gdb_hexWord(ptr, 0);
    ptr += 8;
    gdb_hexWord(ptr, regs[0]);
    ptr[8] = 0;
    gdb_putPacket(GDB_DO_CHECK);
    DEBUG_GDB("Cmd read regs\n");
}

void gdb_setRegs()
{
    char *buff;
    int i, len;

    len = strlen(inBuff);

    buff=inBuff+1;
    for (i = 0; i < 16 && len >= (i + 1) * 8; i++)
    {
        threadCurrent->regs[i+1]=gdb_getHexWord(buff);
        buff += 8;
    }

    if (len >= 16 * 8 + 8 * 16 + 2 * 8)
    {
        buff += 8 * 16 + 8;
        threadCurrent->regs[0]=gdb_getHexWord(buff);
    }
    gdb_replyOK();
    DEBUG_GDB("Cmd write regs\n");
}

void gdb_readMem(void)
{
    unsigned int addr,len;
    int i;
    if (sscanf(inBuff+1, "%lx,%lx", &addr, &len) != 2)
    {
        gdb_replyKO(0);
        return;
    }

    if (len > (BUFF_SIZE - 16) / 2) {
        gdb_replyKO(1);
        return;
    }

    for (i = 0; i < len; i++)
        gdb_hexByte(outBuff + i * 2, *((unsigned char *)(addr + i)));            

    outBuff[len * 2] = '\0';
    gdb_putPacket(GDB_DO_CHECK);
    DEBUG_GDB("Cmd read Mem (@%x, len=%x)\n",addr,len);
}

void gdb_writeMem(void)
{
    unsigned long addr, len, i;
    int pos;

    pos = -1;
    sscanf(inBuff+1, "%lx,%lx:%n", &addr, &len, &pos);
    if (pos == -1) {
        gdb_replyKO(0);
        return;
    }

    for (i = 0; i < len; i++)
        *((unsigned char *)(addr + i)) = gdb_getHexByte(inBuff + 1 + pos + i * 2);
    
    gdb_replyOK();
    DEBUG_GDB("Cmd write Mem at %x, len %x\n",addr,len);
    print_data(inBuff + 1 + pos,len*2);
}

void gdb_stepping(void)
{
    /* the address to insert the step breakpoint */
    unsigned long * regs = threadCurrent->regs;
    unsigned int ret_addr = regs[PC];
    int step_thumb_state = regs[CPSR] & 0x20;
    int thumb_state = regs[CPSR] & 0x20;
    unsigned int step_addr = ret_addr + (thumb_state ? 2 : 4);
    struct breakpoint_descr *step_bkpt;
    uint32_t reg_set[17];
    int i;

    DEBUG_GDB("Stepping (%s)\n",inBuff);

    /* if the next to be executed instruction will cause a branch the step
    * address must be set to the resulting destination address.
    */
    if ( instructionExecuted_opcode( ret_addr, thumb_state, regs[CPSR]))
    {
        uint32_t branch_addr;

        for ( i = 0; i < 15; i++) reg_set[i] = regs[i+1];

        if ( thumb_state) /* the PC is expected to be current address + 4 */
            reg_set[15] = ret_addr + 4;
        else  /* the PC is expected to be current address + 8 */
            reg_set[15] = ret_addr + 8;

        reg_set[16]=regs[0];
        
        if ( causeJump_opcode( ret_addr, &step_thumb_state, reg_set, &branch_addr))
        {
            step_addr = branch_addr;
            //DEBUG_GDB("Branch instruction, dest %08x, thumb %d\n", step_addr, step_thumb_state);
        }
    }
    else
        DEBUG_GDB("Instruction not executed\n");


    /* see if the step break already exists */
    step_bkpt = removeFromList_breakpoint( &stepping_breakpts,step_addr);
    if (step_bkpt)
        DEBUG_GDB("Step bk at %08x already exists\n", step_addr);
    else
        step_bkpt = removeHead_breakpoint( &free_breakpts);

    if (step_bkpt)
    {
        struct breakpoint_descr *disable_bkpt;    
        initDescr_breakpoint( step_bkpt, step_addr, step_thumb_state);

        addHead_breakpoint(&stepping_breakpts, step_bkpt);

        /* disable any breakpoints at the current return addr */
        disable_bkpt = removeFromList_breakpoint( &active_breakpts,ret_addr);
        if (disable_bkpt)
            addHead_breakpoint( &disabled_breakpts, disable_bkpt);
    }
    else
    {
    /*
    * no more breakpoint descriptors left, send a stop packet so the
    * debugger sits on the current instruction until the user does something.
    * That is about the best option available.
    */
        gdb_sendSig(5);
    }
}

void gdb_addBkpt(void)
{
    int type;
    unsigned int addr;
    int len;
    struct breakpoint_descr *active_bkpt;
    
    if(sscanf(inBuff+1, "%d,%lx,%d", &type,&addr, &len)!=3)
    {
        gdb_replyKO(0);
        return;
    }

    DEBUG_GDB("Bkpt ADD cmd addr=%x, type=%d, len=%x\n", addr,type,len);
    
    switch(type)
    {
        case 0:
            active_bkpt = removeFromList_breakpoint( &active_breakpts,addr);
            if (active_bkpt)
                DEBUG_GDB("SW bk at %08x already exists\n", addr);
            else
                active_bkpt = removeHead_breakpoint( &free_breakpts);

            if (active_bkpt)
            {
                initDescr_breakpoint( active_bkpt, addr, 0); /*forcing ARM mode */
                
                addHead_breakpoint(&active_breakpts, active_bkpt);
                gdb_replyOK();
            }
            else
            {
            /*
            * no more breakpoint descriptors left, send a stop packet so the
            * debugger sits on the current instruction until the user does something.
            * That is about the best option available.
            */
                gdb_replyKO(0);
            }
            break;
        case 1:
        case 2:
        case 3:
        case 4:
            outBuff[0]='\0';
            gdb_putPacket(GDB_DO_CHECK);
            break;
    }
}

void gdb_delBkpt(void)
{
    int type;
    unsigned int addr;
    int len;
    struct breakpoint_descr *rm_bkpt;
    
    if(sscanf(inBuff+1, "%d,%lx,%d", &type,&addr, &len)!=3)
    {
        gdb_replyKO(0);
        return;
    }

    DEBUG_GDB("Bkpt DEL cmd addr=%x, type=%d, len=%x\n", addr,type,len);
    
    switch(type)
    {
        case 0:
            rm_bkpt = removeFromList_breakpoint( &active_breakpts,addr);
            if (rm_bkpt)
                addHead_breakpoint(&free_breakpts, rm_bkpt);
            
            rm_bkpt = removeFromList_breakpoint( &disabled_breakpts,addr);
            if (rm_bkpt)
                addHead_breakpoint(&free_breakpts, rm_bkpt);
            gdb_replyOK();
            break;
        case 1:
        case 2:
        case 3:
        case 4:
            outBuff[0]='\0';
            gdb_putPacket(GDB_DO_CHECK);
            break;
    }
}


/******************************/
/* Gdb command parsing        */
/******************************/

void gdb_doCmd(void)
{
    int stop=0;

    struct breakpoint_descr *found;

    /* Remove all the breakpoints from memory so the
    * code is seen as written.   */
    removeAll_breakpoint(stepping_breakpts);
    removeAll_breakpoint(active_breakpts);

    /*DEBUG_GDB( "Flushing caches\n");
    CACHE_INVALIDATE(CACHE_CODE);
    CACHE_INVALIDATE(CACHE_DATA);*/

    /* put the disabled breakpoints back on the active list. */
    catLists_breakpoint(&active_breakpts,disabled_breakpts);
    disabled_breakpts = NULL;


   /* Remove any stepping breakpoints that match the current address */
    found = removeFromList_breakpoint( &stepping_breakpts, threadCurrent->regs[PC]);
    if (found) addHead_breakpoint( &free_breakpts, found);

    found = removeFromList_breakpoint( &active_breakpts, threadCurrent->regs[PC]);
    if (found) addHead_breakpoint( &disabled_breakpts, found);
    
    while(!stop)
    {
        if(!gdb_getPacket()) stop=1;
        
        switch(inBuff[0])
        {
            case 'p':
                gdb_getReg();                
                break;
            case 'P':
                gdb_setReg();
                break;
            case 'g':
                gdb_getRegs();
                break;
            case 'G':
                gdb_setRegs();
                break;
            case 'm':
                gdb_readMem();    
                break;
            case 'M':
                gdb_writeMem();
                break;
            case 'D':
                DEBUG_GDB("Cmd detach\n");
                stop=1;
                break;
            case 'c':   
                DEBUG_GDB("Cmd continue\n");
                stop=1;
                break;
            case 's':
                gdb_stepping();
                stop=1;
                break;
            case '?':
                gdb_sendSig(5);
                DEBUG_GDB("Cmd last Sig\n");
                break;
            case 'Z':
                gdb_addBkpt();
                break;
            case 'z':
                gdb_delBkpt();
                break;
            default:
                outBuff[0]='\0';
                gdb_putPacket(GDB_DO_CHECK);
                DEBUG_GDB("UKN cmd: %s\n",inBuff);
                break;
        }        
    }
    DEBUG_GDB( "Inserting active breakpoints\n");
    insertAll_breakpoint( active_breakpts);
    DEBUG_GDB( "Inserting stepping breakpoints\n");
    insertAll_breakpoint( stepping_breakpts);

    DEBUG_GDB( "Flushing caches\n");
    CACHE_INVALIDATE(CACHE_CODE);
    CACHE_INVALIDATE(CACHE_DATA);
    //regs[PC]=ret_addr;
}

/****************************/
/*  Printk like debug msg   */
/****************************/

void gdb_outString(char *msg)
{
    int i;
    if(debugActive) return;
    outBuff[0] = 'O';
    i = 1;
    while (*msg && i + 2 <= BUFF_SIZE - 1)
    {
        gdb_hexByte(outBuff + i, *msg++);
        i += 2;
    }
    outBuff[i] = 0;
    gdb_putPacket(GDB_NO_CHECK);
}

/**************************/
/*       Initialisation   */
/**************************/

void gdb_init(void)
{
    int i;
    uart_need(GDB_UART);              /* request UART */
    uart_changeSpeed(115200,GDB_UART);
    blockingUartPoll=0;
    debugActive=1;

    for ( i = 0; i < MAX_BREAKPOINTS; i++)
    {
        breakpoint_pool[i].next = NULL;
        if ( i > 0) breakpoint_pool[i - 1].next = &breakpoint_pool[i];
    }
    free_breakpts = &breakpoint_pool[0];
    stepping_breakpts = NULL;
    active_breakpts = NULL;
    disabled_breakpts = NULL;
    
    printk("Starting GDB first\nWaiting for its connexion - press F1 key to skip\n");
    gdb_sendSig(5);
    
    gdb_doCmd();

    debugActive=0;
    blockingUartPoll=1;
    //irq_changeHandler(UART_IRQ_NUM(GDB_UART),gbd_uartInt);
        /* clear uart buffer in */
    //while(uart_in(&c,GDB_UART)) /*nothing*/;
    //irq_enable(UART_IRQ_NUM(GDB_UART));
}
