/*
*   mastermind.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
* initial code from Schoki - Linav project 2004
* 
*
*/

#include <api.h>

#include <sys_def/evt.h>
#include <sys_def/stddef.h>
#include <sys_def/colordef.h>
#include <sys_def/font.h>

#include <fs/cfg_file.h>

/* color define */
#define BG_COLOR            COLOR_WHITE
#define TXT_B_COLOR         COLOR_RED

#define TXT_ONE_COLOR		COLOR_BLUE
#define TXT_TWO_COLOR		COLOR_GREEN
#define TXT_THREE_COLOR		COLOR_RED
#define TXT_FOUR_COLOR		COLOR_DARK_BLUE
#define TXT_FIVE_COLOR		COLOR_BROWN
#define TXT_SIX_COLOR		COLOR_LIGHT_GREEN
#define TXT_SEVEN_COLOR		COLOR_ORANGE
#define TXT_EIGHT_COLOR		COLOR_BLACK

#define FIELD_BG_COLOR      COLOR_GREY
#define FIELD_FOUND_COLOR   COLOR_LIGHT_GREY
#define CROSS_COLOR         COLOR_RED
#define CURSOR_COLOR        COLOR_BLUE
#define UI_TXT_COLOR        COLOR_GREEN

#include "minesweeper.h"

/*Cfg Functions*/
void WriteCfg(void);
void ReadCfg(void);

void MinesResult(void);

/* percentage of mines on minefield used durring generation */
int p=22;

/* the cursor coordinates */
int x=0,y=0;

/* the height and width of the field (nb of cells)*/
int height,max_height,width,max_width;

int screen_height;
int screen_width;

int evt_handler;
int stop;

/* the minefield */
tile * minefield;

/* total number of mines on the game */
int mine_num = 0;
int tiles_left = 0;

int fieldX=0,fieldY=0;
int nb_left_txtY;

#define MINE_FIELD(I,J) minefield[(I)*width+(J)]
#define FIELD_X             fieldX
#define FIELD_Y             fieldY
#define NB_LEFT_TXT_Y       nb_left_txtY

struct minesweeper_s * cur_conf;

#define ACTION_EXIT         (cur_conf->action_exit)

#define ACTION_UP           (cur_conf->action_up)
#define ACTION_DOWN         (cur_conf->action_down)
#define ACTION_RIGHT        (cur_conf->action_right)
#define ACTION_LEFT         (cur_conf->action_left)

#define ACTION_DISCOVER     (cur_conf->action_discover)
#define ACTION_TOGGLE_FLAG  (cur_conf->action_toggleflag)
#define ACTION_NEW_GAME     (cur_conf->action_newgame)
#define ACTION_SETTINGS     (cur_conf->action_settings)

void computeSizePos(void)
{
    int h;
    gfx_getStringSize("M",0,&h);
    nb_left_txtY=screen_height-h-1;
    fieldX=(screen_width-width*PIECE_DIM)/2;
    fieldY=(screen_height-h-2-height*PIECE_DIM)/2;
}

/* discovers the tile when player clears one of them */
/* a chain reaction (of discovery) occurs if tile has no mines */
/* as neighbors */
void discover(int x, int y){

    if(x<0) return;
    if(y<0) return;
    if(x>width-1) return;
    if(y>height-1) return;
    if(MINE_FIELD(y,x).known) return;

    MINE_FIELD(y,x).known = 1;
    if(MINE_FIELD(y,x).neighbors == 0){
        discover(x-1,y-1);
        discover(x,y-1);
        discover(x+1,y-1);
        discover(x+1,y);
        discover(x+1,y+1);
        discover(x,y+1);
        discover(x-1,y+1);
        discover(x-1,y);
    }
    return;
}



