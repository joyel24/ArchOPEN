/*
*   memory.c
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
*/

#include <api.h>
#include <sys_def/evt.h>
#include <sys_def/stddef.h>
#include <sys_def/colordef.h>
#include <sys_def/font.h>

#define PIECE_DIM 30

#define CURSORFRAME 2

#define BG_COLOR COLOR_ORANGE
#define HIDDEN_CELL COLOR_TURQUOISE
#define CURSOR_COLOR COLOR_DARK_BLUE

#include "memory.h"

struct memory_s * cur_conf;

#define X_START  x_start;
#define Y_START  y_start;

/* the cursor coordinates */
int x=0,y=0;
int matrixSize = 0;
int turns = 0;

int x_start,y_start;

int screen_width;
int screen_height;
int arch;

/* action def */
#define ACTION_EXIT cur_conf->action_exit
#define ACTION_NEWGAME cur_conf->action_newgame
#define ACTION_LEFT cur_conf->action_left
#define ACTION_RIGHT cur_conf->action_right
#define ACTION_UP cur_conf->action_up
#define ACTION_DOWN cur_conf->action_down
#define ACTION_TOGGLE cur_conf->action_toggle
#define ACTION_SETTINGS cur_conf->action_settings

/* the height and width of the field */
/* could be variable if malloc worked in the API :) */
int height;
int width;

#define MAX_MATRIX_SIZE cur_conf->max_matrixSize
#define CUR_SIZE(W,H) {W=matrixSizeTab[matrixSize][0]; H=matrixSizeTab[matrixSize][1];}

char * size_str[] = {
    "8x8",
    "8x7",
    "6x6",
    "6x5",
    "4x4",
};

#define NB_SIZE 5

int matrixSizeTab[NB_SIZE][2] = {{8,7},{8,7},{6,6},{6,5},{4,4}};

/* the field */
tile field[MAX_HEIGHT][MAX_WIDTH];

int cntPairsLeft = 0;

void displayField();
testpair testField();
int testPair();
pair getPair();
void setCursor(int del);

int evt_handler;

void endOfGame()
{
    msgBox_info("!! You Win !!");
}

void showInfo()
{
    char tmp[30];
    int w,h;

    sprintf(tmp,"Left %d - Turns: %d", cntPairsLeft,turns);
    gfx_getStringSize(tmp,&w,&h);
    gfx_fillRect(BG_COLOR,0,screen_height-1-h,screen_width,h+1);
    gfx_putS(COLOR_BLACK, BG_COLOR, (screen_width-w)/2, screen_height-1-h, tmp);
}

void mSize_onChange(CHOOSER t)
{
    if(t->index < MAX_MATRIX_SIZE)
        t->index=MAX_MATRIX_SIZE;
    t->paint(t);
}

void settings_screen()
{
    int h,w,event,y;
    char tmp[50];
    WIDGETMENU_CHOOSER mSize;
    WIDGETMENU widgetMenu; 
       
    gfx_clearScreen(BG_COLOR); // clear    
    gfx_getStringSize("SETTINGS",&w,&h);
    gfx_putS(COLOR_BLACK, BG_COLOR, (screen_width-w)/2, 5,  "SETTINGS");
    
    y=5+h+2;
    
    widgetMenu=widgetMenu_create();
    widgetMenu->backColor=BG_COLOR;
    widgetMenu->fillColor=BG_COLOR;
    widgetMenu->setRect(widgetMenu,5,y,screen_width-5,h+4);
    widgetMenu->ownWidgets=true; // the menu will handle items destroy
    widgetMenu->font=STD6X9;
    
    
    mSize = widgetMenuChooser_create();
    mSize->caption="Size:";
    mSize->chooser->items=size_str;
    mSize->chooser->itemCount=NB_SIZE;
    mSize->chooser->index=matrixSize;
    mSize->chooser->wrap=WIDGET_WRAP_OFF;
    mSize->chooser->onChange=(CHOOSER_CHANGEEVENT)mSize_onChange;
    widgetMenu->addItem(widgetMenu,mSize);    
    
    widgetMenu->setFocusedWidget(widgetMenu,mSize);
    widgetMenu->paint(widgetMenu);
    
    y = y+h+4+5;
    gfx_getStringSize("M",&w,&h);
    gfx_putS(COLOR_BLACK, BG_COLOR, 10,y, "Direction: Joystick");
    y = y+h+2;
    sprintf(tmp,"Toggle: %s",getBtnName(ACTION_TOGGLE));
    gfx_putS(COLOR_BLACK, BG_COLOR, 10,y, tmp);
    y = y+h+2;
    sprintf(tmp,"New game: %s",getBtnName(ACTION_NEWGAME));
    gfx_putS(COLOR_BLACK, BG_COLOR, 10,y,tmp );
    y = y+h+2;
    sprintf(tmp,"Exit: %s",getBtnName(ACTION_EXIT));
    gfx_putS(COLOR_BLACK, BG_COLOR, 10,y, tmp);
    
    do{
        event=evt_getStatusBlocking(evt_handler);
        if (!event) continue; // no new events
        if(widgetMenu->handleEvent(widgetMenu,event))
            continue;
    }while(event!=BTN_OFF); 
    
    widgetMenu->destroy(widgetMenu);
    
    matrixSize=mSize->chooser->index;
    CUR_SIZE(width,height);
    x_start = (screen_width-(CURSORFRAME+width*(PIECE_DIM+CURSORFRAME)))/2;
    y_start = (screen_height-(CURSORFRAME+height*(PIECE_DIM+CURSORFRAME)+1+h))/2;
}

