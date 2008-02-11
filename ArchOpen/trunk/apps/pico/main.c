#include "shared.h"
#include "picoint.h"
#include "gui.h"
#include "pico/sound.h"


unsigned int *myBitmap;
unsigned int *framebuf;

#define INPUT_UP       0x1
#define INPUT_DOWN     0x2
#define INPUT_LEFT     0x4
#define INPUT_RIGHT    0x8
#define INPUT_A        0x10
#define INPUT_B        0x20
#define INPUT_C        0x40
#define INPUT_START    0x80

#ifdef SOUND_USE_DSP
tDspCom * dspCom;
#endif

unsigned short pSndBuffer[SAMPLECOUNT*2];

int FrameSkip=0;
int oldVideoMode=0;


int fpsCount=0;
int fpsStart;

__IRAM_DATA volatile int vblankNum;
int prevVblankNun=0;

char CurrentROMFile[512]="";

int state_slot;
int tvOut=0;

int armFrequency, overclocking;

int key_btn1_config, key_btn2_config, key_f3_config, key_on_config;

int screen_initialX, screen_initialY, screen_initialWidth, screen_initialHeight;

void clk_overclock(bool en)
{
#ifdef GMINI_OVERCLOCKING
    int dspf;

    if(en && overclocking){
        clkc_setClockFrequency(CLK_ARM,armFrequency*1000000);
    }else{
        // default params
        clkc_setClockParameters(CLK_ARM,15,2,2);
        clkc_setClockParameters(CLK_ACCEL,15,2,1);
    }
#endif
};

void print_infos()
{}
#ifdef PROFILING
void print_profiling()
{
    int i;
    char s[100];

    int old_plane=gfx_getPlane();
    gfx_setPlane(BMAP1);
    gfx_planeShow(BMAP1);
    gfx_clearScreen(COLOR_TRSP);
    for (i=0 ; i<profile_count ; i++)
    {
        profile_sprint(s, i);
        gfx_putS(COLOR_RED, COLOR_WHITE, 0, 30+9*i, s);
    }
    gfx_setPlane(old_plane);
}
#endif

#ifdef HAVE_RESIZE
void resize_init(int sw, int srw, int sh, int w,int h)
{
    long ip,op;

    op=(long)gfx_planeGetBufferOffset(VID1);
    ip=(long)(myBitmap+8+328*8);

    resize_setup(ip,sw*2,srw*2,sh,op,w*2,h);
}
#endif

#ifdef SOUND_USE_DSP
void dsp_interrupt(int irq,struct pt_regs * regs)
{
    // debug message coming from the dsp
    if (dspCom->hasDbgMsg)
    {
        char * str=malloc(256);
        int i;

        for (i=0;i<256;++i)
        {
            str[i]=dspCom->dbgMsg[i];
        }

        printf("dsp> %s\n",str);
        dspCom->hasDbgMsg=0;
        free(str);
    }
}
#endif


void dsp_init()
{
    extern char _binary_apps_pico_dspcode_pico_dsp_out_start;
    extern char _binary_apps_pico_dspcode_pico_dsp_out_end;
    unsigned char * dspcode=&_binary_apps_pico_dspcode_pico_dsp_out_start;
    int len=&_binary_apps_pico_dspcode_pico_dsp_out_end-&_binary_apps_pico_dspcode_pico_dsp_out_start;

#ifdef SOUND_USE_DSP
    // dsp irq handler
    irq_changeHandler(IRQ_DSP,dsp_interrupt);
    irq_enable(IRQ_DSP);

    *DSP_COM=0;

    dsp_loadProgramFromMemory(dspcode,len);

    // setup dma
    outw((inw(0x309a8)&0x03ff)|0x1000,0x309a8); //REFCTL : DMA 2 is used for DSP<>SDRAM

    dsp_run();

    while (!(*DSP_COM)); // wait for the dsp to finish init

    dspCom = (tDspCom *) DSP_RAM(*DSP_COM);

    dspCom->armBusy=1;
    dspCom->chipNum=dsp_getChipNum();
    dspCom->PSGTail=0;
    dspCom->PSGHead=0;
    dspCom->YM2612Tail=0;
    dspCom->YM2612Head=0;
    dspCom->PSGEnabled=1;
    dspCom->YM2612Enabled=1;
    dspCom->DACEnabled=1;
    dsp_write32(&dspCom->lfo_pm_table_addr,((unsigned long)lfo_pm_table)-SDRAM_START);
#ifdef DM320
    dspCom->port280Interrupt=1;
#else
    dspCom->port280Interrupt=0;
#endif
    dspCom->armInitFinished=1;
#endif
}



