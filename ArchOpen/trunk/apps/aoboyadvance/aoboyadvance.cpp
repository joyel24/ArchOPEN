#include "GBA.h"
#include "GBAinline.h"
#include "medios.h"
#include "aoboyadvance.h"


extern "C"{
    #include "MMU.h"
}

#define DEBUG_ABORT
#define DEBUG_SWI

#ifdef DEBUG_ABORT
#define ABORT_PRINT(s...)   printk(s)
#else
#define ABORT_PRINT(s...)
#endif

#ifdef DEBUG_SWI
#define SWI_PRINT(s...)   printk(s)
#else
#define SWI_PRINT(s...)
#endif

#define LINE_TIMER_DIVIDER 333

struct EmulatedSystem emulator;

int systemRedShift = 0;
int systemBlueShift = 16;
int systemGreenShift = 8;
int systemColorDepth = 32;
int systemDebug = 0;
//int systemVerbose = 0x0;
int systemVerbose = 0xffff;
int systemSaveUpdateCounter = SYSTEM_SAVE_NOT_UPDATED;

int cartridgeType = 3;

int emulating = 0;
int RGB_LOW_BITS_MASK=0x821;
int systemFrameSkip = 1;
u32 systemColorMap32[0x10000];
u16 systemColorMap16[0x10000];

bool systemSoundOn = false;

char romPath[MAX_PATH];
int fpsStart=0;
int fpsCount=0;

void systemMessage(int num, const char *msg, ...)
{
  char buffer[2048];
  va_list valist;

  va_start(valist, msg);
  vsprintf(buffer, msg, valist);

  printk("%s\n", buffer);
  va_end(valist);
}

void systemDrawScreen()
{
}

bool systemReadJoypads()
{
  return true;
}

u32 systemReadJoypad(int which)
{
    int res=0;
    int b=btn_readState();

    if (b&BTMASK_BTN1)  res|=1;
    if (b&BTMASK_BTN2)  res|=2;
    if (b&BTMASK_F2)    res|=4;
    if (b&BTMASK_ON)    res|=8;
    if (b&BTMASK_RIGHT) res|=16;
    if (b&BTMASK_LEFT)  res|=32;
    if (b&BTMASK_UP)    res|=64;
    if (b&BTMASK_DOWN)  res|=128;
    if (b&BTMASK_F3)    res|=256;
    if (b&BTMASK_F1)    res|=512;

    if (b&BTMASK_F2){
        int f;

    cache_clean();
    cache_invalidate(CACHE_ALL);

        f=open("/iram.bin",O_RDWR|O_CREAT);
        write(f,internalRAM,0x8000);
        close(f);
    }

    return res;
}

void systemShowSpeed(int speed)
{
}

void system10Frames(int rate)
{
}

void systemFrame()
{
    int prevTick=tmr_getMicroTick();

    fpsCount++;
    if (prevTick-fpsStart>=100000){
        printf("%d fps\n",fpsCount);
        fpsCount=0;
        fpsStart=prevTick;
    }

}

void systemSetTitle(const char *title)
{
}

void systemScreenCapture(int a)
{
}

u32 systemReadJoypadExtended()
{
  return 0;
}

void systemWriteDataToSoundBuffer()
{
}

bool systemSoundInit()
{
  return true;
}

void systemSoundShutdown()
{
}

void systemSoundPause()
{
}

void systemSoundResume()
{
}

void systemSoundReset()
{
}

static int clockTicks = 0;

u32 systemGetClock()
{
  return clockTicks++;
}

void systemUpdateMotionSensor()
{
}

int systemGetSensorX()
{
  return 0;
}

int systemGetSensorY()
{
  return 0;
}

void systemGbPrint(u8 *data,int pages,int feed,int palette, int contrast)
{
}

void systemScreenMessage(const char *msg)
{
}

bool systemCanChangeSoundQuality()
{
  return false;
}

bool systemPauseOnFrame()
{
  return false;
}

void systemGbBorderOn()
{
}

void dbgOutput(char * s,u32 u){
    if (s) printk(s);
}


u8 * allocBoundary(int size,int shift){
    u8 * data=(u8 *)calloc(1,size+(1<<shift));
    data=(u8* )(((u32)data+(1<<shift))&((0xffffffff>>shift)<<shift));

//    for(u32 * i=(u32*)data;i<(u32*)(data+size);++i) *i=(u32)i;

    return data;
}

