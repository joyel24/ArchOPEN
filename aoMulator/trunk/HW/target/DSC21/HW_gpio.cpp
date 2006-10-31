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
    switch(addr)
    {
        case GPIO_DIRECTION0:                                              /* DIR 0 */
            DEBUG_HW(GPIO_HW_DEBUG,"GPIO read direction 0 : %04x\n",DIR[0]);
            return DIR[0];
        case GPIO_DIRECTION1:                                              /* DIR 1 */
            DEBUG_HW(GPIO_HW_DEBUG,"GPIO read direction 1 : %04x\n",DIR[1]);
            return DIR[1];
        case GPIO_INVERT0:                                              /* INV 0 */
            DEBUG_HW(GPIO_HW_DEBUG,"GPIO read inversion 0 : %04x\n",INV[0]);
            return INV[0];
        case GPIO_INVERT1:                                              /* INV 1 */
            DEBUG_HW(GPIO_HW_DEBUG,"GPIO read inversion 1 : %04x\n",INV[1]);
            return INV[1];
        case GPIO_BITSET0:                                              /* SET 0 */
            {
                uint32_t tmp_val =0;
                for(int k=0;k<0x10;k++)
                {
                    if(DIR[0] & gpio_mask[k])
                    {
                        int res=IS_GPIO_SET(k);
                        if((res && !(INV[0] & gpio_mask[k])) || (!res && (INV[0] & gpio_mask[k])))
                            tmp_val |= gpio_mask[k];
                    }
                    else
                    {
                        if (IS_GPIO_SET(k)) tmp_val |= gpio_mask[k];
                    }

                }
                DEBUG_HW(GPIO_HW_DEBUG,"GPIO read state 0: %04x\n",tmp_val);
                return tmp_val;
            }
        case GPIO_BITSET1:                                              /* SET 1 */
            {
                uint32_t tmp_val =0;
                for(int k=0;k<0x10;k++)
                {
                    if(DIR[1] & gpio_mask[k])
                    {
                        int res=IS_GPIO_SET(k+0x10);
                        if((res && !(INV[1] & gpio_mask[k])) || (!res && (INV[1] & gpio_mask[k])))
                            tmp_val |= gpio_mask[k];
                    }
                    else
                    {
                        if (IS_GPIO_SET(k+0x10)) tmp_val |= gpio_mask[k];
                    }
                }
                DEBUG_HW(GPIO_HW_DEBUG,"GPIO read state 1: %04x\n",tmp_val);
                return tmp_val;
            }
        case GPIO_BITCLEAR0:                                              /* CLR 0 */
            {
                uint32_t tmp_val =0;
                for(int k=0;k<0x10;k++)
                {
                    if(DIR[0] & gpio_mask[k])
                    {
                        int res=IS_GPIO_SET(k);
                        if((res && !(INV[0] & gpio_mask[k])) || (!res && (INV[0] & gpio_mask[k])))
                            tmp_val |= gpio_mask[k];
                    }
                    else
                    {
                        if (IS_GPIO_SET(k)) tmp_val |= gpio_mask[k];
                    }

                }
                DEBUG_HW(GPIO_HW_DEBUG,"GPIO read state 0: %04x\n",tmp_val);
                return tmp_val;
            }
        case GPIO_BITCLEAR1:                                              /* CLR 1 */
            {
                uint32_t tmp_val =0;
                for(int k=0;k<0x10;k++)
                {
                    if(DIR[1] & gpio_mask[k])
                    {
                        int res=IS_GPIO_SET(k+0x10);
                        if((res && !(INV[1] & gpio_mask[k])) || (!res && (INV[1] & gpio_mask[k])))
                            tmp_val |= gpio_mask[k];
                    }
                    else
                    {
                        if (IS_GPIO_SET(k+0x10)) tmp_val |= gpio_mask[k];
                    }

                }
                DEBUG_HW(GPIO_HW_DEBUG,"GPIO read state 1: %04x\n",tmp_val);
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
    switch(addr)
    {
        case GPIO_DIRECTION0:                                              /* DIR 0 */
            {
                int tmp = val ^ DIR[0];          // compute what has changed
                int tmp2 = val;
                DIR[0] = val & 0xFFFF;
                //DEBUG_HW(GPIO_HW_DEBUG,"GPIO write DIR_0 (%04x): ",val);
                for(int k = 0; k<0x10;k++)
                {
                    if(tmp&0x1)
                    {
                        //DEBUG_HW(GPIO_HW_DEBUG,"%s(GPIO%x)=> %s",gpio_str[k],k,(tmp2&0x1)?"READ":"WRITE");
                        if((tmp2&0x1)!=0x1) /* we are in write mode now */
                        {
                            if(BITSET[0] & gpio_mask[k])
                            {
                                //DEBUG_HW(GPIO_HW_DEBUG," & SET");
                                SET_GPIO(k);
                            }

                            if(BITCLR[0] & gpio_mask[k])
                            {
                                //DEBUG_HW(GPIO_HW_DEBUG," & CLR");
                                CLR_GPIO(k);
                            }
                        }
                        GPIO_DIR_CHG(k,tmp2&0x1);
                        //DEBUG_HW(GPIO_HW_DEBUG,"|");
                    }
                    tmp=tmp >> 1;
                    tmp2=tmp2 >> 1;
                }
                //DEBUG_HW(GPIO_HW_DEBUG,"\n");
                break;
            }
        case GPIO_DIRECTION1:                                              /* DIR 1 */
            {
                int tmp = val ^ DIR[1];          /* compute what has changed */

                int tmp2 = val;
                DIR[1] = val & 0xFFFF;

                for(int k = 0; k<0x10;k++)
                {
                    if(tmp&0x1)
                    {
                        //DEBUG_HW(GPIO_HW_DEBUG,"%s(GPIO%x)=> %s",gpio_str[k+0x10],k+0x10,(tmp2&0x1)?"READ":"WRITE");
                        if((tmp2&0x1)!=0x1) /* we are in write mode now */
                        {
                            if(BITSET[1] & gpio_mask[k])
                            {
                                //DEBUG_HW(GPIO_HW_DEBUG," & SET");
                                SET_GPIO(k+0x10);
                            }

                            if(BITCLR[1] & gpio_mask[k])
                            {
                                //DEBUG_HW(GPIO_HW_DEBUG," & CLR");
                                CLR_GPIO(k+0x10);
                            }
                        }
                        GPIO_DIR_CHG(k+0x10,tmp2&0x1);
                        //DEBUG_HW(GPIO_HW_DEBUG,"|");
                    }
                    tmp=tmp >> 1;
                    tmp2=tmp2 >> 1;
                }
                //DEBUG_HW(GPIO_HW_DEBUG,"\n");
                break;
            }
        case GPIO_INVERT0:                                              /* INV 0 */
            DEBUG_HW(GPIO_HW_DEBUG,"GPIO write INV_0 : %04x change to %04x\n",INV[0],val&0xFFFF);
            INV[0]=val&0xFFFF;
            break;
        case GPIO_INVERT1:                                              /* INV 1 */
            DEBUG_HW(GPIO_HW_DEBUG,"GPIO write INV_1 : %04x change to %04x\n",INV[1],val&0xFFFF);
            INV[1]=val&0xFFFF;
            break;
        case GPIO_BITSET0:                                              /* SET 0 */
            {
                //DEBUG_HW(GPIO_HW_DEBUG,"GPIO set 0: %04x (with DIR mask:%08x): ",val,DIR_0);
                //val &= ~DIR_0;
                for(int k=0;k<0x10;k++)
                {
                    if(val & 0x1)
                    {
                        //DEBUG_HW(GPIO_HW_DEBUG,"%s(GPIO%x)",gpio_str[k],k);
                        if(DIR[0] & gpio_mask[k])
                        {
                            BITSET[0] |=gpio_mask[k];
                            //DEBUG_HW(GPIO_HW_DEBUG,"(delayed)");
                        }
                        else
                            SET_GPIO(k);
                        //DEBUG_HW(GPIO_HW_DEBUG,"|");
                    }
                    val = val >> 1;
                }
            }
            //DEBUG_HW(GPIO_HW_DEBUG,"\n");
            break;
        case GPIO_BITSET1:                                              /* SET 1 */
            {
                //DEBUG_HW(GPIO_HW_DEBUG,"GPIO set 1: %04x (with DIR mask:%08x): ",val,DIR_1);
                //val &= ~DIR_1;
                for(int k=0;k<0x10;k++)
                {
                    if(val & 0x1)
                    {
                        //DEBUG_HW(GPIO_HW_DEBUG,"%s(GPIO%x)",gpio_str[k+0x10],k+0x10);
                        if(DIR[1] & gpio_mask[k])
                        {
                            BITSET[1] |=gpio_mask[k];
                            //DEBUG_HW(GPIO_HW_DEBUG,"(delayed)");
                        }
                        else
                            SET_GPIO(k+0x10);
                        //DEBUG_HW(GPIO_HW_DEBUG,"|");
                    }
                    val = val >> 1;
                }
            }
            //DEBUG_HW(GPIO_HW_DEBUG,"\n");
            break;
        case GPIO_BITCLEAR0:                                              /* CLR 0 */
            {
                //DEBUG_HW(GPIO_HW_DEBUG,"GPIO clr 0: %04x (with DIR mask:%08x): ",val,DIR_0);
                //val &= ~DIR_0;
                for(int k=0;k<0x10;k++)
                {
                    if(val & 0x1)
                    {
                        //DEBUG_HW(GPIO_HW_DEBUG,"%s(GPIO%x)",gpio_str[k+0x10],k+0x10);
                        if(DIR[0] & gpio_mask[k])
                        {
                            BITCLR[0] |=gpio_mask[k];
                            //DEBUG_HW(GPIO_HW_DEBUG,"(delayed)");
                        }
                        else
                            CLR_GPIO(k);
                        //DEBUG_HW(GPIO_HW_DEBUG,"|");
                    }
                    val = val >> 1;
                }
            }
            //DEBUG_HW(GPIO_HW_DEBUG,"\n");
            break;
        case GPIO_BITCLEAR1:                                              /* CLR 1 */
            {
                //DEBUG_HW(GPIO_HW_DEBUG,"GPIO clr 1: %04x (with DIR mask:%08x): ",val,DIR_1);
                //val &= ~DIR_1;
                for(int k=0;k<0x10;k++)
                {
                    if(val & 0x1)
                    {
                        //DEBUG_HW(GPIO_HW_DEBUG,"%s(GPIO%x)",gpio_str[k+0x10],k+0x10);
                        if(DIR[1] & gpio_mask[k])
                        {
                            BITCLR[1] |=gpio_mask[k];
                            //DEBUG_HW(GPIO_HW_DEBUG,"(delayed)");
                        }
                        else
                            CLR_GPIO(k+0x10);
                        //DEBUG_HW(GPIO_HW_DEBUG,"|");
                    }
                    val = val >> 1;
                }
            }
            //DEBUG_HW(GPIO_HW_DEBUG,"\n");
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