void snd_init()
{
#ifdef SOUND_USE_AIC23
    aic23_setSampleRate(PsndRate);
    aic23_enableOutput(true);
    aic23_setOutputVolume(65+27,AIC23_CHANNEL_BOTH);
#endif
}

void snd_setVolume(int vol)
{
#ifdef SOUND_USE_AIC23
    aic23_setOutputVolume(vol+27,AIC23_CHANNEL_BOTH);
#endif
}

void snd_close()
{
#ifdef SOUND_USE_AIC23
    aic23_enableOutput(false);
#endif
}

//Callback function used by scanline renderer
int MyScan(unsigned int num, unsigned int *data)
{
    int i;

    for ( i=0; i<320 ; i++ )
    {
#ifdef HAVE_RESIZE
        myBitmap[i+num*328+8+328*8]=data[i];
#else
        myBitmap[i+num*320]=data[i];
#endif
    }

    return 0;
}

//Callback function used to convert cram to YUV colors
int YUVCram(int cram)
{
    int high, R, G, B;

    R=(cram&0x00e)<<4;
    G=cram&0x0e0;
    B=(cram&0xe00)>>4;

    high=osd_RGB2Packed(R, G, B);
    return high;
}

//Callback function to update palette
void UpdatePalette()
{
    int c=0;

    // Update palette:
    for (c=0;c<64;c++) HighPal[c]=YUVCram(Pico.cram[c]);
    Pico.m.dirtyPal=0;
}

static unsigned int LastSecond=0;
static int FramesDone=0;
static int FramesPerSecond=60;

static int DoFrame()
{
    PicoSkipFrame=1;
    PicoScan=NULL;

    //Emulation of 1 frame
    PicoFrame();
    return 0;
}

static int DrawFrame()
{
    PicoSkipFrame=0;
    PicoScan=MyScan; // Setup scanline callback

    /*PicoScan=NULL;
    PicoSkipFrame=1;*/

    //Tracks mode changes (32 or 40 column mode)
    if (Pico.video.reg[12]&1 )
    {
        if ( oldVideoMode!=40 )
        {
            display_set();
            oldVideoMode=40;
        }
    }
    else
    {
        if ( oldVideoMode!=32 )
        {
            display_set();
            oldVideoMode=32;
        }
    }

    //Emulation of 1 frame
    PicoFrame();

    PicoScan=NULL;
    PicoSkipFrame=1;

    return 0;
}

int prevTick;

int EmulateFrame()
{

    int i=0/*need=0*/;
    int time=0,frame=0;
    int frameTickDelta,frameTick,frameLen;

    // get prev values
    prevVblankNun=vblankNum;
    prevTick=tmr_getMicroTick();

    for (i=0;i<FrameSkip;i++)
    {
        DoFrame();
    }

    DrawFrame();


    // frame sync
    frameTick=tmr_getMicroTick()-prevTick;
    frameLen=(FrameSkip+1)*(Pico.m.pal?100000/50:100000/60);
    frameTickDelta=frameLen-frameTick;

    if ( !Pico.m.pal && (tvOut==0 || tvOut==2))
    {
        if (frameTick<frameLen)
        {
            while ((vblankNum-prevVblankNun)<(FrameSkip+1)) /* nothing */;
        }
    }
    else
    {
        while(frameTick<frameLen){
            frameTick=tmr_getMicroTick()-prevTick;
        }
    }


    if (frameTickDelta>2*(Pico.m.pal?100000/50:100000/60)/3) --FrameSkip; // let's assume rendering is done in 2/3 the time of a frame
    if (frameTickDelta<-(Pico.m.pal?100000/50:100000/60)/20) ++FrameSkip; // we tolerate 5% error
    FrameSkip=FrameSkip>0?FrameSkip:0;

    // fps count
        fpsCount++;
        if (prevTick-fpsStart>=100000){
            printf("%d fps\n",fpsCount);
            fpsCount=0;
            fpsStart=prevTick;
        }

    //profile_printAll();
    return 0;
}