void setupMMU(){
    u32 i,desc,offset;
    u8* uncachedZone;
    u32 dummy=0;

    rom=allocBoundary(8*1024*1024,16); // large
    printk("rom %x\n",rom);
    offset=0;
    for (i=0x8000000;i<0xA000000;i+=0x100000){
        desc=0x18f7000+offset|MMU_FLD_BASE|MMU_FLD_TYPE_COARSE;
        if (i>=0x8800000) desc=MMU_FLD_TYPE_FAULT;
        *MMU_GetFirstLevelDescriptorRef((u32 *)i)=desc;
        *MMU_GetFirstLevelDescriptorRef((u32 *)(i+0x2000000))=desc;
        *MMU_GetFirstLevelDescriptorRef((u32 *)(i+0x4000000))=desc;
        //*MMU_GetFirstLevelDescriptorRef((u32 *)(i+0x2000000))=MMU_FLD_TYPE_FAULT;
        //*MMU_GetFirstLevelDescriptorRef((u32 *)(i+0x4000000))=MMU_FLD_TYPE_FAULT;
        offset+=0x400;
    }
    for (i=0x8000000;i<0x8800000;i+=0x10000)
        MMU_SetSecondLevelDescriptor((u32 *)i,(u32)rom+(i&0xffffff)|MMU_SLD_TYPE_LARGE|MMU_CACHABLE|MMU_BUFFERABLE);

    for (i=0xe000000;i<0x10000000;i+=0x100000)
        *MMU_GetFirstLevelDescriptorRef((u32 *)i)=MMU_FLD_TYPE_FAULT;

    internalRAM=allocBoundary(32*1024,12); // small
    printk("iram %x\n",internalRAM);
    for (i=0x3000000;i<0x4000000;i+=0x100000)
        *MMU_GetFirstLevelDescriptorRef((u32 *)i)=0x18f9000|MMU_FLD_BASE|MMU_FLD_TYPE_COARSE;
    for (i=0x3000000;i<0x3100000;i+=0x1000)
        MMU_SetSecondLevelDescriptor((u32 *)i,(u32)internalRAM+(i&0x7fff)|MMU_SLD_TYPE_SMALL|MMU_CACHABLE/*|MMU_BUFFERABLE*/);

    workRAM=allocBoundary(256*1024,16); // large
    printk("wram %x\n",workRAM);
    for (i=0x2000000;i<0x3000000;i+=0x100000)
        *MMU_GetFirstLevelDescriptorRef((u32 *)i)=0x18fa000|MMU_FLD_BASE|MMU_FLD_TYPE_COARSE;
    for (i=0x2000000;i<0x2100000;i+=0x10000)
        MMU_SetSecondLevelDescriptor((u32 *)i,(u32)workRAM+(i&0x3ffff)|MMU_SLD_TYPE_LARGE|MMU_CACHABLE/*|MMU_BUFFERABLE*/);

    paletteRAM=allocBoundary(1024,10); // tiny
    printk("palette %x\n",paletteRAM);
    for (i=0x5000000;i<0x6000000;i+=0x100000)
        *MMU_GetFirstLevelDescriptorRef((u32 *)i)=0x18f6000|MMU_FLD_BASE|MMU_FLD_TYPE_FINE;
    for (i=0x5000000;i<0x5100000;i+=0x400)
        MMU_SetSecondLevelDescriptor((u32 *)i,(u32)paletteRAM|MMU_SLD_TYPE_TINY|MMU_CACHABLE|MMU_BUFFERABLE);

    vram=allocBoundary(96*1024,12); // small
    printk("vram %x\n",vram);
    for (i=0x6000000;i<0x7000000;i+=0x100000)
        *MMU_GetFirstLevelDescriptorRef((u32 *)i)=0x18f4000|MMU_FLD_BASE|MMU_FLD_TYPE_COARSE;
    for (i=0x6000000;i<0x6100000;i+=0x1000){
        if ((i&0x1ffff)<0x18000)
            offset=i&0x1ffff;
        else
            offset=(i&0x1ffff)-0x8000;
        MMU_SetSecondLevelDescriptor((u32 *)i,(u32)vram+offset|MMU_SLD_TYPE_SMALL|MMU_CACHABLE|MMU_BUFFERABLE);
    }

    oam=allocBoundary(1024,10); // tiny
    printk("oam %x\n",oam);
    for (i=0x7000000;i<0x8000000;i+=0x100000)
        *MMU_GetFirstLevelDescriptorRef((u32 *)i)=0x18f5000|MMU_FLD_BASE|MMU_FLD_TYPE_FINE;
    for (i=0x7000000;i<0x7100000;i+=0x400)
        MMU_SetSecondLevelDescriptor((u32 *)i,(u32)oam|MMU_SLD_TYPE_TINY|MMU_CACHABLE|MMU_BUFFERABLE);

    // IO ram
    ioMem=allocBoundary(1024,10); // tiny
    printk("ioMem %x\n",ioMem);
    SetDomainAccessControl(1,MMU_DOMAIN_CLIENT);
    *MMU_GetFirstLevelDescriptorRef((u32 *)0x4000000)=0x18f2000|MMU_FLD_BASE|MMU_FLD_TYPE_FINE|MMU_DOMAIN(1);
    for (i=0x4100000;i<0x5000000;i+=0x100000)
        *MMU_GetFirstLevelDescriptorRef((u32 *)i)=MMU_FLD_TYPE_FAULT;

    MMU_SetSecondLevelDescriptor((u32 *)0x4000000,(u32)ioMem|MMU_SLD_TYPE_TINY|/*MMU_CACHABLE|MMU_BUFFERABLE|*/0x20);
    for (i=0x4000400;i<0x4100000;i+=0x400)
        MMU_SetSecondLevelDescriptor((u32 *)i,MMU_SLD_TYPE_FAULT);



    uncachedZone=allocBoundary(4*1024,12); // small
    printk("uncachedZone %x\n",uncachedZone);
    *MMU_GetFirstLevelDescriptorRef((u32*)0xfff00000)=0x18f3000|MMU_FLD_BASE|MMU_FLD_TYPE_COARSE;
    for (i=0xfff00000;i!=0;i+=0x1000)
        MMU_SetSecondLevelDescriptor((u32 *)i,(u32)uncachedZone|MMU_SLD_TYPE_SMALL);

    uncachedZone[4]=0x04;
    uncachedZone[5]=0xf0;
    uncachedZone[6]=0x1f;
    uncachedZone[7]=0xe5;

    uncachedZone[0x12]=0x70;
    uncachedZone[0x13]=0x47;

    // invalidate tlb
    asm volatile(
        "mcr p15,0,%0,c8,c7,0\n"
    ::"r" (dummy));
    
    cache_clean();
    cache_invalidate(CACHE_ALL);
}

