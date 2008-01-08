/*
*   kernel/driver/lcd.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <driver/lcd.h>
#include <driver/fm_remote.h>
#include <driver/energy.h>

#include <kernel/kernel.h>
#include <kernel/delay.h>

#include <fs/cfg_file.h>

int lcd_brightness;
int lcd_state;

#define LCD_FADE_ENABLE_DELAY  5
#define LCD_FADE_DISABLE_DELAY 5

void lcd_init()
{
    
    lcd_setBrightness(LCD_MAX_BRIGHTNESS);
    lcd_enable();
    arch_lcd_init();
}

void lcd_brightIni(void)
{
    CFG_DATA * cfg;
    int needWrite=0;
     
    cfg=cfg_readFile("/medios/medios.cfg");
            
    if(!cfg)
    {
        printk("Can't open cfg file\n");
        /* creating default */
        cfg=cfg_newFile();
        if(!cfg)
        {
            printk("Can't create new cfg file\n");
            return;
        }           
        
        cfg_writeInt(cfg,"lcdBrightness",LCD_MAX_BRIGHTNESS);
        lcd_setBrightness(LCD_MAX_BRIGHTNESS);         
        needWrite=1;   
    }
    else
    {
        if(cfg_itemExists(cfg,"lcdBrightness"))
        {
            lcd_setBrightness(cfg_readInt(cfg,"lcdBrightness"));
        }
        else
        {
            cfg_writeInt(cfg,"lcdBrightness",LCD_MAX_BRIGHTNESS);
            lcd_setBrightness(LCD_MAX_BRIGHTNESS);         
            needWrite=1;   
        }   
    }
    if(needWrite) cfg_writeFile(cfg,"/medios/medios.cfg");
    cfg_clear(cfg);
}

void lcd_fadeEnable()
{
    int i;
    lcd_state=1;
    arch_lcd_enable();
    for(i=0;i<lcd_brightness;i++)
    {
        arch_lcd_setBrigthness(i);
        mdelay(LCD_FADE_ENABLE_DELAY);
    }
    arch_lcd_setBrigthness(lcd_brightness);
}

void lcd_fadeDisable()
{
    int i;

    for(i=lcd_brightness;i>=0;i--)
    {
        arch_lcd_setBrigthness(i);
        mdelay(LCD_FADE_DISABLE_DELAY);
    }
    arch_lcd_setBrigthness(0);
    lcd_state=0;
    arch_lcd_disable();
}

void lcd_enable()
{
    lcd_state=1;
    arch_lcd_setBrigthness(lcd_brightness);
    arch_lcd_enable();
}

void lcd_disable()
{
    lcd_state=0;
    arch_lcd_setBrigthness(0);
    arch_lcd_disable();
}

int lcd_enabled(){
    return lcd_state;
}

void lcd_setBrightness(int br){
    lcd_brightness=br;
    if (lcd_state) arch_lcd_setBrigthness(br);
}

int lcd_getBrightness(){
    return lcd_brightness;
}

void lcd_keyPress(void)
{
    lcd_state=1; /*needed here i norder to launch timer*/
    kernel_doCmd(CMD_LCD_ENABLE);
    if(FM_is_connected())
        FM_lightsON();
    lcd_launchTimer();
}
