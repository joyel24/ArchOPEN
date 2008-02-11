/*
*   apps/avlo/avlo.c
*
*   Avlo a loader for Jbmm, av1xx and av3xx
*   ArchOpen Project
*   Copyright (c) 2007 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/kernel.h>
#include <kernel/evt.h>
#include <kernel/malloc.h>
#include <kernel/delay.h>

#include <driver/ata.h>
#include <driver/batDc.h>
#include <driver/lcd.h>
#include <driver/osd.h>
#include <driver/usb_fw.h>

#include <gfx/graphics.h>

#include <init/exit.h>

#include <lib/string.h>

#include <fs/stdfs.h>
#include <fs/vfs.h>

#include <sys_def/colordef.h>
#include <sys_def/font.h>

#define AVLO_C

#include <parse_cfg.h>

#include <avlo.h>
#include <avlo_cfg.h>

#include <default_cfg.h>

#define MAX_OFF_PRESS    500
#define MAX_REPEAT       6000
#define MAX_DELAY        25000

#define CUSTOM_COLOR_START  230

#include <avlo_colors.h>

#define NO_TIME_OUT    0
#define WITH_TIME_OUT  1

#define BAT_LOOP_SIZE 10000

void (*binCaller)(void)=(void (*)(void))SDRAM_ARCHOS_START;

struct config_image cfg[MAX_CFG];
struct config_gene cfgG;

int usbstate,usbenable=0,cleanUSBMsg=0;
int chkdefault,cnt=0,cursorPos=0,delayCnt;
int errNoDefault=0,cntNoDefault=0,stateNoDefault=0;
int nbOff=0;
int maxRepeat;
char tmp_txt[100];

int evt_handler;
//int evt;

struct avlo_cfg * ptr_cfg;

int app_main(int argc,char** argv)
{
    int ret,nbCfg,redraw;
    int i,dd;
    int bat_loop=0;
    void * ptr;
    int evt;
           
    printk("In main AVLO\n");        
    
    gfx_openGraphics();
    
    //gfx_planeSetState(VID1,0xF0);
    //gfx_planeSetSize(VID1,SCREEN_WIDTH,SCREEN_HEIGHT,32,GFX_SMODE_STD);
    
    /* load image ini_graphics((unsigned int)bg_img); */
    /*
#ifdef HAVE_IMG   
    showPlane(VID1);
#endif
    showPlane(BMAP1);
 */    
        
    evt_handler=evt_getHandler(BTN_CLASS);
    
loopErr:
    ptr_cfg = &default_cfg;
    usbenable=0;cleanUSBMsg=0;cnt=0;cursorPos=0;errNoDefault=0;cntNoDefault=0;stateNoDefault=0;nbOff=0;
    
    gfx_clearScreen(COLOR_TSP);
    gfx_fontSet(TXT_FONT);
    
    printIniLevel(0);
    
    //msgbox: HD_init_string();
    
       
    /**************************/
    printk("ok\n");
    
    printIniLevel(1);

    //ptr_cfg = &default_cfg;
    
    // msgbox: readingConfString();
    printk("about to open avlo.cfg\n");
    
    if((ret=file_open("/avlo.cfg"))<0)
    {
        printk("Can't open avlo.cfg\n");
        doFault(0);
        goto loopErr;        
    }

    printk("avlo.cfg open\n");
    
    printIniLevel(2);

    if((nbCfg=do_parse(cfg,&cfgG))<0)
    {
        printk("Can't parse avlo.cfg\n");
        doFault(1);
        goto loopErr;
    }

    printk("avlo.cfg parsed\n");
    
    file_close();
    
    chkdefault=(!cfgG.defBin[0]==0);

    if(cfgG.repeat==0)
        maxRepeat=MAX_REPEAT;
    else
        maxRepeat=cfgG.repeat;

    if(cfgG.timeOut==0)
        delayCnt=MAX_DELAY;
    else
        delayCnt=cfgG.timeOut;

    printk("Gal opt:\n-default=%s (=>%s),\n-repeat=%d,\n-time out=%d\n-bg img=%s\n",cfgG.defBin,
        chkdefault?"has default":"no default",maxRepeat,delayCnt,cfgG.bg_img[0]!=0?cfgG.bg_img:"NO IMG");
