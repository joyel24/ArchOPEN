/*
*   apps/avboy/menu.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
* Gameboy / Color Gameboy emulator (port of gnuboy)
*/

/*********************************************************************/
/* menu.c - user menu for rockboy                                    */
/*                                                                   */
/* Modified by CjNr11 08/12/2005                                     */
/* Mod by Oxygen 11/2007 : using evt and palette                     */
/*********************************************************************/

#include <sys_def/ctype.h>
#include <sys_def/buttons.h>
#include <driver/videnc.h>
#include <driver/clkc.h>

#include "defs.h"
#include "avboy.h"
#include "mem.h"
#include "fb.h"

#define getstringsize(a,b,c) gfx_getStringSize(a,b,c)
#define drawline(a,b,c,d,e) gfx_drawLine(e,a,b,c,d)
#define drawrect(a,b,c,d,e) gfx_drawRect(e,a,b,c,d)
#define fillrect(a,b,c,d,e) gfx_fillRect(e,a,b,c,d)
#define putsxy(a,b,c) gfx_putS(COLOR_BLACK,COLOR_WHITE,a,b,c)

 
#define OSD_BITMAP1_WIDTH 160
#define OSD_BITMAP1_HEIGHT 144

#define USER_MENU_QUIT -2

#if defined(GMINI4XX) || defined(GMINI402)
#define LCD_WIDTH 220
#define LCD_HEIGHT 176
#define X_OFFSET 0
#define Y_OFFSET 0
#endif

#if defined(AV3XX) || defined(AV4XX)
#define LCD_WIDTH 320
#define LCD_HEIGHT 240
#define X_OFFSET 0x14
#define Y_OFFSET 0x12
#endif

#if defined(PMA)
#define LCD_WIDTH 320
#define LCD_HEIGHT 240
#define X_OFFSET 0x0
#define Y_OFFSET 0x0
#endif

#if defined(AV5XX)
#define LCD_WIDTH 480
#define LCD_HEIGHT 272
#define X_OFFSET 0
#define Y_OFFSET 0x0
#endif

extern int frameskip;
int TVState = 0;
int TVStd = 0;
int TVInt = 0;
int ARMFreq = 0;
int RotScreen = 0;
int ZoomX=0;
int TVmode=VIDENC_MODE_NTSC;

int MENU_X=10;

//int SDRFreq = 0;

#if defined(AV3XX) || defined(AV4XX) || defined(PMA) || defined(AV5XX)
extern int bt_UP;
extern int bt_DOWN;
extern int bt_LEFT;
extern int bt_RIGHT;
extern int bt_A;
extern int bt_B;
extern int bt_START;
extern int bt_SELECT;
extern int bt_MENU;
#endif

/* load/save state function declarations */
static void do_slot_menu(bool is_load);
static void do_opt_menu(void);
static void do_opt2_menu(void);
static void munge_name(char *buf, size_t bufsiz);

#define MENU_CANCEL (-1)
static int do_menu(char *title, char **items, size_t num_items, int sel_item);

/* main menu items */
#define MAIN_MENU_TITLE "AOBoy"
typedef enum {
  MM_ITEM_BACK,
  MM_ITEM_LOAD,
  MM_ITEM_SAVE,
  MM_ITEM_OPT,
  MM_ITEM_QUIT,
  MM_ITEM_LAST
} MainMenuItem;

/* strings for the main menu */
static const char *main_menu[] = {
  "Back to Game",
  "Load State...",
  "Save State...",
  "Options...",
  "Quit AOBoy"
};

typedef enum {
  SM_ITEM_SLOT1,
  SM_ITEM_SLOT2,
  SM_ITEM_SLOT3,
  SM_ITEM_SLOT4,
  SM_ITEM_SLOT5,
//  SM_ITEM_FILE,
  SM_ITEM_BACK,
  SM_ITEM_LAST
} SlotMenuItem;


/* this semi-evil, but we snprintf() into these strings later
 * Note: if you want more save slots, just add more lines 
 * to this array */
static const char *slot_menu[] = {
  "1.              ",
  "2.              ",
  "3.              ",
  "4.              ",
  "5.              ",
//  "Save to File...    ",
  "Previous Menu..."
};

#define OPT_MENU_TITLE "Options"
typedef enum {
  OM_ITEM_FRAME,
  OM_ITEM_TV,
  OM_ITEM_TVS,
  OM_ITEM_TVI,
  OM_ITEM_MO,
  OM_ITEM_BACK,
  OM_MENU_LAST
} OptMenuItem;