__IRAM_CODE void osd_interrupt(int irq,struct pt_regs * regs)
{
    vblankNum++;
}

void display_init()
{
    gfx_openGraphics();

    vblankNum=0;
    irq_changeHandler(IRQ_OSD,osd_interrupt);
    irq_enable(IRQ_OSD);

#ifdef HAVE_RESIZE
    myBitmap=malloc(328*256*sizeof(unsigned int));
    myBitmap=(char*)(((unsigned int)myBitmap+32)&0xffffffe0);

    framebuf=(unsigned int *)malloc(328*248*4);
    framebuf=(char*)(((unsigned int)framebuf+32)&0xffffffe0);

    gfx_planeSetBufferOffset(VID1, framebuf);
#else
    myBitmap=malloc(328*256*sizeof(unsigned int));
    gfx_planeSetBufferOffset(VID1, myBitmap);
#endif

    char *bmap=malloc(320*256);
    bmap=(char*)(((unsigned int)bmap+32)&0xffffffe0);
    gfx_planeSetBufferOffset(BMAP1,bmap);

    gfx_planeGetPos(VID1,&screen_initialX, &screen_initialY);
    gfx_planeGetSize(VID1,&screen_initialWidth,&screen_initialHeight,NULL);

#if defined(GMINI402) || defined(GMINI4XX)
    screen_initialWidth=216;
    //osd_setComponentSourceWidth(OSD_VIDEO1,220/8);
#endif

    gui_showGuiPlane();
}

static void romfname_ext(char *dst, char *ext)
{
    //build save file name
    sprintf(dst,SAVES_PATH"%s",strrchr(CurrentROMFile,'/')+1);
    *(char *)strrchr(dst,'.')='\0';
}

typedef unsigned int (*STATE_SL_FUNC)(int, void *, int);

int SaveLoadGame(int load, int sram)
{
    int ret = 0;
    char saveFname[512];

    // make save filename
    romfname_ext(saveFname, "");
    if (sram) strcat(saveFname, ".srm");
    else
    {
        if (state_slot > 0 && state_slot < 10) sprintf(saveFname, "%s.%i", saveFname, state_slot);
        strcat(saveFname, ".mds");
    }

    gui_showText(saveFname);

    if (sram)
    {
        int sramFile;
        int sram_size = SRam.end-SRam.start+1;
        if (SRam.reg_back & 4) sram_size=0x2000;
        if (!SRam.data) return 0; // SRam forcefully disabled for this game
        if (load)
        {
            sramFile = open(saveFname, O_RDONLY);
            if (sramFile<0) return -1;
            read(sramFile, SRam.data, sram_size);
            close(sramFile);
        }
        else
        {
            // sram save needs some special processing
            // see if we have anything to save
            for (; sram_size > 0; sram_size--)
                if (SRam.data[sram_size-1]) break;

            if (sram_size)
            {
                sramFile = open(saveFname, O_RDWR | O_CREAT);
                ret = write(sramFile, SRam.data, sram_size);
                ret = (ret != sram_size) ? -1 : 0;
                close(sramFile);
                //sync();
            }
        }
        return ret;
    }
    else
    {
        int PmovFile = -1;
        // try gzip first
        /*if(currentConfig.EmuOpt & 8) {
        	strcat(saveFname, ".gz");
        	if( (PmovFile = gzopen(saveFname, load ? "rb" : "wb")) ) {
        		areaRead  = gzRead2;
        		areaWrite = gzWrite2;
        		if(!load) gzsetparams(PmovFile, 9, Z_DEFAULT_STRATEGY);
        	} else
        		saveFname[strlen(saveFname)-3] = 0;
        }*/
        if (PmovFile<=0)
        { // gzip failed or was disabled
            if ( (PmovFile = open(saveFname, load ? O_RDONLY : O_RDWR | O_CREAT ))>=0 )
            {
                areaRead  = (STATE_SL_FUNC) read;
                areaWrite = (STATE_SL_FUNC) write;
                gui_showText("State file opened");
            }
        }
        if (PmovFile>=0)
        {
            PmovState(load ? 6 : 5, PmovFile);
            //strcpy(noticeMsg, load ? "GAME LOADED" : "GAME SAVED");
            /*if(areaRead == gzRead2)
            	 gzclose(PmovFile);
            else */
            close (PmovFile);
            PmovFile = -1;
            /*if (!load) sync();
            else Pico.m.dirtyPal=1;*/
            if (load) Pico.m.dirtyPal=1;
        }
        else
        {
            //strcpy(noticeMsg, load ? "LOAD FAILED" : "SAVE FAILED");
            ret = -1;
        }

        return ret;
    }
}

