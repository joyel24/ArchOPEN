/*
*   kernel/driver/clkc_DM270.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/io.h>
#include <kernel/kernel.h>
#include <kernel/delay.h>

#include <driver/hardware.h>
#include <driver/clkc.h>

int clkc_dividers[]={2,3,4};
int clkc_dividersCount=sizeof(clkc_dividers)/sizeof(int);

int clkc_pllAddress[]={CLKC_PLL0,CLKC_PLL1};
int clkc_pllSel[]={CLKC_SEL_ARM_PLL1,CLKC_SEL_DSP_PLL1,CLKC_SEL_SDRAM_PLL1,CLKC_SEL_ACCEL_PLL1};

int clkc_divMask[]={CLKC_DIV_ARM_MASK,CLKC_DIV_DSP_MASK,CLKC_DIV_SDRAM_MASK,CLKC_DIV_ACCEL_MASK};
int clkc_divShift[]={CLKC_DIV_ARM_SHIFT,CLKC_DIV_DSP_SHIFT,CLKC_DIV_SDRAM_SHIFT,CLKC_DIV_ACCEL_SHIFT};

void clkc_getClockParameters(int clock,int * m,int * n,int * div){
    int addr;

    // divider
    *div=((inw(CLKC_PLL_DIVIDER)&clkc_divMask[clock])>>clkc_divShift[clock])+1;

    // M & N
    addr=clkc_pllAddress[clkc_getPllNum(clock)];

    *m=((inw(addr)&CLKC_PLL_M_MASK)>>CLKC_PLL_M_SHIFT)+1;
    *n=((inw(addr)&CLKC_PLL_N_MASK)>>CLKC_PLL_N_SHIFT)+1;
}

void clkc_setClockParameters(int clock,int m,int n,int div){
    int addr;
    int tmp;
    
    // Setting the Bypass state for all clocks
    tmp=inw(CLKC_BYPASS);
    outw(0x1111,CLKC_BYPASS);

    // divider
    outw((inw(CLKC_PLL_DIVIDER)&~clkc_divMask[clock]) | (((div-1)<<clkc_divShift[clock])&clkc_divMask[clock]),CLKC_PLL_DIVIDER);

    // M & N
    addr=clkc_pllAddress[clkc_getPllNum(clock)];

    outw((((m-1)<<CLKC_PLL_M_SHIFT)&CLKC_PLL_M_MASK) | (((n-1)<<CLKC_PLL_N_SHIFT)&CLKC_PLL_N_MASK),addr);
    
    // wait a little for PLL to be in a sane state
    mdelay(100);
    
    // Canceling the Bypass state
    outw(tmp,CLKC_BYPASS);
}

int clkc_getPllNum(int clock){
    return (inw(CLKC_PLL_SELECT)&clkc_pllSel[clock])?1:0;
}

void clkc_setPllNum(int clock,int pll){
    if(pll){
        outw(inw(CLKC_PLL_SELECT)|clkc_pllSel[clock],CLKC_PLL_SELECT);
    }else{
        outw(inw(CLKC_PLL_SELECT)&~clkc_pllSel[clock],CLKC_PLL_SELECT);
    }
}

bool clkc_getPowerState(int device){
    //TODO
    return false;
}

void clkc_setPowerState(int device,bool state){
    //TODO
}