static const char *opt_menu[] = {
  "Frameskip       ",
  "TV Out          ",
  "TV Standard     ",
  "TV Interlaced   ",
  "More Options    ",
  "Previous Menu..."
};

#define OPT2_MENU_TITLE "More Options"
typedef enum {
#if defined(AV3XX) || defined(AV4XX) || defined(PMA) || defined(AV5XX)
  OM2_ITEM_ZMX,
  OM2_ITEM_ROT,
#endif
  OM2_ITEM_OCA,
  OM2_ITEM_CAN,
  OM2_ITEM_PAL,
  OM2_ITEM_BACK,
  OM2_MENU_LAST
} Opt2MenuItem;

static const char *opt2_menu[] = {
 // "Zoom X+1        ",
 // "Zoom Normal     ",
#if defined (AV3XX) || defined(AV4XX) || defined(PMA) || defined(AV5XX)
  "Zoom X          ",
  "Rotate Scr.     ",
#endif
  "Overclock ARM   ",
  "Clock ARM Normal",
  "Palette         ",
  "Previous Menu..."
};

void clk_overclock(bool en){
#if defined(GMINI402) || defined(GMINI4XX) || defined(AV3XX) || defined(PMA)
    if(en && ARMFreq){
#if defined(AV3XX)
        (*(volatile unsigned short *)(0x30880))=0x8080 + (ARMFreq<<4);
#else
        clkc_setClockFrequency(CLK_ARM,ARMFreq*10000000+100000000);
#endif
        // wait a little for PLL to be in a sane state
        mdelay(100);
    }else{
        // default params
#if defined(AV3XX)
        (*(volatile unsigned short *)(0x30880))=0x8021;
#else
        clkc_setClockParameters(CLK_ARM,15,2,2);
        // wait a little for PLL to be in a sane state
        mdelay(100);
        clkc_setClockParameters(CLK_ACCEL,15,2,1);
#endif
        // wait a little for PLL to be in a sane state
        mdelay(100);
    }
#endif
};

/*
 * do_user_menu - create the user menu on the screen.
 *
 * Returns USER_MENU_QUIT if the user selected "quit", otherwise
 * returns zero.
 *
 * Note: this is the only non-static function in this file at the
 * moment.  In the future I may turn do_menu/etc into a proper API, in
 * which case they'll be exposed as well.
 *
 */
int do_user_menu(void) {
  int mi, ret, num_items;
  bool done = false;

  /* set defaults */
  ret = 0; /* return value */
  mi = 0; /* initial menu selection */
  num_items = sizeof(main_menu) / sizeof(char*);

  /* loop until we should exit menu */
  while (!done) {
    /* get item selection */
    mi = do_menu(MAIN_MENU_TITLE, (char**) main_menu, num_items, mi);
    
    /* handle selected menu item */
    switch (mi) {
      case MM_ITEM_QUIT:
        // don't overclock during hdd access
        clk_overclock(false);
        ret = USER_MENU_QUIT;
      case MENU_CANCEL:
      case MM_ITEM_BACK:
        done = true;
        break;
      case MM_ITEM_LOAD:
        do_slot_menu(true);
        break;
      case MM_ITEM_SAVE:
        do_slot_menu(false);
        break;
      case MM_ITEM_OPT:
        do_opt_menu();
        break;
    }
  }

  /* return somethin' */
  return ret;
}

/*
 * munge_name - munge a string into a filesystem-safe name
 */
static void munge_name(char *buf, const size_t bufsiz) {
  unsigned int i, max;

  /* check strlen */
  max = strlen(buf);
  if(max > bufsiz) max =  bufsiz;
  
  /* iterate over characters and munge them (if necessary) */
  for (i = 0; i < max; i++)
    if (!isalnum(buf[i]))
      buf[i] = '_';
}

/*
 * build_slot_path - build a path to an slot state file for this rom
 *
 * Note: uses rom.name.  Is there a safer way of doing this?  Like a ROM
 * checksum or something like that?
 */
static void build_slot_path(char *buf, size_t bufsiz, size_t slot_id) {
 // char name_buf[40];
  char *name_buf;
  name_buf=(char *)malloc(40);
  /* munge state file name */
  strncpy(name_buf, rom.name,256); // sizeof(name_buf));
  name_buf[16] = '\0';
  munge_name(name_buf, strlen(name_buf));

  /* glom the whole mess together */
  snprintf(buf, bufsiz, "%s/%s-%d.avb", STATE_DIR, name_buf, slot_id + 1);
  free(name_buf);
}

/*
 * do_file - load or save game data in the given file
 *
 * Returns true on success and false on failure.
 *
 * @desc is a brief user-provided description (<20 bytes) of the state.
 * If no description is provided, set @desc to NULL.
 *
 */
