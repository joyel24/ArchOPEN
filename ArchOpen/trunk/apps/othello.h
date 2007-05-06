/*
*   otello.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __APPS_OTHELLO
#define __APPS_OTHELLO


#define PIECE_SIZE 12
#define NB_CELL    8

#define BG_COLOR     COLOR_BLACK
#define LINE_COLOR   COLOR_WHITE
#define SEL_COLOR    COLOR_RED
#define TXT_COLOR    COLOR_WHITE

#define EMPTY      -1
#define HUMAN      0
#define ENEMY      1

#define JUST_TEST   0
#define NO_MOVE     0
#define CHK_ALL     1
#define DO_MOVE     1

#define NXT_POS     0
#define PREV_POS    1

#define SEARCH_DEPTH 4

struct othello_s {
/* param */
    int cellSize;
    int center_X;
    int center_Y;
    int x_ini;
    int y_ini;
/* score position */
    int nbPieceU_X;
    int nbPieceU_Y;
    int nbPieceO_X;
    int nbPieceO_Y;
/* Mode help position */
    int modeTxt_X;
    int modeTxt_Y;    
/* action definition*/
    int action_exit;
    int action_new_game;
    int action_left;
    int action_right;
    int action_up;
    int action_down;
    int action_toggle_mode;
    int action_play;
};

/* conf instance */
struct othello_s default_conf = {
    20,
    0,1,
    10,0,
    4,220,100,220,
    10,2,
    BTN_OFF,BTN_ON,
    BTN_LEFT,BTN_RIGHT,BTN_UP,BTN_DOWN,
    BTN_F3,BTN_F1
};

/* small screen instance */
struct othello_s small_conf = {
    13,
    0,1,
    10,0,
    4,120,100,120,
    10,2,
    BTN_OFF,BTN_ON,
    BTN_LEFT,BTN_RIGHT,BTN_UP,BTN_DOWN,
    BTN_F3,BTN_F1
};

/* av4 instance */
struct othello_s av4_conf = {
    20,
    1,1,
    0,0,
    4,220,100,220,
    10,2,
    BTN_OFF,BTN_F1,
    BTN_LEFT,BTN_RIGHT,BTN_UP,BTN_DOWN,
    BTN_F3,BTN_ON
};

/* gmini instance */
struct othello_s gmini_conf = {
    16,
    0,1,
    10,0,
    4,160,100,160,
    10,2,
    BTN_OFF,BTN_ON,
    BTN_LEFT,BTN_RIGHT,BTN_UP,BTN_DOWN,
    BTN_1,BTN_3
};

struct othello_entry {
    int arch;
    struct othello_s * conf;
};

struct othello_entry conf_tab[] = {
    {AV3XX_ARCH,    &default_conf},
    {GMINI4XX_ARCH, &gmini_conf},
    {AV1XX_ARCH,    &small_conf},
    {JBMM_ARCH,     &small_conf},
    {GMINI402_ARCH, &gmini_conf},
    {AV4XX_ARCH,    &av4_conf},
    {PMA_ARCH,      &av4_conf},
    {AV5XX_ARCH,    &av4_conf},
    {-1,            NULL}
};

void redraw(void);

/* img data + struct*/
 unsigned char human_bmap[12][12]= {
     {00,00,00,00,24,24,24,24,00,00,00,00},
     {00,00,24,24,24,24,24,24,24,24,00,00},
     {00,24,24,24,24,24,24,24,24,24,24,00},
     {00,24,24,24,00,00,00,00,24,24,24,00},
     {24,24,24,00,00,00,00,00,00,24,24,24},
     {24,24,24,00,00,00,00,00,00,24,24,24},
     {24,24,24,00,00,00,00,00,00,24,24,24},
     {24,24,24,00,00,00,00,00,00,24,24,24},
     {00,24,24,24,00,00,00,00,24,24,24,00},
     {00,24,24,24,24,24,24,24,24,24,24,00},
     {00,00,24,24,24,24,24,24,24,24,00,00},
     {00,00,00,00,24,24,24,24,00,00,00,00},
 };

 unsigned char av3xx_bmap[12][12]= {
     {00,00,00,00,25,25,25,25,00,00,00,00},
     {00,00,25,25,25,25,25,25,25,25,00,00},
     {00,25,25,25,25,25,25,25,25,25,25,00},
     {00,25,25,25,25,25,25,25,25,25,25,00},
     {25,25,25,25,25,25,25,25,25,25,25,25},
     {25,25,25,25,25,25,25,25,25,25,25,25},
     {25,25,25,25,25,25,25,25,25,25,25,25},
     {25,25,25,25,25,25,25,25,25,25,25,25},
     {00,25,25,25,25,25,25,25,25,25,25,00},
     {00,25,25,25,25,25,25,25,25,25,25,00},
     {00,00,25,25,25,25,25,25,25,25,00,00},
     {00,00,00,00,25,25,25,25,00,00,00,00},
 };

 BITMAP hBitmap = {(unsigned int) human_bmap, 12, 12, 0, 0};
 BITMAP aBitmap = {(unsigned int) av3xx_bmap, 12, 12, 0, 0};

#endif