void setupGraphics(){
    for(int i = 0; i < 0x10000; i++) {
        int r,g,b,y,cr,cb;
        r=(i & 0x1f);
        g=((i & 0x3e0) >> 5);
        b=((i & 0x7c00) >> 10);
        r=r<<3;
        g=g<<3;
        b=b<<3;
        y=RGB2Y(r,g,b);
        cr=RGB2Cr(r,g,b);
        cb=RGB2Cb(r,g,b);

        systemColorMap32[i] = cb | y<<8 | cr<<16 | y<<24;
    }

    gfx_planeSetSize(VID2,240,160,32,GFX_SMODE_STD);
}

extern "C" {

extern void vector_SWI();
extern void vector_PrefetchAbort();
extern void vector_DataAbort();
extern void vector_IRQ();

volatile u32 GBAInterrupt;
volatile u32 VBlank;
u32 GBAContext[18];
u32 EmuContext[18];

void initGBAContext(){
    memset(&GBAContext[0],0,sizeof(GBAContext));
    GBAContext[13]=0x3007f00; // set sp
    GBAContext[15]=0x8000000; // jump to rom start
    GBAContext[16]=0x10; // switch to user with IRQ/FIQ in ARM mode
    GBAContext[17]=0x00;

    GBAInterrupt=0;
    VBlank=0;
}

void  switchContext(u32 * currentContext,u32 * newContext){
    asm volatile (
        "stmia r0,{r0-r14}\n"   // store common regs
        "mrs r2,cpsr\n"
        "str r2,[r0,#(16*4)]\n" // store cpsr
        "mrs r2,spsr\n"
        "str r2,[r0,#(17*4)]\n" // store spsr
        "ldr r2,=restore\n"
        "str r2,[r0,#(15*4)]\n" // store pc
        "ldr r2,[r1,#(16*4)]\n"
        "msr cpsr,r2\n"         // restore new cpsr
        "ldr r2,[r1,#(17*4)]\n"
        "msr spsr,r2\n"         // restore new spsr
        "ldmia r1,{r0-r15}\n"   // restore new regs + pc
        "restore:\n"
        "nop\n"
    );
}

void doGBAInterrupt(){
//    printk("doGBAInterrupt!\n");
    GBAInterrupt=1;
}

void doVBlank(){
//    printk("doVBlank!\n");
    VBlank=1;
}

int getCPSR(){
    int res;

    asm volatile(
        "mrs %0,cpsr\n"
    :"=r" (res));

    return res;
}

int getSPSR(){
    int res;

    asm volatile(
        "mrs %0,spsr\n"
    :"=r" (res));

    return res;
}

void printRegs(){
     register long _r0 asm("r0");
     register long _r1 asm("r1");
     register long _r2 asm("r2");
     register long _r3 asm("r3");
     register long _r4 asm("r4");
     register long _r5 asm("r5");
     register long _r6 asm("r6");
     register long _r7 asm("r7");
     register long _r8 asm("r8");
     register long _r9 asm("r9");
     register long _r10 asm("r10");
     register long _r11 asm("r11");
     register long _r12 asm("r12");
     register long _r13 asm("r13");
     register long _r14 asm("r14");
     register long _r15 asm("r15");

     printk("R0  %8x R1  %8x R2  %8x R3  %8x\nR4  %8x R5  %8x R6  %8x R7  %8x\nR8  %8x R9  %8x R10 %8x R11 %8x\nR12 %8x R13 %8x R14 %8x R15 %8x\nCPSR %8x SPSR %8x\n",
            _r0,_r1,_r2,_r3,_r4,_r5,_r6,_r7,_r8,_r9,_r10,_r11,_r12,_r13,_r14,_r15,getCPSR(),getSPSR());
}

void swiHandler(u32 swinum){
    SWI_PRINT("aoBoyAdvance SWI #%x (%x,%x,%x,%x) %x\n",swinum,registers[0],registers[1],registers[2],registers[3],getCPSR());

    CPUSoftwareInterrupt(swinum);
}

void prefetchAbortHandler(u32 instr,void * mva){
    ABORT_PRINT("aoBoyAdvance prefetch %x abort MVA=%x\n",instr,mva);
}

void dataAbortHandler(u32 instr,void * mva){
    bool thumb;
    bool write;
    int reg;
    int regs;
    int size;
    bool fail=false;
    bool increment=true;
    int i;
    u32 tmpaddr;

    // instruction analysis

    thumb=!(instr & 0xffff0000);

    if (thumb){
        write=!(instr&(1<<11));
        reg=instr & 0x7;
        size=-1;
        switch (instr&0xf000){
            case 0x7000: size=1; break;

            case 0x8000: size=2; break;

            case 0x6000:
            case 0x9000: size=4; break;

            case 0x5000:
                switch (instr&0x600){
                    case 0x000: size=4; break;

                    case 0x200: size=2; break;

                    case 0x400:
                    case 0x600: size=1; break;
                }
                break;

            case 0xc000: // ldmia / stmia ?
                increment=true;
                reg=-1;
                regs=instr&0xff;
                break;
        }
    }else{
        write=!(instr&(1<<20));
        reg=(instr>>12) & 0xf;

        if (instr&0xe000000) {
            size=(instr&(1<<22))?1:4;
        }else{
            size=(instr&(1<<5))?2:1;
        }

        if ((instr&0xe000000)==0x8000000){ // ldm / stm ?
            increment=instr&(1<<23);
            reg=-1;
            regs=instr&0xffff;
            size=-1;
        }
    }

    // instruction execution

    if (reg>=0){
        if (write){
            switch (size){
                case 1: CPUWriteByte((u32)mva,registers[reg]); break;
                case 2: CPUWriteHalfWord((u32)mva,registers[reg]); break;
                case 4: CPUWriteMemory((u32)mva,registers[reg]); break;
            }
        }else{
            switch (size){
                case 1: registers[reg]=CPUReadByte((u32)mva); break;
                case 2: registers[reg]=CPUReadHalfWord((u32)mva); break;
                case 4: registers[reg]=CPUReadMemory((u32)mva); break;
            }
        }
    }else{
        ABORT_PRINT("MULTIPLE %s regs=%x\n",(increment)?"increment":"decrement",regs);
        tmpaddr=(u32)mva;
        for(i=0;i<16;++i){
            if (regs&1<<i){
                if (write){
                    CPUWriteMemory(tmpaddr,registers[i]);
                }else{
                    registers[i]=CPUReadMemory(tmpaddr);
                }
                ABORT_PRINT("MULTIPLE r%d %x\n",i,tmpaddr);

                tmpaddr+=(increment)?4:-4;
            }
        }
    }

    if ((reg>12) || ((reg<0) && (regs&0xe000))) fail=true;

    if (fail || reg==-1 || true){
        ABORT_PRINT("aoBoyAdvance data %x abort MVA=%x addr=%x\n",instr,mva,registers[13]-8);
        ABORT_PRINT("%s %s r%d %d bytes\n",thumb?"thumb":"ARM",write?"write":"read",reg,size);
    }
}

void setupVectors(){
    *(u32 *)0x08=(((u32)vector_SWI - 0x10)>>2) | 0xea000000;
    *(u32 *)0x0C=(((u32)vector_PrefetchAbort - 0x14)>>2) | 0xea000000;
    *(u32 *)0x10=(((u32)vector_DataAbort - 0x18)>>2) | 0xea000000;
    *(u32 *)0x18=(((u32)vector_IRQ - 0x20)>>2) | 0xea000000;

    cache_clean();
    cache_invalidate(CACHE_CODE);
}

void lineTimerInterrupt(void * address){
//    register char * _sp asm("sp");

//    printk("TB %x %x %x\n",getCPSR(),getSPSR(),address);

//    print_data(_sp,128);
//    printRegs();

    switchContext(GBAContext,EmuContext);

//   printk("TA %x %x %x\n",getCPSR(),getSPSR(),get_lr);

//    print_data(_sp,128);
//    printRegs();
}

void dummyLineTimerInterrupt(int, pt_regs *){
//    printk("dummyLineTimerInterrupt\n");
    triggerLineTimer();
}

void setupLineTimer(){
    TMR_SET_SCAL(0, TMR1);
    TMR_SET_DIV(LINE_TIMER_DIVIDER,TMR1);
    TMR_SET_MODE(TMR_MODE_ONESHOT, TMR1);

    irq_ack(IRQ_TMR_1);
    irq_changeHandler(IRQ_TMR_1,dummyLineTimerInterrupt);
    irq_enable(IRQ_TMR_1);
}

void triggerLineTimer(){
    TMR_TRIGGER(TMR1);
}

int app_main(){
    icon_init();
    iniBrowser();
    gfx_openGraphics();

    setupLineTimer();
    setupMMU();
    setupVectors();
    setupGraphics();

    set_timer_status(HD_TIMER,TIMER_MODE_BAT,MODE_DISABLE);
    set_timer_status(HD_TIMER,TIMER_MODE_DC,MODE_DISABLE);

    for(;;){
        gfx_planeHide(VID2);
        gfx_planeShow(BMAP1);

        if (browser_simpleBrowse("/roms/GBA",romPath)!=MED_OK) break;
        printk("%s\n",romPath);

        if (!CPULoadRom(romPath)){
            printk("CPULoadRom failed\n");
            break;
        }

        ata_StopHD(0);

        cache_clean();
        cache_invalidate(CACHE_CODE);

        cartridgeType = 0;
        emulator = GBASystem;

        initGBAContext();
        CPUInit("", useBios);
        CPUReset();

        gfx_planeSetBufferOffset(VID2,pix);
        gfx_planeHide(BMAP1);
        gfx_planeShow(VID2);

        //cache_enable(CACHE_ALL,false);
        //cache_enable(CACHE_CODE,false);
        //cache_enable(CACHE_DATA,false);

        emulating = 1;
        while(emulating) {
            emulator.emuMain(emulator.emuCount);
            if (btn_readState()&BTMASK_OFF) emulating=0;
        }
        emulating = 0;

        emulator.emuCleanUp();
    }

    gfx_closeGraphics();

    return 1;
}

}