static bool do_file(char *path/*, char *desc*/, bool is_load) {
  int fd; //, file_mode;

  /* load/save state */
  if (is_load) {
    fd = open(path, O_RDONLY );
    if(!fd) {
      printf("Retry...\n"); 
      fd = open(path, O_RDONLY );
      if(!fd) return false;
      else printf("File opened!\n");
    }
    else printf("File opened!\n");

    /* load state */
    loadstate(fd);
    printf("Loaded state from \"%s\"\n", path);

  } else {
    fd = open(path, O_WRONLY | O_CREAT );
    if(!fd) {
      printf("Retry...\n"); 
      fd = open(path, O_WRONLY | O_CREAT ); 
      if(!fd) return false;
      else printf("File opened!\n");
    }
    else printf("File opened!\n");

    /* save state */
    savestate(fd);
  }

  /* close file descriptor */
  close(fd);

  /* return true (for success) */
  return true;
}

/*
 * do_slot - load or save game data in the given slot
 *
 * Returns true on success and false on failure.
 */
static bool do_slot(size_t slot_id, bool is_load) {
  char *path_buf;
  bool res;
  path_buf=(char *)malloc(256);

  /* build slot filename*/
  build_slot_path(path_buf, 256, slot_id);
  printf("Path : %s\n",path_buf);

  /* load/save file */
  res = do_file(path_buf/*, desc_buf*/, is_load);
  free(path_buf);
  return  res;
}

/*
 * get information on the given slot
 */
static void slot_info(char *info_buf, size_t info_bufsiz, size_t slot_id) {
  char * buf;
  int fd;
  buf=(char *)malloc(256);

  /* get slot file path */
  build_slot_path(buf, 256, slot_id);

  /* attempt to open slot */
  if ((fd = open(buf, O_RDONLY)) >= 0) {
    snprintf(info_buf, info_bufsiz, "%2d. State Saved", slot_id + 1);
    close(fd);
  } else {
    /* if we couldn't open the file, then the slot is empty */
    snprintf(info_buf, info_bufsiz, "%2d. Empty", slot_id + 1);
    free(buf);
  }
}

/*
 * do_slot_menu - prompt the user for a load/save memory slot
 */
static void do_slot_menu(bool is_load) {
  int i, mi, ret, num_items;
  bool done = false;
  char *title;

  // don't overclock during hdd access
  clk_overclock(false);

  /* set defaults */
  ret = 0; /* return value */
  mi = 0; /* initial menu selection */
  num_items = sizeof(slot_menu) / sizeof(char*);
  
  /* create menu items (the last two are file and previous menu,
   * so don't populate those) */
  for (i = 0; i < num_items - 1; i++)
    slot_info((char*) slot_menu[i], 17, i);

  /* set menu title */
  if(is_load) title = "Load State";
  else title = "Save State";

  /* loop until we should exit menu */
  while (!done) {
    /* get item selection */
    mi = do_menu(title, (char**) slot_menu, num_items, mi);

    /* handle selected menu item */
    done = true;
    if (mi != MENU_CANCEL && mi != SM_ITEM_BACK) {
      done = do_slot(mi, is_load);

      /* if we couldn't save the state file, then print out an
       * error message */
      if (!is_load && !done)
      printf("Couldn't save state file.");
    }
  }
  clk_overclock(true);
}