#ifdef LOAD_BG
    if(cfgG.bg_img[0] != 0)
    {
        int fd_img = open(cfgG.bg_img,O_RDONLY);
        int size = SCREEN_REAL_WIDTH*2*SCREEN_HEIGHT;
        printk("Loading img\n");
        if(fd_img<0)
            printk("[WARNING] File not found: %s\n",cfgG.bg_img);
        else
        {
            ptr=gfx_planeGetBufferOffset(VID1);
            int nb_read = read(fd_img,ptr,size);
            printk("Read: %x/%x bytes of image\n",nb_read,size);
            if(nb_read==size)
            {
                gfx_planeSetState(BMAP1,OSD_BMAP_1_CFG 
                        | OSD_BITMAP_TRSP_ENABLE_BIT | OSD_BITMAP_BLEND_FACTOR(0));
                gfx_planeShow(BMAP1); /* needed to make new state val real */
                gfx_planeShow(VID1);
                /* changing cfg ptr */
                ptr_cfg = (struct avlo_cfg*)malloc(sizeof(struct avlo_cfg));
                if(!ptr_cfg)
                {
                    printk("[WARNING] can't malloc avlo_cfg struct\n");
                    ptr_cfg = &default_cfg;   
                }
                else
                {
                    nb_read = read(fd_img,ptr_cfg,sizeof(struct avlo_cfg));
                    printk("Read: %x/%x bytes of cfg\n",nb_read,sizeof(struct avlo_cfg));
                    if(nb_read!=sizeof(struct avlo_cfg))
                    {
                        printk("[WARNING] can't load avlo_cfg from file\n");
                        ptr_cfg = &default_cfg;  
                    }
                }
                init_colors();
            }
            else
                printk("[WARNING] only read %d/%d from %s\n",nb_read,size,cfgG.bg_img);
            close(fd_img);
        }
    }
    else
    {
        printk("[WARNING] No img defined\n");
    }
#else
    ptr=gfx_planeGetBufferOffset(VID1);
    for(i=0;i<SCREEN_REAL_WIDTH*4*SCREEN_HEIGHT;i++)
        *((int*)ptr+i)=COLOR32_BLACK;
    gfx_planeShow(VID1);
    gfx_planeSetState(BMAP1,OSD_BMAP_1_CFG 
                        | OSD_BITMAP_TRSP_ENABLE_BIT | OSD_BITMAP_BLEND_FACTOR(0));
    gfx_planeShow(BMAP1); /* needed to make new state val real */
#endif
    printIniLevel(3);

    usbstate=-1;
    
    redraw=1;

    bat_loop=BAT_LOOP_SIZE ;
    
    evt_purgeHandler(evt_handler);
    
    printk("Handler: %d\n",evt_handler);
    
    while(1)
    {
        if(redraw)
        {
            drawMenu(nbCfg);
            redraw=0;
        }

        affUSB();

        /* read bat */
        bat_loop++;
        if(bat_loop>BAT_LOOP_SIZE)
        {
            avlo_drawBat();
            bat_loop = 0;
        }
        
        evt=evt_getStatus(evt_handler);
        
        if(processDefault(evt,nbCfg)<0)
            goto loopErr;
        
        if(evt==NO_EVENT)
            continue;
/*
        printErr(key);
*/
        /*NOTE: need something for off */
        
        if(usbenable)
        {
            if(evt==BTN_F3)
            {
                printk("disable usb\n");
                usbenable=0;
                disableUsbFw();
                mdelay(10);
                ata_hardReset(HD_DISK);
                vfs_init();
                disk_addAll();  
                redraw=1;
                cleanUSBMsg=1;
                //waitKeyReleased(NO_TIME_OUT);
                for(i=0;i<0x10000;i++); /* Nothing */
                evt_purgeHandler(evt_handler);
                goto loopErr;
            }
        }
        else
        {
            switch(evt)
            {
                case BTN_DOWN:
                    moveCursor(+1,nbCfg);
                    break;
                case BTN_UP:
                    moveCursor(-1,nbCfg);
                    break;
                case BTN_ON:
                case BTN_RIGHT:
                {
                    char * ext=strrchr(cfg[cursorPos].image,'.')+1;
                    int launch=0;
                    printk("loading: %s ext:%s\n",cfg[cursorPos].image,ext);
                    if(toLower(ext[0])=='a' && toLower(ext[1])=='j' && toLower(ext[2])=='z' && ext[3]=='\0'
                            //&& cfgG.key[0]!=0
                                    //&& loadCJBM(cfg[cursorPos].image,cfgG.key))
                                    && fastLoadCJBM(cfg[cursorPos].image))
                        launch=1;
                    else if (loadFile(cfg[cursorPos].image,(char*)SDRAM_START,1))
                        launch=1;
    
                    if(launch)
                    {
                        printk("File loaded, now launching\n");
                        printk("append=%s\n",cfg[cursorPos].append);
                        ata_StopHD(HD_DISK);
                        if(SDRAM_START!=SDRAM_ARCHOS_START)
                            resetMem(SDRAM_ARCHOS_START,SDRAM_ARCHOS_CFG);
                        else
                            binCaller();
                        while(1);
                    }
                    else
                        printk("error loading %s\n",cfg[cursorPos].image);
                    redraw=1;
                    break;
                }
                case BTN_F3:
                    if(usbstate)
                    {                 
                        usbenable=1;
                        USBEnableString();
                        if(disk_rmAll()!=MED_OK)
                        {
                            printk("can't umount\n");
                            goto loopErr;
                        }

                        enableUsbFw();
                        //waitKeyReleased(NO_TIME_OUT);  
                        for(dd=0;dd<1000;dd++) /* nothing */;  
                        //pass_key_release = 1;
                        evt_purgeHandler(evt_handler);
                    }
                    break;                
            }
        }
    }
}



