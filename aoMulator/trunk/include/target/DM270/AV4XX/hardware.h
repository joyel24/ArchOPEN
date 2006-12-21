/*
*   hardware.h
*
*    emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#define ARCH_NAME         "AV4XX"
#define CHIP_NAME         "DM270"

//#define homebrew //or let it be ;-)

/************************************************************ Memory Zone */

#define RESET_VECTOR      0x00000000

#define IRAM_START        0x00000004
#define IRAM_END          0x00008000
#define TI_REG_START      0x00030000
#define TI_REG_END        0x00040000

/*IRAM should be the same on all arch */



#define FLASH_START       0x0100000
#define FLASH_END         0x0200000
#define FLASH_LOAD_OFFSET   0x00000000

#define SDRAM_START       0x00900000
#define SDRAM_END         0x01900000

#ifndef homebrew
#define SDRAM_LOAD_OFFSET   0x0000000 // original;
#else
#define SDRAM_LOAD_OFFSET   0x310000 //gmini400 homebrew;
#endif

//#define HAS_VID0
#define HAS_LCD
//#define HAS_UART_XWIN

/************************************************************ initial state */

#ifndef homebrew
#define START_ADDR        SDRAM_START //original
#else
#define START_ADDR        0xC10000  //gmini400 homebrew;
#endif

#define STACK_INIT        (IRAM_END - 0x4)
#define RESET_INIT_VAL    0xEA03FFFE
#define INIT_MODE         M_SVC

/* Special init for this arch: enable monitor command line mode */
#define ARCH_INIT   \
{                   \
    write(0x10600f,0x55,1);\
}

/************************************************************ HW config */

/********************** TIMERS    ****************************************/
#define TMR0_START   TI_REG_START+0x000
#define TMR1_START   TI_REG_START+0x080
#define TMR2_START   TI_REG_START+0x100
#define TMR3_START   TI_REG_START+0x180

/********************** UART 0   ****************************************/
#define UART0_START   TI_REG_START+0x300
#define UART0_END     TI_REG_START+0x310

/********************** UART 1   ****************************************/
#define UART1_START   TI_REG_START+0x380
#define UART1_END     TI_REG_START+0x390

/********************** WDT   ****************************************/
#define WDT_START   TI_REG_START+0x400
#define WDT_END     TI_REG_START+0x406
#define WDT_FIQ     0x1b
/********************** IRQ      ****************************************/
#define IRQ_START   TI_REG_START+0x500
#define IRQ_END     TI_REG_START+0x57f

#define FIQ_0    0x0
#define FIQ_1    0x2
#define FIQ_2    0x4
#define IRQ_0    0x8
#define IRQ_1    0xA
#define IRQ_2    0xC

#define IRQ_OFFSET 4

#define INT_STATUS      (IRQ_START+0x0)
#define INT_FIQ_ENTRY   (IRQ_START+0x10)
#define INT_IRQ_ENTRY   (IRQ_START+0x18)
#define INT_FSEL        (IRQ_START+0x20)
#define INT_ENABLE      (IRQ_START+0x28)
#define INT_EABASE      (IRQ_START+0x38)
#define INT_INTPRIO     (IRQ_START+0x40)

#define NB_INT        39

#define NB_FIQ        3
#define NB_IRQ        3

#define NB_OF_REG     NB_FIQ+NB_IRQ

#define REG_NUM(irq) (irq<16?0:irq<32?1:2)
#define REAL_NUM(irq) (irq<16?irq:irq<32?irq-16:irq-32) //get the irq line number in the reg

/********************** GPIO     ****************************************/
#define GPIO_START   TI_REG_START+0x580
#define GPIO_END     TI_REG_START+0x5FF

#define NB_GPIO_REG                        3

#define GPIO_DIRECTION0                    (GPIO_START+0x00)  // GIO 0-15
#define GPIO_DIRECTION1                    (GPIO_START+0x02)  // GIO 16-31
#define GPIO_DIRECTION2                    (GPIO_START+0x04)
#define GPIO_INVERT0                       (GPIO_START+0x06)  // GIO 0-15
#define GPIO_INVERT1                       (GPIO_START+0x08)  // GIO 16-31
#define GPIO_INVERT2                       (GPIO_START+0x0a)
#define GPIO_BITSET0                       (GPIO_START+0x0c)  // GIO 0-15
#define GPIO_BITSET1                       (GPIO_START+0x0e)  // GIO 16-31
#define GPIO_BITSET2                       (GPIO_START+0x10)
#define GPIO_BITCLEAR0                     (GPIO_START+0x12)  // GIO 0-15
#define GPIO_BITCLEAR1                     (GPIO_START+0x14)  // GIO 16-31
#define GPIO_BITCLEAR2                     (GPIO_START+0x16)
#define GPIO_ENABLE_IRQ                    (GPIO_START+0x24)  // GIO 0-7
#define GPIO_FSEL                          (GPIO_START+0x26)
#define GPIO_BITRATE                       (GPIO_START+0x28)