void printNumberOfMines(void){
     char tmp[20];
     int i=0;
     int j=0;

     if(mine_num > 0)
     {
        tiles_left = 0;
        for(i=0;i<height;i++){
            for(j=0;j<width;j++){
                    if(MINE_FIELD(i,j).flag) tiles_left++;
            }
        }

        sprintf(tmp,"%d Mines ", mine_num-tiles_left);
        gfx_fillRect(BG_COLOR, 0,NB_LEFT_TXT_Y, screen_width, screen_height-NB_LEFT_TXT_Y);
        gfx_putS(UI_TXT_COLOR, BG_COLOR, 1,NB_LEFT_TXT_Y, tmp);
     }
}

/* put mines on the mine field */
/* there is p% chance that a tile is a mine */
/* if the tile has coordinates (x,y), then it can't be a mine */
void minesweeper_putmines(int p, int x, int y){
    int i,j;
    for(i=0;i<height;i++){
        for(j=0;j<width;j++){
            if(rand()%100<p && !(y==i && x==j)){
                MINE_FIELD(i,j).mine = 1;
                mine_num++;
            } else {
                MINE_FIELD(i,j).mine = 0;
            }
            MINE_FIELD(i,j).neighbors = 0;
        }
    }

    /* we need to compute the neighbor element for each tile */
    for(i=0;i<height;i++){
        for(j=0;j<width;j++){
            if(i>0){
                if(j>0)
                    MINE_FIELD(i,j).neighbors += MINE_FIELD(i-1,j-1).mine;
                MINE_FIELD(i,j).neighbors += MINE_FIELD(i-1,j).mine;
                if(j<width-1)
                    MINE_FIELD(i,j).neighbors += MINE_FIELD(i-1,j+1).mine;
            }
            if(j>0)
                MINE_FIELD(i,j).neighbors += MINE_FIELD(i,j-1).mine;
            if(j<width-1)
                MINE_FIELD(i,j).neighbors += MINE_FIELD(i,j+1).mine;
            if(i<height-1){
                if(j>0)
                    MINE_FIELD(i,j).neighbors += MINE_FIELD(i+1,j-1).mine;
                MINE_FIELD(i,j).neighbors += MINE_FIELD(i+1,j).mine;
                if(j<width-1)
                    MINE_FIELD(i,j).neighbors += MINE_FIELD(i+1,j+1).mine;
            }
        }
    }
}

void setCursor(int del)
{
   if(del)
       gfx_drawRect(BG_COLOR,
                    FIELD_X+x*PIECE_DIM, FIELD_Y+y*PIECE_DIM,
                    PIECE_DIM, PIECE_DIM);
    else
       gfx_drawRect(CURSOR_COLOR,
                    FIELD_X+x*PIECE_DIM, FIELD_Y+y*PIECE_DIM,
                    PIECE_DIM, PIECE_DIM);
}