int avlo_chargeProgress = 0;

void avlo_drawBat(void)
{
    int power=0;
    int color=0;
    int level=0;
    int frame_color = 0;
    
    if(!POWER_CONNECTED)
    {
        power = GET_BAT_LEVEL;
        if(GET_BAT_LEVEL==-1)
        {
            level = 0;
            color = COLOR_PWR_L3;
        }
        else
        {
            if(power < 1320)
                color = COLOR_PWR_L0;
            else if(power < 1400)
                color = COLOR_PWR_L1;
            else if(power < 1480)
                color = COLOR_PWR_L2;
            else
                color = COLOR_PWR_L3;
    
            if(power > 1200)
                level = (int)(power - 1300) / 15;
        }    
        if(level > 20)
            level = 20;
        frame_color = COLOR_PWR_FRAME_DC_OFF;
    }
    else
    {
        if(avlo_chargeProgress == 0)
            level = 0;
        else if(avlo_chargeProgress == 1)
            level = 7;
        else if(avlo_chargeProgress == 2)
            level = 14;
        else
            level = 20;

        if(avlo_chargeProgress < 3 && avlo_chargeProgress >=0)
            avlo_chargeProgress++;
        else
            avlo_chargeProgress = 0;

        color = COLOR_PWR_CHARGE;
        frame_color = COLOR_PWR_FRAME_DC_ON;
    }

    gfx_drawRect(frame_color,BAT_X,BAT_Y,22,10);
    gfx_fillRect(frame_color,BAT_X+22,BAT_Y+2,3,6);
    gfx_fillRect(COLOR_TSP,BAT_X+1,BAT_Y+1,20,8);
    gfx_fillRect(color,BAT_X+1,BAT_Y+1,level,8);
    
}

void drawBox(int txt_width,int txt_height,int * start_x,int * start_y)
{
    *start_x = (LCD_WIDTH-(txt_width+8))/2;
    *start_y = (LCD_HEIGHT-(txt_height+8))/2;
    gfx_drawRect(COLOR_MSG_BOX_0,*start_x,*start_y,txt_width+8,txt_height+8);
    gfx_drawRect(COLOR_MSG_BOX_1,*start_x+1,*start_y+1,txt_width+6,txt_height+6);
    gfx_fillRect(COLOR_BOX,*start_x+2,*start_y+2,txt_width+4,txt_height+4);
    *start_x += 4;
    *start_y += 4;
}

void (*decode)(char * src,char * dst)=(void (*)(char * src,char * dst))AJZ_DECODE_ADDR;

