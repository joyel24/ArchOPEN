/*
*   hardware.h
*
*   JBMM emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#define ARCH_NAME         "JBMM"
#define CHIP_NAME         "DSC21"

/************************************************************ Memory Zone */

#define RESET_VECTOR      0x00000000

#define IRAM_START        0x00000000
#define IRAM_END          0x00008000
#define TI_REG_START      0x00030000
#define TI_REG_END        0x00040000

/*IRAM should be the same on all arch */



#define FLASH_START       0x100000
#define FLASH_END         0x180000
#define FLASH_LOAD_OFFSET 0x000000

#define SDRAM_START       0x08000000
#define SDRAM_END         0x09000000
#define SDRAM_LOAD_OFFSET 0x00000000


/************************************************************ initial state */

#define START_ADDR        SDRAM_START
//#define START_ADDR        FLASH_START
#define STACK_INIT        (IRAM_END - 0x4)
#define RESET_INIT_VAL    0xEA03FFFE
#define INIT_MODE         M_SVC

//#define HAS_VID0
#define HAS_LCD
#define DISP_BMAP 2
//#define HAS_UART_XWIN

/************************************************************ HW config */

/********************** TIMERS    ****************************************/
#define TMR0_START   TI_REG_START+0x000
#define TMR1_START   TI_REG_START+0x080
#define TMR2_START   TI_REG_START+0x100
#define TMR3_START   TI_REG_START+0x180

/********************** UART 0   ****************************************/
#define UART0_START   TI_REG_START+0x380
#define UART0_END     TI_REG_START+0x390

/********************** UART 1   ****************************************/
#define UART1_START   TI_REG_START+0x400
#define UART1_END     TI_REG_START+0x410

/********************** WDT   ****************************************/
#define WDT_START   TI_REG_START+0x300
#define WDT_END     TI_REG_START+0x310

// need this to fool do_Int
#define WDT_FIQ     0xFF

/********************** IRQ      ****************************************/
#define IRQ_START   TI_REG_START+0x580
#define IRQ_END     TI_REG_START+0x5D0

#define FIQ_0    0x0
#define IRQ_0    0x2
#define IRQ_1    0x0

#define INT_FIQ_STATUS  (IRQ_START+0x00)
#define INT_IRQ_STATUS  (IRQ_START+0x02)
#define INT_FIQ_ENABLE  (IRQ_START+0x20)
#define INT_IRQ_ENABLE  (IRQ_START+0x22)
#define INT_ID          (IRQ_START+0x40)
#define INT_RAW         (IRQ_START+0x42)

#define IRQ_OFFSET 1

#define NB_INT        27

#define NB_FIQ        1
#define NB_IRQ        2

#define NB_OF_REG     NB_FIQ+NB_IRQ
#define REG_NUM(irq)  (irq<16?0:1)
#define REAL_NUM(irq) (irq<16?irq:irq-16) //get the irq line number in the reg

/********************** GPIO     ****************************************/
#define GPIO_START   TI_REG_START+0x600
#define GPIO_END     TI_REG_START+0x620

#define NB_GPIO_REG                        2

#define GPIO_DIRECTION0                    (GPIO_START+0x00)  // GIO 0-15
#define GPIO_DIRECTION1                    (GPIO_START+0x02)  // GIO 16-31

#define GPIO_INVERT0                       (GPIO_START+0x04)  // GIO 0-15
#define GPIO_INVERT1                       (GPIO_START+0x06)  // GIO 16-31

#define GPIO_BITSET0                       (GPIO_START+0x08)  // GIO 0-15
#define GPIO_BITSET1                       (GPIO_START+0x0a)  // GIO 16-31

#define GPIO_BITCLEAR0                     (GPIO_START+0x0c)  // GIO 0-15
#define GPIO_BITCLEAR1                     (GPIO_START+0x0e)  // GIO 16-31

#define GPIO_ENABLE_IRQ                    (GPIO_START+0x10)  // GIO 0-7
#define GPIO_FSEL                          (GPIO_START+0x12)
#define GPIO_BITRATE                       (GPIO_START+0x14)