#define GPIO_STR  \
{ "UKN", "ON", "OFF", "DC Power", "UKN", "UKN", "UKN", "CF conn", \
  "I2C_DA", "I2C_CLK", "I2C_slave_DA", "USB conn", "UKN", "I2C_slave_CLK", "BTN LCD2", "BTN LCD1", \
                    \
  "UKN", "UKN", "UKN", "UKN", "UKN", "UKN", "UKN", "UKN", \
  "UKN", "UKN", "UKN", "UKN", "UKN", "UKN", "UKN", "UKN", \
                    \
  "UKN", "UKN", "UKN", "UKN", "UKN", "UKN", "UKN", "UKN", \
  "UKN", "UKN", "UKN", "UKN", "UKN", "UKN", "UKN", "UKN", \
}

#define GPIO_ON_NUM    0x01
#define GPIO_OFF_NUM   0x02
#define GPIO_LCD_SWT_1 0x0F
#define GPIO_LCD_SWT_2 0x0E

#define GPIO_I2C_SDA  0x08
#define GPIO_I2C_SCL  0x09

#define GPIO_I2C_SLAVE_SDA  0x0A
#define GPIO_I2C_SLAVE_SCL  0x0D

#define GIO_POWER_CONNECTED   0x03
#define GIO_USB_CONNECTED     0x0b
#define GIO_CF_CONNECTED      0x07

#define DSP_START   TI_REG_START+0x600
#define DSP_END     TI_REG_START+0x610

/********************** I2C     ****************************************/

/********************** OSD     ****************************************/
#define OSD_START   TI_REG_START+0x680
#define OSD_END     TI_REG_START+0x700
//#define OSD_START   0x18d0300
//#define OSD_END     0x18d0320

/********************** CCD     ****************************************/
#define CCD_START   TI_REG_START+0x700
#define CCD_END     TI_REG_START+0x73A

/********************** PREVIEW ****************************************/
#define PREVIEW_START   TI_REG_START+0x780
#define PREVIEW_END     TI_REG_START+0x7EC

/********************** PAL NTSC Encoder *******************************/
#define PAL_NTSC_ENC_START   TI_REG_START+0x800
#define PAL_NTSC_ENC_END     TI_REG_START+0x84e

/********************** CLOCK    ****************************************/
#define CLOCK_START   TI_REG_START+0x880
#define CLOCK_END     TI_REG_START+0x890


#define  MEM_CFG_START TI_REG_START+0xA00
#define  MEM_CFG_STOP TI_REG_START+0xA30


/********************** ECR      ****************************************/
#define ECR_START   TI_REG_START+0x900
#define ECR_END     TI_REG_START+0x906

/********************** TI VERSION **************************************/
#define TI_VER_START   TI_REG_START+0x906
#define TI_VER_END     TI_REG_START+0x908

#define TI_VER 0x32

/********************** HW_30A24 ****************************************/

/* this HW is used on the av for the usb/DC/HD status */
#define HAS_HW_30A24


/********************** DMA      ****************************************/
#define DMA_START   TI_REG_START+0xa38
#define DMA_END     TI_REG_START+0xa48

#define DMA_SDRAM_TO_ATA  0x53
#define DMA_ATA_TO_SDRAM  0x35

#define DMA_SRC_HI  DMA_START+0x0
#define DMA_SRC_LO  DMA_START+0x2
#define DMA_DST_HI  DMA_START+0x4
#define DMA_DST_LO  DMA_START+0x6
#define DMA_SIZE    DMA_START+0x8
#define DMA_SEL     DMA_START+0xa
#define DMA_CTL     DMA_START+0xc
#define DMA_GO      DMA_START+0xc

#define INT_DMA     15

/********************** IDE ****************************************/
#define IDE_BASE    0x05100000
#define IDE_END     0x05100100

#define IDE_DATA                          (IDE_BASE+0x20)
#define IDE_ERROR                         (IDE_BASE+0x22)
#define IDE_NSECTOR                       (IDE_BASE+0x24)
#define IDE_SECTOR                        (IDE_BASE+0x26)
#define IDE_LCYL                          (IDE_BASE+0x28)
#define IDE_HCYL                          (IDE_BASE+0x2a)
#define IDE_SELECT                        (IDE_BASE+0x2c)
#define IDE_CONTROL                       (IDE_BASE+0x1c)
#define IDE_STATUS                        (IDE_BASE+0x2e)
#define IDE_COMMAND                       (IDE_BASE+0x2e)

/********************** CPLD     ****************************************/
#define CPLD_START       0x02000000
#define CPLD_END         0x03000000
#define CPLD_PORT_OFFSET 0x00500000

/********************** LCD      ****************************************/
#define SCREEN_WIDTH  320
#define SCREEN_HEIGHT 240