int fastLoadCJBM(char * filename)
{
    int x,y,h,w;
#if 1
    gfx_fontSet(TXT_FONT);
    gfx_getStringSize("Can't load ajz atm...",&w,&h);
    drawBox(w,h,&x,&y);
    gfx_putS(COLOR_TXT,COLOR_BOX,x,y,"Can't load ajz atm...");
    return 0;
#else
    unsigned char * cptr;
    if(decode)
    {
    gfx_getStringSize("File Loaded, decompressing...",&w,&h);
    if (!loadFile(filename,(char*)0x03800000,1))
    {
        return 0;
    }
    printk("File loaded, now decompressing\n");
    //gfx_fillRect(COLOR_BOX,60, 100, 230, 40);
    gfx_fontSet(TXT_FONT);
    gfx_getStringSize("File Loaded, decompressing...",&w,&h);
    drawBox(w,h,&x,&y);
    gfx_putS(COLOR_TXT,COLOR_BOX,x,y,"File Loaded, decompressing...");
    cptr=(unsigned char *)0x03F00860+0x10;
    *cptr=0;
    decode((char*)0x03800000,(char*)0x03000000);
    printk("decompress done\n");
    }
    else
        printk("No decode function\n");
    return 1;
#endif
}

void moveCursor(int direction,int nbCfg)
{
    int h,w,offset;    
    // unhighlight current
    gfx_fontSet(TXT_FONT);
    gfx_getStringSize(cfg[cursorPos].label,&w,&h);
    offset = h;
    w--;
    h--;
    gfx_putS(COLOR_TXT,COLOR_TSP,ENTRY_X, ENTRY_Y + cursorPos*offset,cfg[cursorPos].label);    
    gfx_drawLine(COLOR_TSP,ENTRY_X-1,ENTRY_Y+1+ cursorPos*offset,ENTRY_X-1,ENTRY_Y+h-1+ cursorPos*offset);
    gfx_drawLine(COLOR_TSP,ENTRY_X+w+1,ENTRY_Y+1+ cursorPos*offset,ENTRY_X+w+1,ENTRY_Y+h-1+ cursorPos*offset);
    gfx_drawLine(COLOR_TSP,ENTRY_X-2,ENTRY_Y+2+ cursorPos*offset,ENTRY_X-2,ENTRY_Y+h-2+ cursorPos*offset);
    gfx_drawLine(COLOR_TSP,ENTRY_X+w+2,ENTRY_Y+2+ cursorPos*offset,ENTRY_X+w+2,ENTRY_Y+h-2+ cursorPos*offset);
    // move to nxt
    cursorPos+=direction;
    
    if(cursorPos<0) cursorPos=nbCfg;
    if(cursorPos>nbCfg) cursorPos=0;
    // highlight nxt
    gfx_putS(COLOR_TXT,COLOR_SEL,ENTRY_X, ENTRY_Y + cursorPos*offset,cfg[cursorPos].label);
    gfx_getStringSize(cfg[cursorPos].label,&w,&h);
    w--;
    h--;
    gfx_drawLine(COLOR_SEL,ENTRY_X-1,ENTRY_Y+1+ cursorPos*offset,ENTRY_X-1,ENTRY_Y+h-1+ cursorPos*offset);
    gfx_drawLine(COLOR_SEL,ENTRY_X+w+1,ENTRY_Y+1+ cursorPos*offset,ENTRY_X+w+1,ENTRY_Y+h-1+ cursorPos*offset);
    gfx_drawLine(COLOR_SEL,ENTRY_X-2,ENTRY_Y+2+ cursorPos*offset,ENTRY_X-2,ENTRY_Y+h-2+ cursorPos*offset);
    gfx_drawLine(COLOR_SEL,ENTRY_X+w+2,ENTRY_Y+2+ cursorPos*offset,ENTRY_X+w+2,ENTRY_Y+h-2+ cursorPos*offset);
    // change bottom status
    gfx_fillRect(COLOR_TSP,STATUS_X, STATUS_Y, 200, 10);
    snprintf(tmp_txt,100,"%s%s",BTM_TXT,cfg[cursorPos].image);
    gfx_putS(COLOR_TXT,COLOR_TSP,STATUS_X, STATUS_Y,tmp_txt);
}

void affUSB()
{
    if(usbstate != usb_isConnected())
    {
        usbstate=usb_isConnected();
        if(usbstate)
        {
           USB_connDisp();
        }
        else
        {
            USB_connHide();
        }
    }
}