void settings_screen()
{
    int h,w,event,y;
    char tmp[50];
    WIDGETMENU_TRACKBAR trackWidth;
    WIDGETMENU_TRACKBAR trackHeight;
    WIDGETMENU_TRACKBAR trackPercent;
    WIDGETMENU widgetMenu;
       
    gfx_clearScreen(BG_COLOR); // clear
    gfx_getStringSize("SETTINGS",&w,&h);
    gfx_putS(UI_TXT_COLOR, BG_COLOR, (screen_width-w)/2, 5,  "SETTINGS");
    gfx_drawLine(UI_TXT_COLOR,(screen_width-w)/2-1,5+h+1,(screen_width-w)/2+w+1,5+h+1);
    
    y=5+h+2;

    widgetMenu=widgetMenu_create();
    widgetMenu->setRect(widgetMenu,5,y,screen_width-5,3*(5+h+2));
    widgetMenu->ownWidgets=true; // the menu will handle items destroy
    
    trackWidth=widgetMenuTrackbar_create();
    trackWidth->caption="Width";
    widgetMenu->addItem(widgetMenu,trackWidth);
    trackWidth->trackbar->setParam(trackWidth->trackbar,5,max_width,1);
    trackWidth->trackbar->setValue(trackWidth->trackbar,width);

    trackHeight=widgetMenuTrackbar_create();
    trackHeight->caption="Height";
    widgetMenu->addItem(widgetMenu,trackHeight);
    trackHeight->trackbar->setParam(trackHeight->trackbar,5,max_height,1);
    trackHeight->trackbar->setValue(trackHeight->trackbar,height);
    
    trackPercent=widgetMenuTrackbar_create();
    trackPercent->caption="Percent";
    widgetMenu->addItem(widgetMenu,trackPercent);
    trackPercent->trackbar->setParam(trackPercent->trackbar,0,100,5);
    trackPercent->trackbar->setValue(trackPercent->trackbar,p);

    y+=3*(5+h+2)+5;
    
    gfx_getStringSize("M",&w,&h);
    gfx_putS(UI_TXT_COLOR, BG_COLOR, 10,y, "Direction: Joystick");
    y = y+h+2;
    sprintf(tmp,"Toggle: %s",getBtnName(ACTION_TOGGLE_FLAG));
    gfx_putS(UI_TXT_COLOR, BG_COLOR, 10,y, tmp);
    y = y+h+2;
    sprintf(tmp,"Discover: %s",getBtnName(ACTION_DISCOVER));
    gfx_putS(UI_TXT_COLOR, BG_COLOR, 10,y, tmp);
    y = y+h+2;
    sprintf(tmp,"New game: %s",getBtnName(ACTION_NEW_GAME));
    gfx_putS(UI_TXT_COLOR, BG_COLOR, 10,y,tmp );
    y = y+h+2;
    sprintf(tmp,"Exit: %s",getBtnName(ACTION_EXIT));
    gfx_putS(UI_TXT_COLOR, BG_COLOR, 10,y, tmp);

    widgetMenu->setFocusedWidget(widgetMenu,trackWidth);
    widgetMenu->paint(widgetMenu);
    
    do{
        event=evt_getStatusBlocking(evt_handler);
        if (!event) continue; // no new events
        if(widgetMenu->handleEvent(widgetMenu,event))
            continue;
    }while(event!=BTN_OFF && event!=BTN_ON);
    if(event==BTN_ON)
    {
        if(trackHeight->trackbar->value != width || trackHeight->trackbar->value != height)
        {
            free(minefield);
            minefield = (tile*)malloc(sizeof(tile)*height*width);
            if(!minefield)
            {
                msgBox_show("Memory error", "No more memory => exit\n", 
                            MSGBOX_TYPE_OK, MSGBOX_ICON_ERROR, evt_handler);
                stop=1;
                widgetMenu->destroy(widgetMenu);
                return;
            }
        }
        
        width=trackHeight->trackbar->value;
        height=trackHeight->trackbar->value;
		p=trackPercent->trackbar->value;			
		WriteCfg();						
        computeSizePos();
        init_field();
    }
    gfx_clearScreen(BG_COLOR);
    displayMineField();
    setCursor(0);
    widgetMenu->destroy(widgetMenu);
}

void WriteCfg(void)
{	
	CFG_DATA * cfg;
    cfg=cfg_readFile("minesweeper.cfg");
    if (!cfg) //if not exists
    {
        cfg=cfg_newFile();
    }
    cfg_writeInt(cfg,"MinesPercentage",p);
    cfg_writeFile(cfg,"minesweeper.cfg");
    
    cfg_clear(cfg);
}

void ReadCfg(void)
{	
	CFG_DATA * cfg;
	cfg=cfg_readFile("minesweeper.cfg");
    if (!cfg) //if not exists
    {
        p=20;
        WriteCfg();
    }	
    else
    {
		p=cfg_readInt(cfg,"MinesPercentage");
		cfg_clear(cfg);
    }
}