static void do_opt_menu(void) {
  int mi, num_items;
  bool done = false;

  /* set a couple of defaults */
  num_items = sizeof(opt_menu) / sizeof(char*);
  mi = 0;
  snprintf((char *)opt_menu[0], 17, "Frameskip      %1d", frameskip);
  snprintf((char *)opt_menu[1], 17, "TV Out       %s", (TVState ? " ON" : "OFF"));
  snprintf((char *)opt_menu[2], 17, "TV Standard %s", (TVStd ? " PAL" : "NTSC"));
  snprintf((char *)opt_menu[3], 17, "TV Interlaced  %s", (TVInt ? "Y" : "N"));
  while (!done) {
    mi = do_menu(OPT_MENU_TITLE, (char**) opt_menu, num_items, mi);
    switch(mi) {
      case OM_ITEM_FRAME:
        frameskip=(frameskip+1)%10;
        snprintf((char *)opt_menu[0], 17, "Frameskip      %1d", frameskip);
        break;
      case OM_ITEM_TV:
        //(*(volatile unsigned short *)(0x30800))^=0x4;
        TVState = !TVState;
       // if(TVState)(*(volatile unsigned short *)(0x3058E))=0x2000;
       // else (*(volatile unsigned short *)(0x3058A))=0x2000;
        if(TVState) {
           gfx_planeSetPos(BMAP1,(LCD_WIDTH-OSD_BITMAP1_WIDTH) + X_OFFSET + 40,(LCD_HEIGHT-OSD_BITMAP1_HEIGHT)/2 + Y_OFFSET + (TVStd ? 42 : 20));
           videnc_setup(TVmode,TVInt);
        }
        else {
           gfx_planeSetPos(BMAP1,(LCD_WIDTH-OSD_BITMAP1_WIDTH) + X_OFFSET,(LCD_HEIGHT-OSD_BITMAP1_HEIGHT)/2 + Y_OFFSET);
           videnc_setup(VIDENC_MODE_LCD,false);
        }
        snprintf((char *)opt_menu[1], 17, "TV Out       %s", (TVState ? " ON" : "OFF"));
        break;
      case OM_ITEM_TVS:
      //  (*(volatile unsigned short *)(0x30800))^=0x8000;
        TVStd = !TVStd;
        if(TVStd) {
           TVmode=VIDENC_MODE_PAL;
        }
        else {
           TVmode=VIDENC_MODE_NTSC;
        }
        if(TVState) {
           gfx_planeSetPos(BMAP1,(LCD_WIDTH-OSD_BITMAP1_WIDTH) + X_OFFSET + 40,(LCD_HEIGHT-OSD_BITMAP1_HEIGHT)/2 + Y_OFFSET + (TVStd ? 42 : 20));
           videnc_setup(TVmode,TVInt);
        }
        snprintf((char *)opt_menu[2], 17, "TV Standard %s", (TVStd ? " PAL" : "NTSC"));
        break;
      case OM_ITEM_TVI:
        //(*(volatile unsigned short *)(0x30800))^=0x4000;
        TVInt = !TVInt;
        if(TVState) {
           gfx_planeSetPos(BMAP1,(LCD_WIDTH-OSD_BITMAP1_WIDTH) + X_OFFSET + 40,(LCD_HEIGHT-OSD_BITMAP1_HEIGHT)/2 + Y_OFFSET + (TVStd ? 42 : 20));
           videnc_setup(TVmode,TVInt);
        }
        snprintf((char *)opt_menu[3], 17, "TV Interlaced  %s", (TVInt ? "Y" : "N"));
        break;
      case OM_ITEM_MO:
        do_opt2_menu();
        break;
      case MENU_CANCEL:
        done = true;
        break;
      case OM_ITEM_BACK:
        done = true;
        break;
    }
  }
}