#define GPIO_STR  \
{ "UKN" , "UKN", "UKN", "UKN", "UKN", "UKN", "UKN", "UKN", \
  "UKN", "UKN", "UKN", "UKN", "UKN", "UKN", "UKN", "UKN", \
                    \
  "UKN", "UKN", "UKN", "UKN", "UKN", "UKN", "UKN", "UKN", \
  "BTN-OFF", "BTN-ON", "HD_PW", "SCL", "SDA", "UKN", "UKN", "UKN"  \
}

#define GPIO_ON_NUM    0x19
#define GPIO_OFF_NUM   0x18

#define GPIO_I2C_SCL 0x1B
#define GPIO_I2C_SDA  0x1C

#define GPIO_MAS_EOD             0x04
#define GPIO_MAS_Di              0x08
#define GPIO_MAS_PW              0x10
#define GPIO_MAS_PR              0x1F

#define GPIO_LCD                 0x17

#define GPIO_HD_PW               0x1A

#define GPIO_BAT_LVL             0x1D

/********************** I2C     ****************************************/

/********************** OSD     ****************************************/
#define OSD_START   TI_REG_START+0x680
#define OSD_END     TI_REG_START+0x700

/********************** CCD     ****************************************/
//#define CCD_START   TI_REG_START+0x800
//#define CCD_END     TI_REG_START+0x820

/********************** PREVIEW ****************************************/
//#define PREVIEW_START   TI_REG_START+0x780
//#define PREVIEW_END     TI_REG_START+0x7EC

/********************** PAL NTSC Encoder *******************************/
//#define PAL_NTSC_ENC_START   TI_REG_START+0x820
//#define PAL_NTSC_ENC_END     TI_REG_START+0x83A

/********************** CLOCK    ****************************************/
#define CLOCK_START   TI_REG_START+0xA80
#define CLOCK_END     TI_REG_START+0xAF0

/********************** ECR      ****************************************/
#define ECR_START   TI_REG_START+0x900
#define ECR_END     TI_REG_START+0x906

/********************** TI VERSION **************************************/
#define TI_VER_START   TI_REG_START+0x906
#define TI_VER_END     TI_REG_START+0x908

#define TI_VER 0x32

/********************** HW_30A24 ****************************************/

/* this HW is used on the av for the usb/DC/HD status */
//#define HAS_HW_30A24

/********************** DMA      ****************************************/
#define DMA_START   TI_REG_START+0xb80
#define DMA_END     TI_REG_START+0xb90

#define DMA_SDRAM_TO_ATA  0x1
#define DMA_ATA_TO_SDRAM  0x0

#define DMA_SRC_HI  DMA_START+0x8
#define DMA_SRC_LO  DMA_START+0x6
#define DMA_DST_HI  DMA_START+0xC
#define DMA_DST_LO  DMA_START+0xA
#define DMA_SIZE    DMA_START+0x4
#define DMA_SEL     DMA_START+0x2
#define DMA_CTL     DMA_START+0xE
#define DMA_GO      DMA_START+0x0

#define INT_DMA     15

/********************** DSP      ****************************************/

#define DSP_START   TI_REG_START+0x700
#define DSP_END     TI_REG_START+0x710

/********************** IDE ****************************************/

#define IDE_BASE    0x04000000
#define IDE_END     0x04000FFF

#define IDE_DATA                          (IDE_BASE+0x800)
#define IDE_ERROR                         (IDE_BASE+0x900)
#define IDE_NSECTOR                       (IDE_BASE+0xa00)
#define IDE_SECTOR                        (IDE_BASE+0xb00)
#define IDE_LCYL                          (IDE_BASE+0xc00)
#define IDE_HCYL                          (IDE_BASE+0xd00)
#define IDE_SELECT                        (IDE_BASE+0xe00)
#define IDE_CONTROL                       (IDE_BASE+0xe80)
#define IDE_STATUS                        (IDE_BASE+0xf00)
#define IDE_COMMAND                       (IDE_BASE+0xf00)

/********************** CPLD     ****************************************/
#define CPLD_START       0x02000000
#define CPLD_END         0x03000000
#define CPLD_PORT_OFFSET 0x00400000

/********************** LCD      ****************************************/
#define SCREEN_WIDTH  336
#define SCREEN_HEIGHT 234