void display_set()
{
    int mode=0;
    int x=0, y=0,w,h;
    int gx, gy, gw,gh;

    if (!tvOut) //LCD
    {
        w=screen_initialWidth;
        h=screen_initialHeight;
        x=0;
        y=0;
        gx=0;
        gy=0;
        getResolution(&gw, &gh);
        mode=VIDENC_MODE_LCD;
    }
    else
    {
        switch (tvOut)
        {
        case 1:
            mode=VIDENC_MODE_PAL;
            x=40;
            y=16;
            w=320;
            h=240;
            gx=0;
            gy=0;
            gw=320;
            gh=240;
            break;
        default:
        case 2:
            mode=VIDENC_MODE_NTSC;
            x=40;
            y=8;
            w=320;
            h=240;
            gx=0;
            gy=0;
            gw=320;
            gh=240;
            break;
        }

    }

    gfx_planeSetPos(VID1,screen_initialX+x+4,screen_initialY+y);
    gfx_planeSetSize(VID1,w,h,32,GFX_SMODE_STD);

#if defined(GMINI402) || defined(GMINI4XX)
    if ( !tvOut )
        osd_setComponentSourceWidth(OSD_VIDEO1,216/8);
#endif

    #ifdef HAVE_RESIZE
        if (Pico.video.reg[12]&1)
            resize_init(328, 320, 224, w, h);
        else
            resize_init(328, 256, 224, w, h);
    #endif

    printf("VID1 pos : %d - %d\n", screen_initialX+x,screen_initialY+y);
    printf("VID1 size : %d - %d\n", w,h);

    gfx_planeSetPos(BMAP1,screen_initialX+gx,screen_initialY+gy);
    gfx_planeSetSize(BMAP1,gw,gh,8,GFX_SMODE_STD);

    videnc_setup(mode,false);

    // resize browser
    if (gw!=browser->width || gh!=browser->height){
        browser->width=gw;
        browser->height=gh;
        browser->nb_disp_entry=-1; // recompute values
        browser->max_entry_length=-1;
        gui_browserNeedInit=true;
    }
}

void emu_init()
{
    memset(pSndBuffer, 0, SAMPLECOUNT*4);
    PsndLen=SAMPLECOUNT;
    PsndRate=SAMPLE_RATE;
    PsndOut=NULL;
    PicoMask=0xffff;

    //Setup callback functions
    PicoPrepareCram=UpdatePalette;
    PicoScan=MyScan;
    PicoCram=YUVCram;


    PicoInit();
}