static void do_opt2_menu(void) {
  int mi, num_items,x,y,c=0;
  bool done = false;

  /* set a couple of defaults */
  num_items = sizeof(opt2_menu) / sizeof(char*);
  mi = 0;
//  snprintf((char *)opt2_menu[0], 17, "Overclock SDR  %1d", SDRFreq);
#if defined(AV3XX) || defined(AV4XX) || defined(PMA) || defined(AV5XX) 
  snprintf((char *)opt2_menu[1], 17, "Rotate Scr.    %1d", RotScreen);
  snprintf((char *)opt2_menu[0], 17, "Zoom X       %s", (ZoomX ? " ON" : "OFF"));
#endif
  snprintf((char *)opt2_menu[OM2_ITEM_OCA], 17, "Overclock ARM  %1d", ARMFreq);
  while (!done) {
    mi = do_menu(OPT2_MENU_TITLE, (char**) opt2_menu, num_items, mi);
    switch(mi) {
  /*    case OM2_ITEM_ZX:
        gfx_planeSetSize(BMAP1,320,288,8,GFX_SMODE_STD);
        gfx_planeSetPos(BMAP1,0x14,0x12);
        (*(volatile unsigned short *)(0x30684))+=0x100;
        break;
      case OM2_ITEM_ZN:
        gfx_planeSetSize(BMAP1,160,144,8,GFX_SMODE_STD);
        gfx_planeSetPos(BMAP1,168,68);
        break;                  
      case OM2_ITEM_OCS:
        SDRFreq++;
        snprintf((char *)opt2_menu[0], 17, "Overclock SDR  %1d", SDRFreq);
        (*(volatile unsigned short *)(0x30884))=0x80C0 + (SDRFreq<<4);
       break;
      case OM2_ITEM_CSN:
        SDRFreq=0;
        snprintf((char *)opt2_menu[0], 17, "Overclock SDR  %1d", SDRFreq);
        (*(volatile unsigned short *)(0x30884))=0x8031;
        break;*/
#if defined(AV3XX) || defined(AV4XX) || defined(PMA) || defined(AV5XX)
      case OM2_ITEM_ZMX:
        if(!RotScreen) {
           ZoomX=!ZoomX;
           if(ZoomX) {
              gfx_planeSetSize(BMAP1,320,144,8,GFX_SMODE_STD);
              gfx_planeSetPos(BMAP1, X_OFFSET,(LCD_HEIGHT-OSD_BITMAP1_HEIGHT)/2 + Y_OFFSET);
              fb.pitch=320;
           }
           else {
              gfx_planeSetSize(BMAP1,160,144,8,GFX_SMODE_STD);
              gfx_planeSetPos(BMAP1,(LCD_WIDTH-OSD_BITMAP1_WIDTH) + X_OFFSET,(LCD_HEIGHT-OSD_BITMAP1_HEIGHT)/2 + Y_OFFSET);
              fb.pitch=OSD_BITMAP1_WIDTH;
           }
           snprintf((char *)opt2_menu[0], 17, "Zoom X       %s", (ZoomX ? " ON" : "OFF"));
        }
        break;
      case OM2_ITEM_ROT:
        if(!ZoomX) {
           RotScreen = (RotScreen+1)%3;
           snprintf((char *)opt2_menu[1], 17, "Rotate Scr.    %1d", RotScreen);
           if(RotScreen==2) {
              fb.pitch=-1;
              gfx_planeSetSize(BMAP1,144,160,8,GFX_SMODE_STD);
              
              #if defined(AV3XX)
              bt_UP = BTMASK_RIGHT;
              bt_DOWN = BTMASK_LEFT;
              bt_LEFT = BTMASK_UP;
              bt_RIGHT = BTMASK_DOWN;
              bt_A = BTMASK_F2;
              bt_B = BTMASK_F3;
              bt_SELECT = BTMASK_F1;
              bt_START = BTMASK_ON;
              bt_MENU = BTMASK_OFF;
              #endif

              #if defined(AV4XX)
              bt_UP = BTMASK_RIGHT;
              bt_DOWN = BTMASK_LEFT;
              bt_LEFT = BTMASK_UP;
              bt_RIGHT = BTMASK_DOWN;
              bt_A = BTMASK_F2;
              bt_B = BTMASK_F3;
              bt_SELECT = BTMASK_F1;
              bt_START = BTMASK_OFF;
              bt_MENU = BTMASK_BTN1;
              #endif
              
              #if defined(PMA)
              bt_UP = BTMASK_RIGHT;
              bt_DOWN = BTMASK_LEFT;
              bt_LEFT = BTMASK_UP;
              bt_RIGHT = BTMASK_DOWN;
              bt_A = BTMASK_F2;
              bt_B = BTMASK_F3;
              bt_SELECT = BTMASK_F1;
              bt_START = BTMASK_OFF;
              bt_MENU = BTMASK_BTN1;
              #endif
              
               #if defined(AV5XX)
              bt_UP = BTMASK_RIGHT;
              bt_DOWN = BTMASK_LEFT;
              bt_LEFT = BTMASK_UP;
              bt_RIGHT = BTMASK_DOWN;
              bt_A = BTMASK_F2;
              bt_B = BTMASK_F3;
              bt_SELECT = BTMASK_F1;
              bt_START = BTMASK_OFF;
              bt_MENU = BTMASK_BTN1;
              #endif

              MENU_X=2;
           }
           else if(RotScreen==1) {
              fb.pitch=1;
              gfx_planeSetSize(BMAP1,144,160,8,GFX_SMODE_STD);
              
              #if defined(AV3XX)
              bt_UP = BTMASK_LEFT;
              bt_DOWN = BTMASK_RIGHT;
              bt_LEFT = BTMASK_DOWN;
              bt_RIGHT = BTMASK_UP;
              bt_A = BTMASK_F3;
              bt_B = BTMASK_F2;
              bt_SELECT = BTMASK_F1;
              bt_START = BTMASK_ON;
              bt_MENU = BTMASK_OFF;
              #endif

              #if defined(AV4XX)
              bt_UP = BTMASK_LEFT;
              bt_DOWN = BTMASK_RIGHT;
              bt_LEFT = BTMASK_DOWN;
              bt_RIGHT = BTMASK_UP;
              bt_A = BTMASK_F2;
              bt_B = BTMASK_F3;
              bt_SELECT = BTMASK_F1;
              bt_START = BTMASK_OFF;
              bt_MENU = BTMASK_BTN1;
              #endif
                           
              #if defined(AV5XX)
              bt_UP = BTMASK_LEFT;
              bt_DOWN = BTMASK_RIGHT;
              bt_LEFT = BTMASK_DOWN;
              bt_RIGHT = BTMASK_UP;
              bt_A = BTMASK_F2;
              bt_B = BTMASK_F3;
              bt_SELECT = BTMASK_F1;
              bt_START = BTMASK_OFF;
              bt_MENU = BTMASK_BTN1;
              #endif
              
              #if defined(PMA)
              bt_UP = BTMASK_LEFT;
              bt_DOWN = BTMASK_RIGHT;
              bt_LEFT = BTMASK_DOWN;
              bt_RIGHT = BTMASK_UP;
              bt_A = BTMASK_F2;
              bt_B = BTMASK_F3;
              bt_SELECT = BTMASK_F1;
              bt_START = BTMASK_OFF;
              bt_MENU = BTMASK_BTN1;
              #endif

              MENU_X=2;
           }
           else {
              fb.pitch=OSD_BITMAP1_WIDTH;
              gfx_planeSetSize(BMAP1,160,144,8,GFX_SMODE_STD);

              #if defined(AV3XX)
              bt_UP = BTMASK_UP;
              bt_DOWN = BTMASK_DOWN;
              bt_LEFT = BTMASK_LEFT;
              bt_RIGHT = BTMASK_RIGHT;
              bt_A = BTMASK_F2;
              bt_B = BTMASK_F1;
              bt_START = BTMASK_ON;
              bt_SELECT = BTMASK_F3;
              bt_MENU = BTMASK_OFF;
              #endif

              #if defined(AV4XX)
              bt_UP = BTMASK_UP;
              bt_DOWN = BTMASK_DOWN;
              bt_LEFT = BTMASK_LEFT;
              bt_RIGHT = BTMASK_RIGHT;
              bt_A = BTMASK_BTN1;
              bt_B = BTMASK_ON;
              bt_START = BTMASK_F1;
              bt_SELECT = BTMASK_OFF;
              bt_MENU = BTMASK_F2;
              #endif
              
              #if defined(AV5XX)
              bt_UP = BTMASK_UP;
              bt_DOWN = BTMASK_DOWN;
              bt_LEFT = BTMASK_LEFT;
              bt_RIGHT = BTMASK_RIGHT;
              bt_A = BTMASK_BTN1;
              bt_B = BTMASK_ON;
              bt_START = BTMASK_F1;
              bt_SELECT = BTMASK_OFF;
              bt_MENU = BTMASK_F2;
              #endif
              
              #if defined(PMA)
              bt_UP = BTMASK_UP;
              bt_DOWN = BTMASK_DOWN;
              bt_LEFT = BTMASK_LEFT;
              bt_RIGHT = BTMASK_RIGHT;
              bt_A = BTMASK_BTN1;
              bt_B = BTMASK_ON;
              bt_START = BTMASK_F2;
              bt_SELECT = BTMASK_F3;
              bt_MENU = BTMASK_F1;
              #endif

              MENU_X=10;
           }
        }
        break;
#endif
      case OM2_ITEM_OCA:
#if defined(PMA)
        if(ARMFreq<3)          // I don't want to destroy my PMA since there's no WTD!
#endif
           ARMFreq++;
        snprintf((char *)opt2_menu[OM2_ITEM_OCA], 17, "Overclock ARM  %1d", ARMFreq);
        //(*(volatile unsigned short *)(0x30880))=0x8080 + (ARMFreq<<4);
        clk_overclock(true);
        break;
      case OM2_ITEM_CAN:
        ARMFreq=0;
        snprintf((char *)opt2_menu[OM2_ITEM_OCA], 17, "Overclock ARM  %1d", ARMFreq);
       // (*(volatile unsigned short *)(0x30880))=0x8021;
        clk_overclock(false);
        break;
      case OM2_ITEM_PAL:
        while (btn_readState());
        for (y=0;y<144;y+=9) {
          for (x=0;x<160;x+=10) {
            gfx_fillRect(c, x, y, 10, 9);
            c++;
          }
        }
        while (!btn_readState());
        break;
      case MENU_CANCEL:
        done = true;
        break;
      case OM2_ITEM_BACK:
        done = true;
        break;
    }
  }
}