void doFault(int faultNum)
{
    int evt;
    printFault(faultNum);
    enableUsbFw();
    usbenable=1;
    printk("error, let's loop\n");
    
    evt_purgeHandler(evt_handler);
    
    while(1)
    {      
        evt=evt_getStatusBlocking(evt_handler);
        if(evt != BTN_OFF)            
            break;
    }    
}


int processDefault(int evt,int nbCfg)
{
    int pos;
    if(chkdefault)
    {
        if(evt==NO_EVENT && cnt < delayCnt)
            gfx_fillRect(COLOR_WAIT,BAR_X+(BAR_W*(cnt++))/delayCnt, BAR_Y, 1, BAR_H);
        if(cnt==delayCnt)
        {
            pos=0;
            while(pos<nbCfg+1 && strcmp(cfg[pos].label,cfgG.defBin))
                pos++;
            if(pos<nbCfg+1)
            {            
                char * ext=strrchr(cfg[pos].image,'.')+1;
                int launch=0;
                printk("loading: %s ext:%s\n",cfg[pos].image,ext);
                if(ext[0]=='a' && ext[1]=='j' && ext[2]=='z' && ext[3]=='\0'                        
                                && fastLoadCJBM(cfg[pos].image))
                    launch=1;
                else if (loadFile(cfg[pos].image,(char*)SDRAM_START,1))
                    launch=1;

                if(launch)
                {
                    printk("File loaded, now launching\n");
                    printk("append=%s\n",cfg[pos].append);
                    ata_StopHD(HD_DISK);
                    if(SDRAM_START!=SDRAM_ARCHOS_START)
                        resetMem(SDRAM_ARCHOS_START,SDRAM_ARCHOS_CFG);
                    else
                        binCaller();
                    while(1);
                }
                else
                    printk("error loading %s\n",cfg[pos].image);
            }
            errNoDefault=1;
            chkdefault=0;
            cnt=0;

        }

        if(evt!=NO_EVENT)
        {
            gfx_fillRect(COLOR_TSP,BAR_X, BAR_Y, BAR_W, BAR_H);
            chkdefault=0;
            cnt=0;
        }
    }
    return 0;
}

                               
#if 0
void printErr(int key)
{
    if(errNoDefault)
    {
        if(cntNoDefault<0x2000)
            cntNoDefault++;
        else
        {
            if(stateNoDefault)
            {
                gfx_fillRect(COLOR_TSP,0, 210, 320, 10);
            }
            else
            {
                gfx_fillRect(COLOR_TSP,0, 210, 320, 10);
                gfx_putS(COLOR_TXT,COLOR_TSP,80,210,"default image can't be found");
            }
            stateNoDefault=!stateNoDefault;
            cntNoDefault=0;
        }

        if(key & BTMASK_ANY)
        {
            gfx_fillRect(COLOR_TSP,0, 210, 320, 10);
            errNoDefault=0;
        }
    }
}
#endif
                               
void drawMenu(int nbCfg)
{
    int pos;
    int bg_color;
    int w,h,offset;

    // clean AVLO txt */
    gfx_clearScreen(COLOR_TSP);
    
    gfx_fillRect(COLOR_TSP,ENTRY_X, ENTRY_Y, 100-7, 100-7);

    gfx_drawRect(COLOR_MSG_BOX_0,BAR_X-2,BAR_Y-2,BAR_W+4,BAR_H+4);
    gfx_drawRect(COLOR_MSG_BOX_1,BAR_X-1,BAR_Y-1,BAR_W+2,BAR_H+2);
    gfx_fillRect(COLOR_TSP,BAR_X, BAR_Y, BAR_W, BAR_H);
    
    gfx_fontSet(TXT_FONT);
    gfx_getStringSize("M",&w,&h);
    offset=h;

    for(pos=0;pos<nbCfg+1;pos++)
    {
        if (pos==cursorPos)
        {
            bg_color = COLOR_SEL;            
        }
        else
        {
            bg_color = COLOR_TSP;
        }

        gfx_fontSet(TXT_FONT);
        gfx_putS(COLOR_TXT,bg_color,ENTRY_X, ENTRY_Y + pos*offset,cfg[pos].label);

        if(pos==cursorPos)
        {
            gfx_getStringSize(cfg[cursorPos].label,&w,&h);
            w--;
            h--;
            gfx_drawLine(COLOR_SEL,ENTRY_X-1,ENTRY_Y+1+ cursorPos*offset,ENTRY_X-1,ENTRY_Y+h-1+ cursorPos*offset);
            gfx_drawLine(COLOR_SEL,ENTRY_X+w+1,ENTRY_Y+1+ cursorPos*offset,ENTRY_X+w+1,ENTRY_Y+h-1+ cursorPos*offset);
            gfx_drawLine(COLOR_SEL,ENTRY_X-2,ENTRY_Y+2+ cursorPos*offset,ENTRY_X-2,ENTRY_Y+h-2+ cursorPos*offset);
            gfx_drawLine(COLOR_SEL,ENTRY_X+w+2,ENTRY_Y+2+ cursorPos*offset,ENTRY_X+w+2,ENTRY_Y+h-2+ cursorPos*offset);
            gfx_fillRect(COLOR_TSP,STATUS_X, STATUS_Y, 200, 10);
            snprintf(tmp_txt,100,"%s%s",BTM_TXT,cfg[pos].image);
            gfx_putS(COLOR_TXT,COLOR_TSP,STATUS_X, STATUS_Y,tmp_txt);
        }
    }
}

