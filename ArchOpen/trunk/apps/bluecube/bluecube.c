/*
 * Bluecube for Av3XX
 * removed all SDL specific code
 *
 * BlueCube - just another tetris clone
 * Copyright (C) 2003  Sebastian Falbesoner
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include <api.h>

#include "global.h"
#include "grafix.h"
#include "box.h"
#include "bluecube.h"


#define NEXT_X  (screenWidth-40)
#define NEXT_Y  (boxdraw.box_y+12)

static void NewGame(void);
static void Game_Loop(void);
static void DrawScene(void);
void StartGameOverAnimation(void);
static void GameOverAnimation(void);
void ClearCluster(void);


int screenWidth,screenHeight,arch;
int bufferSize;

int lines, score;       /* Line counter and score */
int nextPiece;          /* Next cluster  */
int level;              /* Current level */

int bDone=0;     /* Want to Exit? */
int bPause;      /* Pause? */
int bCrazy;      /* Yahooooooooooo ;) CRAZY MODE ^.^ */
int bGameOver;   /* GameOver Animation? */
int bExplode;    /* Explosion is active? */
int x,y;         /* Current explosion coordinates */

int explodeY;

struct client_operations * cops;

struct blueCube_archDef * archParam=NULL;

#define ACTION_STOP           (archParam->action_stop)
#define ACTION_PAUSE          (archParam->action_pause)
#define ACTION_TURN_RIGHT_1   (archParam->action_turn_right_1)
#define ACTION_TURN_RIGHT_2   (archParam->action_turn_right_2)
#define ACTION_MOVE_LEFT      (archParam->action_move_left)
#define ACTION_MOVE_RIGHT     (archParam->action_move_right)
#define ACTION_MOVE_DOWN      (archParam->action_move_down)
#define ACTION_NEW_GAME       (archParam->action_new_game)
#define ACTION_DROP           (archParam->action_drop)

void processEvt(int event)
{
    if(bPause){
        if(event==ACTION_PAUSE)
        {
            bPause=0;
        }
        else if(event==ACTION_STOP)
        {
            bDone=1;
        }

    }else if (!bGameOver){
        if(event==ACTION_TURN_RIGHT_1 || event==ACTION_TURN_RIGHT_2)
        {
            TurnClusterRight();
        }
        else if (event==ACTION_MOVE_DOWN)
        {
            if(MoveCluster(0)){
                    NewCluster();
                }
        }
        else if (event==ACTION_MOVE_LEFT)
        {
            MoveClusterLeft();
        }
        else if (event==ACTION_MOVE_RIGHT)
        {
            MoveClusterRight();
        }
        else if (event==ACTION_PAUSE)
        {
            bPause=1;
        }
        else if (event==ACTION_STOP)
        {
            bDone=1;
        }
        else if (event==ACTION_DROP)
        {
            MoveCluster(1); /* "drop" cluster...      */
            NewCluster();   /* ... and create new one */
        }

    }else{
        if(event==ACTION_NEW_GAME)
        {
            NewGame();
        }
        else if(event==ACTION_STOP)
        {
            bDone=1;
        }
    }
}

void mainLoop(int eventHandler)
{
    int event;
    int prevTick;

    bDone=0;

    prevTick=tmr_getMicroTick();

    while(!bDone){
        event=evt_getStatus(eventHandler);

        if(tmr_getMicroTick()-prevTick>=TICK_INTERVAL){
            Game_Loop();

            prevTick=tmr_getMicroTick();
        }

        if(event){
            processEvt(event);
        }else{
            yield();
        }
    }
}

void initArch(){
    int bw,bh,bd;
    int i=0;
    arch=getArch();
    while(conf_table[i].conf!=NULL && conf_table[i].arch!=arch) i++;
    if(conf_table[i].conf==NULL)
    {
        printf("Using default conf");
        archParam=&default_conf;
    }
    else
    {
        printf("Using conf for %d\n",arch);
        archParam=conf_table[i].conf;
    }
    getResolution(&screenWidth,&screenHeight);

    gfx_planeGetSize(BMAP1,&bw,&bh,&bd);
    bufferSize=bw*bh;
}

int app_main(int argc,char** argv)
{
    int eventHandler;

    gfx_openGraphics();

    initArch();

    srand(tmr_getTick());

    /* intialise game state */
    NewGame();

    eventHandler = evt_getHandler(BTN_CLASS);

    mainLoop(eventHandler);

    evt_freeHandler(eventHandler);

    return 1;
}


/*=========================================================================
// Name: NewGame()
// Desc: Starts a new game
//=======================================================================*/
static void NewGame()
{
    InitBox(); /* Clear Box */
    BoxDrawInit(screenWidth,screenHeight); /* Init boxdraw values */

    lines = 0; /* Reset lines counter */
    score = 0; /* Reset score */
    level = 0; /* Reset level */
    nextPiece = rand()%7; /* Create random nextPiece */
    NewCluster();

    bPause = 0; /* No pause */
    bCrazy = 0; /* No crazymode :) */
    bGameOver = 0;
    bExplode = 0;

}