/*********************************************************************/
/*  MENU FUNCTIONS                                                   */
/*********************************************************************/
/* at some point i'll make this a generic menu interface, but for now,
 * these defines will suffice */
#define MENU2_X 10                      // MENU_X defined at the top
#define MENU_Y 8
#define MENU_WIDTH (OSD_BITMAP1_WIDTH - 2 * MENU2_X)
#define MENU_HEIGHT (OSD_BITMAP1_HEIGHT - 2 * MENU_Y)
//#define MENU_RECT MENU_X, MENU_Y, MENU_WIDTH, MENU_HEIGHT
//#define SHADOW_RECT MENU_X + 1, MENU_Y + 1, MENU_WIDTH, MENU_HEIGHT
#define MENU_ITEM_PAD 2

/*
 * select_item - select menu item (after deselecting current item)
 */
static void select_item(char *title, int curr_item, size_t item_i) {
  int x, y, w, h;

  /* get size of title, use that as height ofr all lines */
  getstringsize(title, &w, &h);
  h += MENU_ITEM_PAD * 2;

  /* calc x and width */
  x = MENU_X + MENU_ITEM_PAD;
  w = MENU_WIDTH - 2 * MENU_ITEM_PAD;

  /* if there is a current item, then deselect it */
  if (curr_item >= 0) {
    /* deselect old item */
    y = MENU_Y + h + MENU_ITEM_PAD * 2; /* account for title */
    y += h * curr_item;
    drawrect(x, y, w, h,COLOR_DARK_BLUE);
  }

  /* select new item */
  curr_item = item_i;

  /* select new item */
  y = MENU_Y + h + MENU_ITEM_PAD * 2; /* account for title */
  y += h * curr_item;
  drawrect(x, y, w, h,COLOR_WHITE);
}