coord GetXY(int pos)
{
    coord xy;
    int index = 0;

    x = 0;
    y = 0;

    do
    {
        if(x+1 < width) x++; else x=0;
        if(x==0) y++;

        index++;
    }
    while ( index < pos );

    xy.x = x;
    xy.y = y;

    return xy;
}

void GenerateField()
{
    int i = 0;
    int j = 0;
    int k = 0;
    int map_size = height*width;
    int pos1 = 0;
    int pos2 = 0;
    int index = 0;
    int tmpIdent = 0;
    coord xy1;
    coord xy2;

    // fill map
    while ( index < map_size)
    {
        field[j][i].identifier = k+1;

        if(i+1 < width) i++; else i=0;
        if(i==0) j++;

        if(k+1>(map_size/2)-1) k=0; else k++;
        index++;
    }

    // unsort these pairs
    for ( i = 0; i < map_size * 20; i++ )
    {
        pos1 = rand() % map_size;
        pos2 = rand() % map_size;

        xy1 = GetXY(pos1);
        xy2 = GetXY(pos2);

        tmpIdent = field[xy1.y][xy1.x].identifier;
        field[xy1.y][xy1.x].identifier = field[xy2.y][xy2.x].identifier;
        field[xy2.y][xy2.x].identifier = tmpIdent;

    }
}


/* init not mine related elements of the mine field */
void init(void){
    int i,j;

    srand(tmr_getTick());

    msgBox_info("Generating Matrix...");

    cntPairsLeft = (height*width)/2;
    x = 0;
    y = 0;
    turns = 0;

    for(i=0;i<height;i++){
        for(j=0;j<width;j++){
            field[i][j].known      = 0;
            field[i][j].identifier = 0;
            field[i][j].test       = 0;
        }
    }

    GenerateField();

    gfx_clearScreen(BG_COLOR);
/* need to put this in the setting screen 
    gfx_putS(COLOR_BLACK, BG_COLOR, 271,13, "New game");
    gfx_putS(COLOR_BLACK, BG_COLOR, 295,47, "Quit");
    gfx_putS(COLOR_WHITE, BG_COLOR, 274,90, "Memory");
    gfx_putS(COLOR_WHITE, BG_COLOR, 265,105, "by Schoki");
    gfx_putS(COLOR_BLACK, BG_COLOR, 271,150, "Pick up");
    gfx_putS(COLOR_BLACK, BG_COLOR, 271,210, "Settings");
*/
    showInfo();
    displayField(0);
    setCursor(0);
}

void setCursor(int del)
{
    if(del)
    {
	    gfx_drawRect(BG_COLOR,x_start+x*PIECE_DIM+x*CURSORFRAME,
                     y_start+y*PIECE_DIM+y*CURSORFRAME,
                PIECE_DIM+(2*CURSORFRAME), PIECE_DIM+(2*CURSORFRAME));
        gfx_drawRect(BG_COLOR,x_start+x*PIECE_DIM+x*CURSORFRAME+1,
                     y_start+y*PIECE_DIM+y*CURSORFRAME+1,
                PIECE_DIM+(2*CURSORFRAME)-2, PIECE_DIM+(2*CURSORFRAME)-2);
    }
    else
    {
        gfx_drawRect(CURSOR_COLOR,x_start+x*PIECE_DIM+x*CURSORFRAME,
                     y_start+y*PIECE_DIM+y*CURSORFRAME,
                PIECE_DIM+(2*CURSORFRAME), PIECE_DIM+(2*CURSORFRAME));
        gfx_drawRect(CURSOR_COLOR,x_start+x*PIECE_DIM+x*CURSORFRAME+1,
                     y_start+y*PIECE_DIM+y*CURSORFRAME+1,
                PIECE_DIM+(2*CURSORFRAME)-2, PIECE_DIM+(2*CURSORFRAME)-2);
    }
}

