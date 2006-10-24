/*
*   HW_gpio.cpp
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

#include <emu.h>

#include <HW/HW_gpio.h>

char * gpio_str[] = GPIO_STR;

uint32_t gpio_mask[]={  0X00000001, 0X00000002,0X00000004,0X00000008,
            0X00000010, 0X00000020,0X00000040,0X00000080,
            0X00000100, 0X00000200,0X00000400,0X00000800,
            0X00001000, 0X00002000,0X00004000,0X00008000,

            0X00010000, 0X00020000,0X00040000,0X00080000,
            0X00100000, 0X00200000,0X00400000,0X00800000,
            0X01000000, 0X02000000,0X04000000,0X08000000,
            0X10000000, 0X20000000,0X40000000,0X80000000};

uint32_t HW_gpio::read(uint32_t addr,int size)
{
    int num;
    switch(addr)
    {
        case GPIO_DIRECTION0: /* DIR */
        case GPIO_DIRECTION1:
        case GPIO_DIRECTION2:
            num = (addr - GPIO_DIRECTION0)/2;
            DEBUG_HW(GPIO_HW_DEBUG,"GPIO read direction %d : %04x\n",num,DIR[num]);
            return DIR[num];

        case GPIO_INVERT0:   /* INV */
        case GPIO_INVERT1:
        case GPIO_INVERT2:
            num = (addr - GPIO_INVERT0)/2;
            DEBUG_HW(GPIO_HW_DEBUG,"GPIO read inversion %d : %04x\n",num,INV[num]);
            return INV[num];
        case GPIO_BITSET0:   /* SET */
        case GPIO_BITSET1:
        case GPIO_BITSET2:
            {
                num = (addr - GPIO_BITSET0)/2;
                uint32_t tmp_val =0;
                for(int k=0;k<0x10;k++)
                {
                    if(DIR[num] & gpio_mask[k])
                    {
                        int res=IS_GPIO_SET(k+num*0x10);
                        if((res && !(INV[num] & gpio_mask[k])) || (!res && (INV[num] & gpio_mask[k])))
                            tmp_val |= gpio_mask[k];
                    }
                    else
                    {
                        if (IS_GPIO_SET(k+num*0x10)) tmp_val |= gpio_mask[k];
                    }

                }
                DEBUG_HW(GPIO_HW_DEBUG,"GPIO read state %d: %04x\n",num,tmp_val);
                return tmp_val;
            }
        case GPIO_BITCLEAR0:  /* CLR */
        case GPIO_BITCLEAR1:
        case GPIO_BITCLEAR2:
            {
                uint32_t tmp_val =0;
                num = (addr - GPIO_BITCLEAR0)/2;
                for(int k=0;k<0x10;k++)
                {
                    if(DIR[num] & gpio_mask[k])
                    {
                        int res=IS_GPIO_SET(k+num*0x10);
                        if((res && !(INV[num] & gpio_mask[k])) || (!res && (INV[num] & gpio_mask[k])))
                            tmp_val |= gpio_mask[k];
                    }
                    else
                    {
                        if (IS_GPIO_SET(k+num*0x10)) tmp_val |= gpio_mask[k];
                    }
                }
                DEBUG_HW(GPIO_HW_DEBUG,"GPIO read state %d: %04x\n",num,tmp_val);
                return tmp_val;
            }
        case GPIO_ENABLE_IRQ:                                              /* IRQ */
            DEBUG_HW(GPIO_HW_DEBUG,"GPIO read IRQ : %04x\n",IRQPORT);
            return IRQPORT;
            break;
        case GPIO_FSEL:                                              /* FSEL */
            DEBUG_HW(GPIO_HW_DEBUG,"GPIO read FSEL : %04x\n",FSEL);
            return FSEL;
            break;
        case GPIO_BITRATE:                                              /* BITRATE */
            DEBUG_HW(GPIO_HW_DEBUG,"GPIO read BITRATE : %04x\n",BITRATE);
            return BITRATE;
            break;
    }

    return 0;
}

void HW_gpio::write(uint32_t addr,uint32_t val,int size)
{
    int num;
    switch(addr)
    {
        case GPIO_DIRECTION0:  /* DIR 0 */
        case GPIO_DIRECTION1:
        case GPIO_DIRECTION2:
            {
                int tmp = val ^ DIR[0];          // compute what has changed
                int tmp2 = val;
                num = (addr - GPIO_DIRECTION0)/2;
                DIR[num] = val & 0xFFFF;
                for(int k = 0; k<0x10;k++)
                {
                    if(tmp&0x1)
                    {
                        if((tmp2&0x1)!=0x1) /* we are in write mode now */
                        {
                            if(BITSET[num] & gpio_mask[k])
                            {
                                SET_GPIO(k+num*0x10);
                            }

                            if(BITCLR[num] & gpio_mask[k])
                            {
                                CLR_GPIO(k+num*0x10);
                            }
                        }
                        GPIO_DIR_CHG(k+num*0x10,tmp2&0x1);
                    }
                    tmp=tmp >> 1;
                    tmp2=tmp2 >> 1;
                }
                break;
            }
        case GPIO_INVERT0: /* INV */
        case GPIO_INVERT1:
        case GPIO_INVERT2:
            num = (addr - GPIO_INVERT0)/2;
            DEBUG_HW(GPIO_HW_DEBUG,"GPIO write INV_%d : %04x change to %04x\n",num,INV[num],val&0xFFFF);
            INV[num]=val&0xFFFF;
            break;
        case GPIO_BITSET0: /* SET */
        case GPIO_BITSET1:
        case GPIO_BITSET2:
            {
                num = (addr - GPIO_BITSET0)/2;
                for(int k=0;k<0x10;k++)
                {
                    if(val & 0x1)
                    {
                        if(DIR[num] & gpio_mask[k])
                        {
                            BITSET[num] |=gpio_mask[k];
                        }
                        else
                            SET_GPIO(k+num*0x10);
                    }
                    val = val >> 1;
                }
            }
            break;

        case GPIO_BITCLEAR0:  /* CLR */
        case GPIO_BITCLEAR1:
        case GPIO_BITCLEAR2:
            {
                num = (addr - GPIO_BITCLEAR0)/2;
                for(int k=0;k<0x10;k++)
                {
                    if(val & 0x1)
                    {
                        if(DIR[num] & gpio_mask[k])
                        {
                            BITCLR[num] |=gpio_mask[k];
                        }
                        else
                            CLR_GPIO(k+num*0x10);
                    }
                    val = val >> 1;
                }
            }
            break;
        case GPIO_ENABLE_IRQ:                                              /* IRQ */
            IRQPORT=val & 0xFF;
            DEBUG_HW(GPIO_HW_DEBUG,"GPIO write IRQ : %04x\n",IRQPORT);
            break;
        case GPIO_FSEL:                                              /* FSEL */
            FSEL = val & 0x7FFF;
            DEBUG_HW(GPIO_HW_DEBUG,"GPIO write FSEL : %04x\n",FSEL);
            break;
        case GPIO_BITRATE:                                              /* BITRATE */
            BITRATE = val & 0xFFFF;
            DEBUG_HW(GPIO_HW_DEBUG,"GPIO write BITRATE : %04x\n",BITRATE);
            break;
    }
}