/*
 * draw_menu - draw menu on screen
 *
 * Returns MENU_CANCEL if the user cancelled, or the item number of the
 * selected item.
 *
 */
static void draw_menu(char *title, char **items, size_t num_items)  {
  size_t i;
  int x, y, w, h, by;

  /* draw the outline */
  gfx_fillRect(COLOR_BLUE,MENU_X + 1, MENU_Y + 1, MENU_WIDTH, MENU_HEIGHT); // fillrect(0xaf,SHADOW_RECT);
  gfx_fillRect(COLOR_DARK_BLUE,MENU_X, MENU_Y, MENU_WIDTH, MENU_HEIGHT);   // fillrect(0x00,MENU_RECT);
  gfx_drawRect(COLOR_WHITE,MENU_X, MENU_Y, MENU_WIDTH, MENU_HEIGHT);   // drawrect(0xff,MENU_RECT);

  /* calculate x/y */
  x = MENU_X + MENU_ITEM_PAD;
  y = MENU_Y + MENU_ITEM_PAD * 2;
  getstringsize(title, &w, &h);
  h += MENU_ITEM_PAD * 2;

  /* draw menu stipple */
  for (i = MENU_Y; i < (size_t) y + h; i += 2)
    drawline(MENU_X, i, MENU_X + MENU_WIDTH-1, i,COLOR_BLUE);

  /* clear title rect */
  fillrect((OSD_BITMAP1_WIDTH - w) / 2 - 2, y - 2, w + 4, h,COLOR_DARK_BLUE);

  /* draw centered title on screen */
  
  gfx_putS(COLOR_WHITE,COLOR_DARK_BLUE,(OSD_BITMAP1_WIDTH - w)/2, y, title);

  /* calculate base Y for items */
  by = y + h + MENU_ITEM_PAD;
  
  /* iterate over each item and draw it on the screen */
  for (i = 0; i < num_items; i++)
    gfx_putS(COLOR_WHITE,COLOR_DARK_BLUE,x+2, by + h * i, items[i]);
}

/*
 * do_menu - draw menu on screen.
 *
 * Draw a menu titled @title on the screen, with @num_items elements
 * from @items, and select the @sel element.  If in doubt, set @sel to
 * -1 :).
 *
 */
static int do_menu(char *title, char **items, size_t num_items, int sel)
{
    int sel_item, ret, curr_item; //,y,x,c=0;
    bool done = false;
    ret = MENU_CANCEL;
    int evtHand,evt;
    
    /* draw menu on screen and select the first item */
    draw_menu(title, items, num_items);
    curr_item = -1;
    select_item(title, curr_item, sel);
    curr_item = sel;
    evtHand=evt_getHandler(BTN_CLASS);
            
    /* loop until the menu is finished */
    while (!done)
    {
        /* grab a button */
        
        evt=evt_getStatusBlocking(evtHand);
    
        switch(evt)
        {
            case BTN_DOWN: /* handle the button */
                /* select next item in list */
                sel_item = curr_item + 1;
                if (sel_item >= (int) num_items)
                sel_item = 0;
                select_item(title, curr_item, sel_item);
                curr_item = sel_item;
                break;
            case BTN_UP: /* select prev item in list */
                sel_item = curr_item - 1;
                if (sel_item < 0)
                sel_item = num_items - 1;
                select_item(title, curr_item, sel_item);
                curr_item = sel_item;
            case BTN_ON:
            case BTN_RIGHT:                
                /* select current item */
                ret = curr_item;
                done = true;
                break;
            case BTN_F2:
            case BTN_OFF:
                /* cancel out of menu */
                ret = MENU_CANCEL;
                done = true;
        }
    }
    
    /* return selected item */
    return ret;
}