void MinesResult(void)
{
    int i,j;
    for(i=0;i<height;i++){
        for(j=0;j<width;j++)
		{
		    if(MINE_FIELD(i,j).known)
            {

                if(MINE_FIELD(i,j).mine)
                {
					if (MINE_FIELD(i,j).flag)
					{gfx_drawBitmap (&Flag_bitmap,FIELD_X+j*PIECE_DIM,FIELD_Y+i*PIECE_DIM);	}
					else {	gfx_drawBitmap (&Mine_bitmap,FIELD_X+j*PIECE_DIM,FIELD_Y+i*PIECE_DIM);}
                }
                else
                {
                        gfx_drawBitmap (&num_bitmap[MINE_FIELD(i,j).neighbors],
                                            FIELD_X+j*PIECE_DIM,FIELD_Y+i*PIECE_DIM);
                }
			}
			else
            {
                if(MINE_FIELD(i,j).mine)
                {
					if (MINE_FIELD(i,j).flag)
					{gfx_drawBitmap (&Flag_bitmap,FIELD_X+j*PIECE_DIM,FIELD_Y+i*PIECE_DIM);	}
					else {	gfx_drawBitmap (&Mine_bitmap,FIELD_X+j*PIECE_DIM,FIELD_Y+i*PIECE_DIM);}
                }
                else
                {
					gfx_drawBitmap (&num_bitmap[MINE_FIELD(i,j).neighbors],
                                            FIELD_X+j*PIECE_DIM,FIELD_Y+i*PIECE_DIM);
                }
            }
		}
	}
}

int winLoose_screen(char * msg)
{
    int evt;
    int stop;
    msgBox_info(msg);
    while(1)
    {
        evt=evt_getStatusBlocking(evt_handler);
        if(evt==NO_EVENT)
            continue;
        if(evt==BTN_OFF)
        {
            stop=1;
            break;
        }
        if(evt==BTN_ON)
        {
            gfx_clearScreen(BG_COLOR);
            init_field();
            displayMineField();
            setCursor(0);
            stop=0;
            break;
        }
    }
    return stop;
}

void eventHandlerLoop(void)
{
    int i=0,j=0;
	int evt;
    while(!stop)
    {
        evt=evt_getStatusBlocking(evt_handler);
		if(evt==NO_EVENT)
			continue;

        if(evt==ACTION_EXIT)
        {
            /* get out of here */
            stop=1;
        }
        else if(evt==ACTION_LEFT)
        {
            setCursor(1);
            if(x-1 >= 0)
                x--;
            else
                x = width-1;
            setCursor(0);
        }
        else if(evt==ACTION_RIGHT)
        {
            setCursor(1);
            if(x+1 >= width)
                x = 0;
            else
                x++;
            setCursor(0);
        }
        else if(evt==ACTION_DOWN)
        {
            setCursor(1);
            if(y+1 >= height)
                y = 0;
            else
                y++;
            setCursor(0);
        }
        else if(evt==ACTION_UP)
        {
            setCursor(1);
            if(y-1 >= 0)
                y--;
            else
                y = height-1;
            setCursor(0);
        }
        else if(evt==ACTION_DISCOVER)
        {
            /* do not discover on a flagged cell --> change to continue to do not escape if user not want... */
            if(MINE_FIELD(y,x).flag) continue;
            /* we init the mines position on first discover
			in order not to loose immediatly*/
            if(tiles_left == width*height) minesweeper_putmines(p,x,y);
            /* show blank */
            discover(x,y);
            if(MINE_FIELD(y,x).mine)
            {
				MinesResult();
                stop=winLoose_screen("!! You loose !!");
            }
            else
            {
                tiles_left = 0;
                for(i=0;i<height;i++){
                    for(j=0;j<width;j++){
                            if(MINE_FIELD(i,j).known == 0) tiles_left++;
                    }
                }
                if(tiles_left == mine_num)
                {
                    stop=winLoose_screen("!! You win !!");
                }
                else
                {
                    displayMineField();
                    setCursor(0);
                }
            }
        }
        else if(evt==ACTION_TOGGLE_FLAG)
        {
            MINE_FIELD(y,x).flag = (MINE_FIELD(y,x).flag + 1)%2;
            displayMineField();
        }
        else if(evt==ACTION_NEW_GAME)
        {
            gfx_clearScreen(BG_COLOR);
            init_field();
            displayMineField();
            setCursor(0);
        }
        else if(evt==ACTION_SETTINGS)
        {
            settings_screen();
        }
	}
}	