/*=========================================================================
// Name: Game_Loop()
// Desc: The main loop for the game
//=======================================================================*/
static void Game_Loop()
{
    if (!bPause)
    {
        if (!bGameOver)
        {
            cluster.dropCount--;  /* Decrease time until the next fall */;

            if (cluster.dropCount == 0)
            {
                ClearCluster();
                if (MoveCluster(0)) /* If cluster "collides"... */
                {
                    NewCluster();   /* then create a new one ;) */
                }
            }

            /* Increase Level */
            if (((level == 0) && (lines >=  10)) ||
                ((level == 1) && (lines >=  30)) ||
                ((level == 2) && (lines >=  50)) ||
                ((level == 3) && (lines >=  70)) ||
                ((level == 4) && (lines >=  90)) ||
                ((level == 5) && (lines >= 110)) ||
                ((level == 6) && (lines >= 130)) ||
                ((level == 7) && (lines >= 150)) ||
                ((level == 8) && (lines >= 170)) ||
                ((level == 9) && (lines >= 200)))
            {
                level++;
            }
        }
        else
        {
            if(bExplode){
                GameOverAnimation();
            }
        }

    }

    DrawScene();
}

/*=========================================================================
// Name: DrawScene()
// Desc: Draws the whole scene!
//=======================================================================*/
static void DrawScene()
{
    char chScore[30], chLines[30],chLevel[30];

    gfx_setPlane(BMAP2);

    gfx_clearScreen(COLOR_WHITE);

    /* Draw border of the box */
    PutRect(boxdraw.box_x-5, boxdraw.box_y-5,
            boxdraw.box_width + 2*5, boxdraw.box_height + 2*5, COLOR_BLACK,0,0);
    PutRect(boxdraw.box_x-1, boxdraw.box_y-1,
            boxdraw.box_width+2, boxdraw.box_height+2, COLOR_BLACK,0,0);

    sprintf(chScore, "%d", score);
    sprintf(chLines, "%d", lines);
    sprintf(chLevel, "%d", level);

    gfx_fontSet(STD6X9);

    gfx_putS(COLOR_BLACK,COLOR_WHITE, 1, boxdraw.box_y, "Score");
    gfx_putS(COLOR_BLACK,COLOR_WHITE, 1, boxdraw.box_y+12, chScore);
    gfx_putS(COLOR_BLACK,COLOR_WHITE, NEXT_X, boxdraw.box_y, "Next");


    gfx_putS(COLOR_BLACK,COLOR_WHITE, 1, boxdraw.box_y+25, "Lines");
    gfx_putS(COLOR_BLACK,COLOR_WHITE, 1, boxdraw.box_y+37, chLines);
    gfx_putS(COLOR_BLACK,COLOR_WHITE, 1, boxdraw.box_y+50, "Level");
    gfx_putS(COLOR_BLACK,COLOR_WHITE, 1, boxdraw.box_y+62, chLevel);

    DrawNextPiece(NEXT_X, NEXT_Y);

    if (!bGameOver){
        DrawCluster(); /* Draw cluster */
    }

    DrawBox(); /* Draw box bricks */

    if (bGameOver && !bExplode)
    {
        sprintf(chScore, "Your Score: %d", score);

        gfx_fontSet(STD8X13);

        gfx_putS(COLOR_BLACK,COLOR_WHITE,(screenWidth-13*8)/2,screenHeight/2-13-7,"- Game Over -");
        gfx_putS(COLOR_BLACK,COLOR_WHITE,(screenWidth-strlen(chScore)*8)/2,screenHeight/2+7,chScore);
    }

    if (bPause)
    {
        gfx_fontSet(STD8X13);

        gfx_putS(COLOR_BLACK,COLOR_WHITE,(screenWidth-10*8)/2,(screenHeight-13)/2,"- Paused -");
    }

    //the game needs double buffering
    memcpy(gfx_planeGetBufferOffset(BMAP1),gfx_planeGetBufferOffset(BMAP2),bufferSize);
}

/*=========================================================================
// Name: StartGameOverAnimation()
// Desc: Starts the gameover animation
//=======================================================================*/
void StartGameOverAnimation()
{
    x = 0;
    y = 0;
    bGameOver = 1;
    bExplode = 1;
    explodeY=0;
}

/*=========================================================================
// Name: GameOverAnimation()
// Desc: Gameover Animation!
//=======================================================================*/
static void GameOverAnimation()
{
    int i;

    for (i=0; i<BOX_BRICKS_X; i++)
        SetBrick(i,explodeY,rand()%6+1);

    explodeY++;

    if (explodeY>=BOX_BRICKS_Y) bExplode=0;
}

/*=========================================================================
// Name: PutRect()
// Desc: Draws a /UN/ filled rectangle onto the screen
//=======================================================================*/
void PutRect(int x, int y, int w, int h, int r, int g, int b)
{
    // GrSetGCForeground(tetris_gc, MWRGB(r, g, b));
    gfx_drawRect(r,x,y,w,h);
}

void ClearRect(int x, int y, int w, int h)
{
    gfx_fillRect(COLOR_WHITE,x,y,w,h);
}

void ClearBox(void)
{
    gfx_fillRect(COLOR_WHITE,boxdraw.box_x+1,boxdraw.box_y+1, boxdraw.box_width-1, boxdraw.box_height-1);
}