int browser(char * rom)
{
    int x, y, w, h, by;
    size_t i;
    
    int sel_item=0, curr_item, num_items=6, nb=0,pos=0;
    char title[]="Start...";
    char (*items)[17];
    char (*list)[MAX_PATH];
    int done = 0,done1=0;
    struct dirent *romdir;
    DIR * romd=NULL;
    int evtHand,evt;

    items = malloc(MAX_PATH*6);
    list = malloc(MAX_PATH);

    romd=opendir(ROM_DIR);
    if(romd) printf("Dir %s opened!\n",ROM_DIR);
    else {
    mkdir(ROM_DIR,0);
    if(romd) printf("Dir %s created and opened!\n",ROM_DIR);
    else printf("Dir error!\n");
    }


  while((romdir=readdir(romd))!=NULL) {
     if(!(romdir->attribute & ATTR_DIRECTORY)) {
     list[nb][0]='\0';
     strcat(list[nb],romdir->d_name);
     nb++;
     list = realloc(list,MAX_PATH*(nb+1));
     }
  }
  
    closedir(romd);

  if(!nb) {
    *rom=0;    
    return 0;
  }

  evtHand=evt_getHandler(BTN_CLASS);
  
  gfx_fillRect(COLOR_BLUE,11,9,140,128);
  gfx_fillRect(COLOR_DARK_BLUE,10,8,140,128);
  gfx_drawRect(COLOR_WHITE,10,8,140,128);

  x = 10 + 2;
  y = 8 + 2 * 2;
  getstringsize(title, &w, &h);
  h += 2 * 2;

  for (i = 8; i < (size_t) y + h; i += 2)
    drawline(10, i, 10 + 139, i,COLOR_BLUE);

  fillrect((160 - w) / 2 - 2, y - 2, w + 4, h,COLOR_DARK_BLUE);
  gfx_putS(COLOR_WHITE,COLOR_DARK_BLUE,(160 - w)/2, y, title);

  by = y + h + 2;
  


    while(!done)
    {
        if(pos > (nb-6)) num_items=nb-pos;
        else num_items=6;
        //  for(i=0;i<num_items;i++) {items[i][0]='\0'; strcat(items[i],list[i+pos]);}
        for(i=0;i<num_items;i++) {strncpy(items[i],list[i+pos],16); items[i][16]='\0';}
        
        gfx_fillRect(COLOR_DARK_BLUE,11,29,138,106);
        for (i = 0; i < num_items; i++)
            gfx_putS(COLOR_WHITE,COLOR_DARK_BLUE,x+2, by + h * i, items[i]);
        
        curr_item = -1;
        select_item(title, curr_item, sel_item);
        curr_item = sel_item;
        done1=0;
        while (!done1)
        {            
            evt=evt_getStatusBlocking(evtHand);        
            
            printk("evt = %d\n",evt);
            
            switch(evt)
            {
                case BTN_DOWN:    
                    sel_item = curr_item + 1;
                    if (sel_item >= (int) num_items)
                    {
                        if(pos < (nb-6)) {sel_item=0;pos+=6;done1=1;}
                        else {sel_item = curr_item;}
                    }
                    else
                    {
                        select_item(title, curr_item, sel_item);
                        curr_item = sel_item;
                    }
                    break;
                case BTN_UP:            
                    sel_item = curr_item - 1;
                    if (sel_item < 0)
                    {
                        if(pos > 0) {sel_item=5;pos-=6;done1=1;}
                        else {sel_item = curr_item;}
                    }
                    else
                    {
                        select_item(title, curr_item, sel_item);
                        curr_item = sel_item;
                    }
                    break;
                case BTN_RIGHT:
                case BTN_ON:
                    done = 1;
                    done1=1;
                    break;
                case BTN_OFF:
                    *rom=0;
                    free(items);
                    free(list);
                    evt_freeHandler(evtHand);
                    return 1;
            }        
        }
    }

  rom[0]='\0';
  strcat(rom,ROM_DIR);
  strcat(rom,list[curr_item+pos]);
  strcat(rom,"\0");
  free(items);
  free(list);
  evt_freeHandler(evtHand);
  return 0;
}