int loadFile(char * fileN,char* buffer,int prog)
{
    int curFile;
    int offset=0;
    int size;
    

    curFile=open(fileN,O_RDONLY);
    printk("openFile(): %x-%s\n",curFile,fileN);


    if(curFile>=0)
    {
        size=filesize(curFile);
        printk("file size:%d\n",size);

        while((read(curFile,&buffer[offset],32*512))>0)
        {
           if(prog)
                drawProgress(offset,size,0);
            offset+=32*512;
        }
        close(curFile);
        return 1;
    }
    else
    {
        printk("Error loading file\n");
        return 0;
    }
}

#define TEST_COLOR(VAR,NAME,NAME2,INDEX) {  \
    if(VAR->NAME.r!=-1)               \
    {                                 \
        osd_setPaletteRGB(VAR->NAME.r,VAR->NAME.g,VAR->NAME.b,INDEX); \
        VAR->NAME2=INDEX;             \
        printk("new color (%d,%d,%d) at %d\n",VAR->NAME.r,VAR->NAME.g,VAR->NAME.b,INDEX); \
    }                                 \
}

void init_colors(void)
{
    TEST_COLOR(ptr_cfg,color_txt,color_txt_index,CUSTOM_COLOR_START)
    TEST_COLOR(ptr_cfg,color_box,color_box_index,CUSTOM_COLOR_START+1)
    TEST_COLOR(ptr_cfg,color_sel,color_sel_index,CUSTOM_COLOR_START+2)
    TEST_COLOR(ptr_cfg,color_load,color_load_index,CUSTOM_COLOR_START+3)
    TEST_COLOR(ptr_cfg,color_wait,color_wait_index,CUSTOM_COLOR_START+4)
    TEST_COLOR(ptr_cfg,color_msg_box_0,color_msg_box_0_index,CUSTOM_COLOR_START+5)
    TEST_COLOR(ptr_cfg,color_msg_box_1,color_msg_box_1_index,CUSTOM_COLOR_START+6)
    TEST_COLOR(ptr_cfg,color_pwr_charge,color_pwr_charge_index,CUSTOM_COLOR_START+7)
    TEST_COLOR(ptr_cfg,color_pwr_l0,color_pwr_l0_index,CUSTOM_COLOR_START+8)
    TEST_COLOR(ptr_cfg,color_pwr_l1,color_pwr_l1_index,CUSTOM_COLOR_START+9)
    TEST_COLOR(ptr_cfg,color_pwr_l2,color_pwr_l2_index,CUSTOM_COLOR_START+10)
    TEST_COLOR(ptr_cfg,color_pwr_l3,color_pwr_l3_index,CUSTOM_COLOR_START+11)
    TEST_COLOR(ptr_cfg,color_pwr_frame_dc_on,color_pwr_frame_dc_on_index,CUSTOM_COLOR_START+12)
    TEST_COLOR(ptr_cfg,color_pwr_frame_dc_off,color_pwr_frame_dc_off_index,CUSTOM_COLOR_START+13)
}