void emu_run()
{
    int quit=0, key;
    int pad=0;
    int lastFrame=0, currentFrame=0, fps=0, nof=0;
    char s[300];

    clk_overclock(true);

#ifdef SOUND_USE_DSP
    dspCom->armBusy=0;
#endif

    fpsStart=prevTick=tmr_getMicroTick();

    display_set();

    printf("emu_run\n");

    while (!quit)
    {

        key=btn_readState();
        pad=0;

        if ( key&BTMASK_F1 )
        {
#ifdef SOUND_USE_DSP
            dspCom->armBusy=1;
#endif

            gui_launchMenu();
            gui_applySettings();

            display_set();

#ifdef SOUND_USE_DSP
            dspCom->armBusy=0;
#endif

            prevTick=tmr_getMicroTick();
        }

        if ( key&BTMASK_F2 )
        {
            if (!con_screenIsVisible())
                con_screenSwitch();
        }
        else
        {
            if (con_screenIsVisible())
                con_screenSwitch();
        }


        if ( key&BTMASK_UP ) pad|=INPUT_UP;
        if ( key&BTMASK_DOWN ) pad|=INPUT_DOWN;
        if ( key&BTMASK_LEFT ) pad|=INPUT_LEFT;
        if ( key&BTMASK_RIGHT ) pad|=INPUT_RIGHT;

        if ( key&BTMASK_BTN1 ) pad|=key_btn1_config;
        if ( key&BTMASK_BTN2 ) pad|=key_btn2_config;
        if ( key&BTMASK_F3 ) pad|=key_f3_config;
        if ( key&BTMASK_ON ) pad|=key_on_config;

        PicoPad[0]=pad;

        if ( key&BTMASK_OFF )
            quit=1;

        EmulateFrame();

#ifdef HAVE_RESIZE
        resize_execute();
#endif

        //Fps calculation
        nof++;
        currentFrame=tmr_getTick();
        if ( currentFrame-lastFrame>100 )
        {
            fps=nof;
            nof=0;
            lastFrame=currentFrame;
        }
    }
#ifdef SOUND_USE_DSP
    dspCom->armBusy=1;
#endif

    clk_overclock(false);
}

int app_main(int argc, char **argv)
{
    int quit=0, key, f;
    int m, n, div;
    int pad=0;
    int lastFrame=0, currentFrame=0, fps=0, nof=0;
    char s[512];

    printf("Pico start\n");

    // disable LCD & halt timer
    set_timer_status(LCD_TIMER,TIMER_MODE_BAT,MODE_DISABLE);
    set_timer_status(LCD_TIMER,TIMER_MODE_DC,MODE_DISABLE);
    set_timer_status(HALT_TIMER,TIMER_MODE_BAT,MODE_DISABLE);
    set_timer_status(HALT_TIMER,TIMER_MODE_DC,MODE_DISABLE);

    // create dirs if they don't exist
    mkdir(PICO_PATH,-1);
    mkdir(SAVES_PATH,-1);

    emu_init();
    printf("emu_init:done\n");
    display_init();
    printf("display_init:done\n");
    snd_init();
    printf("snd_init:done\n");
    dsp_init();
    printf("dsp_init:done\n");
    gui_init();
    printf("gui_init:done\n");


    printf("[PicoInit]:done.\n");

    gui_applySettings();

    while (1)
    {
        clk_overclock(false);

        if (!gui_browse()) break;

        f=open(CurrentROMFile, O_RDONLY);

        if ( Pico.rom )
            free(Pico.rom);
        PicoCartLoad(f, &Pico.rom, &Pico.romsize);
        close(f);
        PicoCartInsert(Pico.rom,Pico.romsize);

        //Load sram if available
        SaveLoadGame(1, 1);

        FramesPerSecond=Pico.m.pal?50:60;

        gui_showEmuPlane();
        emu_run();

        //Save sram if changed
        if ( SRam.changed )
            SaveLoadGame(0, 1);

    }

    gui_showText("Saving settings...");
    clk_overclock(false);
    snd_close();
    gui_close();
    videnc_setup(VIDENC_MODE_LCD,false);
    gfx_closeGraphics();

    return 0;
}