void eventHandlerLoop(void)
{
    pair tmp;
    int gameover = 0;
    testpair test;
    int evt;
    
    int stop=0;
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
        else if(evt==ACTION_TOGGLE)
        {
            test = testField();
            if( ((test.x != x) || (test.y != y)) && (field[y][x].known == 0) )
            {
                field[y][x].test = 1;
                displayField(0);
                if(test.test > 0)
                {
                    turns++;
                    showInfo();
                    tmp = getPair();
                    if(testPair() == 1)
                    {
                        field[tmp.y1][tmp.x1].known = 1;
                        field[tmp.y2][tmp.x2].known = 1;
                        cntPairsLeft--;
                        showInfo();
                        if(cntPairsLeft == 0)
                        {
                            endOfGame();
                            gameover = 1;
                        }
                    }
                    else
                    {
                        mdelay(100);
                    }

                    field[tmp.y1][tmp.x1].test = 0;
                    field[tmp.y2][tmp.x2].test = 0;

                    if(gameover == 0)
                    {
                        displayField(0);
                    }
                    else
                    {
                       gameover = 0;
                    }
                }
            }
        }
        else if(evt==ACTION_NEWGAME)
        {
            gfx_clearScreen(BG_COLOR);
            init();
            displayField(0);
            setCursor(0);
        }
        else if(evt==ACTION_SETTINGS)
        {
            gfx_clearScreen(BG_COLOR);
            settings_screen();
            gfx_clearScreen(BG_COLOR);
            init();
        }
    }
    
}

testpair testField()
{
    int i=0,j=0;
    testpair tmp;

    memset(&tmp, -1, sizeof(tmp));

	for(i=0;i<height;i++)
	{
        for(j=0;j<width;j++)
        {
            if(field[i][j].test)
            {
                tmp.x = j;
                tmp.y = i;
                tmp.test = field[i][j].identifier;
                return tmp;
            }
        }
	}

    return tmp;
}

pair getPair()
{
    pair tmp;
    int i=0,j=0;
    int count = 0;

	for(i=0;i<height;i++)
	{
        for(j=0;j<width;j++)
        {
            if(field[i][j].test)
            {
                if(count==0)
                {
                    tmp.x1 = j;
                    tmp.y1 = i;
                }
                else
                {
                    tmp.x2 = j;
                    tmp.y2 = i;
                }

                count++;
            }
        }
	}

    return tmp;
}

int testPair()
{
    int i=0,j=0;
    int ident1 = 0;
    int ident2 = 0;
    int count = 0;

	for(i=0;i<height;i++)
	{
        for(j=0;j<width;j++)
        {
            if(field[i][j].test)
            {
                if(count==0)
                    ident1 = field[i][j].identifier;
                else
                    ident2 = field[i][j].identifier;

                count++;
            }
        }
	}

    // Paar gefunden ?
    if(ident1 == ident2)
    {
        return 1;
    }

    return 0;
}

void displayField(int showAll)
{
   int i=0,j=0;

	//display the field
	for(i=0;i<height;i++)
	{
        for(j=0;j<width;j++)
        {
            if( (field[i][j].known) || (field[i][j].test) || showAll)
            {
                gfx_drawBitmap (&img[field[i][j].identifier-1],
                                    x_start+CURSORFRAME+j*(PIECE_DIM+CURSORFRAME),
                                    y_start+CURSORFRAME+i*(PIECE_DIM+CURSORFRAME));
            }
            else
            {
                gfx_fillRect(HIDDEN_CELL,
                            x_start+CURSORFRAME+j*(PIECE_DIM+CURSORFRAME),
                            y_start+CURSORFRAME+i*(PIECE_DIM+CURSORFRAME),
                            PIECE_DIM,PIECE_DIM);
            }
        }
	}
}

void arch_init(void)
{
    int h,i=0;
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
    matrixSize=MAX_MATRIX_SIZE;
    CUR_SIZE(width,height);
    getResolution(&screen_width,&screen_height);

    gfx_getStringSize("M",0,&h);
    x_start = (screen_width-(CURSORFRAME+width*(PIECE_DIM+CURSORFRAME)))/2;
    y_start = (screen_height-(CURSORFRAME+height*(PIECE_DIM+CURSORFRAME)+1+h));
}


/* plugin entry point */
void app_main(int argc,char * * argv)
{
    gfx_openGraphics();

    gfx_clearScreen(BG_COLOR);

    gfx_fontSet(STD6X9);

    arch_init();

    init();
    evt_handler=evt_getHandler(BTN_CLASS);
    eventHandlerLoop();
    evt_freeHandler(evt_handler);
}