void displayMineField()
{
   int i=0,j=0;

    //display the mine field
    for(i=0;i<height;i++)
    {
        for(j=0;j<width;j++)
        {
            if(MINE_FIELD(i,j).known)
            {

                if(MINE_FIELD(i,j).mine)
                {
                    gfx_putS(TXT_B_COLOR, FIELD_BG_COLOR,
                                FIELD_X+j*PIECE_DIM+1,FIELD_Y+i*PIECE_DIM+1,
                                "b");
                }
                else
                {
                        gfx_drawBitmap (&num_bitmap[MINE_FIELD(i,j).neighbors],
                                            FIELD_X+j*PIECE_DIM,FIELD_Y+i*PIECE_DIM);
                }
            }
            else if(MINE_FIELD(i,j).flag)
            {
                gfx_fillRect(FIELD_BG_COLOR,
                                FIELD_X+j*PIECE_DIM+1,FIELD_Y+i*PIECE_DIM+1,
                                PIECE_DIM-2,PIECE_DIM-2);

				gfx_drawBitmap (&Flag_bitmap,FIELD_X+j*PIECE_DIM,FIELD_Y+i*PIECE_DIM);
					
				gfx_drawRect(CURSOR_COLOR, 
								FIELD_X+x*PIECE_DIM, FIELD_Y+y*PIECE_DIM,
								PIECE_DIM, PIECE_DIM);
								
            }
            else
            {
                gfx_fillRect(FIELD_BG_COLOR,
                                FIELD_X+j*PIECE_DIM+1,FIELD_Y+i*PIECE_DIM+1,
                                PIECE_DIM-2,PIECE_DIM-2);
            }
        }
    }
   printNumberOfMines();
}

void arch_init(void)
{
    int arch;
    int i=0,h;
    arch=getArch();
    while(conf_tab[i].conf!=NULL && conf_tab[i].arch!=arch) i++;
    if(conf_tab[i].conf==NULL)
    {
        printf("Using default conf");
        cur_conf=&default_conf;
    }
    else
    {
        printf("Using conf for %d\n",arch);
        cur_conf=conf_tab[i].conf;
    }
    
	getResolution(&screen_width,&screen_height);
    gfx_getStringSize("M",0,&h);
    max_height = height = (screen_height-h-2)/PIECE_DIM;
    max_width = width = screen_width/PIECE_DIM;
    
    printf("Init size: %d,%d\n",width,height);
    computeSizePos();
}

void init_field(void)
{
    int i,j;
    tiles_left=width*height;
    mine_num = 0;
    srand(tmr_getTick());
    for(i=0;i<height;i++)
    {
        for(j=0;j<width;j++)
        {
            MINE_FIELD(i,j).known = 0;
            MINE_FIELD(i,j).flag = 0;
        }
    }
}

/* plugin entry point */
void app_main(int argc,char * * argv)
{
/* gfx init */
    gfx_openGraphics();
    gfx_clearScreen(BG_COLOR);
/* use standard font */
    gfx_fontSet(STD6X9);
/* getting arch config */
    arch_init();
/*Get Cfg*/
	ReadCfg();
    stop=0;
/* real init */
	minefield = (tile*)malloc(sizeof(tile)*height*width);
    if(!minefield)
    {
        evt_handler=evt_getHandler(BTN_CLASS);
        msgBox_show("Memory error", "No more memory => exit\n", 
                            MSGBOX_TYPE_OK, MSGBOX_ICON_ERROR, evt_handler);
        evt_freeHandler(evt_handler);
    }
    else
    {
        init_field();
    /* initial draw */    
        displayMineField();
        setCursor(0);
    /* evt loop */
        evt_handler=evt_getHandler(BTN_CLASS);
        eventHandlerLoop();
    /* some cleanup */
        evt_freeHandler(evt_handler);
        if(minefield)
            free(minefield);
    }
}
