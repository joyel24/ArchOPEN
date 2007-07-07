/*
* othello.c
*
* linav - http://linav.sourceforge.net
* Copyright (c) 2004 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/
#include <sys_def/colordef.h>
#include <sys_def/font.h>
#include <sys_def/evt.h>

#include <api.h>

#include "othello.h"

#define ACTION_EXIT         (cur_conf->action_exit)

#define ACTION_UP           (cur_conf->action_up)
#define ACTION_DOWN         (cur_conf->action_down)
#define ACTION_RIGHT        (cur_conf->action_right)
#define ACTION_LEFT         (cur_conf->action_left)

#define ACTION_PLAY         (cur_conf->action_play)
#define ACTION_TOGGLE_MODE  (cur_conf->action_toggle_mode)
#define ACTION_NEW_GAME     (cur_conf->action_new_game)

#define CELL_SIZE           (cur_conf->cellSize)

#define NB_PIECE_U_X          (cur_conf->nbPieceU_X)
#define NB_PIECE_O_X          (cur_conf->nbPieceO_X)
#define NB_PIECE_U_Y          (cur_conf->nbPieceU_Y)
#define NB_PIECE_O_Y          (cur_conf->nbPieceO_Y)

#define MODE_TXT_X          (cur_conf->modeTxt_X)
#define MODE_TXT_Y          (cur_conf->modeTxt_Y)

struct othello_s * cur_conf;

struct position {
    int x;
    int y;
};

struct position cursor_pos={3,2};
int board[NB_CELL][NB_CELL];
int allowedHuman[NB_CELL][NB_CELL];
int nbPieces[2]={2,2};
int endOfGame=0;
int cursorMoveMode=0; /* 0 = move anywhere, 1 = move to possible locations */
int evt_handler;
int y_ini,x_ini,piece_offset;

 void drawPiece(int x, int y, int player)
{
    switch(player)
    {
        case HUMAN:
            gfx_drawBitmap(&hBitmap,x_ini+piece_offset+x*(CELL_SIZE+1),y_ini+piece_offset+y*(CELL_SIZE+1));
            break;
        case ENEMY:
            gfx_drawBitmap(&aBitmap,x_ini+piece_offset+x*(CELL_SIZE+1),y_ini+piece_offset+y*(CELL_SIZE+1));
            break;
    }
}

 void erasePiece(int x, int y)
{
    gfx_fillRect(BG_COLOR,x_ini+1+x*(CELL_SIZE+1),y_ini+1+y*(CELL_SIZE+1),CELL_SIZE,CELL_SIZE);
}

 void drawBoard(void)
{
    int i,j;

    gfx_clearScreen(BG_COLOR);


    for(i=0;i<NB_CELL+1;i++)
    {
        /* horizontal line */
        gfx_drawLine(LINE_COLOR,x_ini,y_ini+i*(CELL_SIZE+1),x_ini+NB_CELL*(CELL_SIZE+1),y_ini+i*(CELL_SIZE+1));
        /* vertical line */
        gfx_drawLine(LINE_COLOR,x_ini+i*(CELL_SIZE+1),y_ini,x_ini+i*(CELL_SIZE+1),y_ini+NB_CELL*(CELL_SIZE+1));
    }

    for(j=0;j<NB_CELL;j++)
        for(i=0;i<NB_CELL;i++)
            if(board[i][j]!=EMPTY)
                drawPiece(i, j,board[i][j]);
}

 void selectCell(int x, int y)
{
    gfx_drawRect(SEL_COLOR,x_ini+x*(CELL_SIZE+1),y_ini+y*(CELL_SIZE+1),CELL_SIZE+2,CELL_SIZE+2);
}

 void unSelectCell(int x, int y)
{
    gfx_drawRect(LINE_COLOR,x_ini+x*(CELL_SIZE+1),y_ini+y*(CELL_SIZE+1),CELL_SIZE+2,CELL_SIZE+2);
}

 void drawNbPiece()
{
    char tmp[10];
    
    gfx_drawBitmap(&hBitmap, NB_PIECE_U_X, NB_PIECE_U_Y);
    sprintf(tmp,"You: %02d",nbPieces[HUMAN]);
    gfx_putS(TXT_COLOR, BG_COLOR, NB_PIECE_U_X+PIECE_SIZE+2 , NB_PIECE_U_Y, tmp);
    
    gfx_drawBitmap(&aBitmap, NB_PIECE_O_X, NB_PIECE_O_Y);
    sprintf(tmp,"Archos: %02d",nbPieces[ENEMY]);
    gfx_putS(TXT_COLOR, BG_COLOR, NB_PIECE_O_X+PIECE_SIZE+2, NB_PIECE_O_Y, tmp);
    
}

 void drawMenu()
{
    if(cursorMoveMode == 0)
    {
    gfx_putS(TXT_COLOR, BG_COLOR, MODE_TXT_X,MODE_TXT_Y, "Nav Mode (F3): Traditional");
    }
    else
    {
    gfx_putS(TXT_COLOR, BG_COLOR, MODE_TXT_X,MODE_TXT_Y, "Nav Mode (F3): Intelligent");
    }
}

void iniBoard(void)
{
    int i,j;

    for(j=0;j<NB_CELL;j++)
        for(i=0;i<NB_CELL;i++)
        {
            board[i][j]=EMPTY;
        }
    i=NB_CELL/2-1;

    /* setting initial piece */
    board[i][i]=ENEMY;board[i+1][i]=HUMAN;
    board[i][i+1]=HUMAN;board[i+1][i+1]=ENEMY;

    /* reset scores */
    nbPieces[1] = 2;
    nbPieces[2] = 2;
}

 void moveCursor(int dx, int dy)
{
    unSelectCell(cursor_pos.x,cursor_pos.y);
    cursor_pos.x+=dx;
    cursor_pos.y+=dy;
    selectCell(cursor_pos.x,cursor_pos.y);
}

 int tstMove(int player,int x, int y,int dx, int dy,int dispMove)
{
    int other=(player==HUMAN?ENEMY:HUMAN);
    int out_of_board=0;
    int tot=0,i;
    x+=dx;y+=dy;
    while(board[x][y]==other)
    {
        x+=dx;y+=dy;
        tot+=1;
        if(x<0 || x>=NB_CELL || y<0 || y>=NB_CELL)
        {
            out_of_board=1;
            break;
        }
    }

    if(!out_of_board && board[x][y]==player)
    {
        if(dispMove)
        {
            for(i=0;i<tot;i++)
            {
                x-=dx;y-=dy;
                erasePiece(x,y);
                drawPiece(x,y,player);
                board[x][y]=player;
            }
        }
        return tot;
    }

    return 0;

}

 int validMove(int player,int x,int y,int chkAll,int dispMove)
{
    int other=(player==HUMAN?ENEMY:HUMAN);

    int tot=0;

    if(x>=2 && board[x-1][y]==other && (tot+=tstMove(player,x,y,-1,0,dispMove)))
        if(!chkAll)
            return 1;
    if(x<=(NB_CELL-3) && board[x+1][y]==other && (tot+=tstMove(player,x,y,1,0,dispMove)))
        if(!chkAll)
            return 1;
    if(y>=2 && board[x][y-1]==other && (tot+=tstMove(player,x,y,0,-1,dispMove)))
        if(!chkAll)
            return 1;
    if(y<=(NB_CELL-3) && board[x][y+1]==other && (tot+=tstMove(player,x,y,0,1,dispMove)))
        if(!chkAll)
            return 1;

    if(x>=2 && y>=2 && board[x-1][y-1]==other && (tot+=tstMove(player,x,y,-1,-1,dispMove)))
        if(!chkAll)
            return 1;
    if(x<=(NB_CELL-3) && y>=2 && board[x+1][y-1]==other && (tot+=tstMove(player,x,y,1,-1,dispMove)))
        if(!chkAll)
            return 1;
    if(x>=2 && y<=(NB_CELL-3) && board[x-1][y+1]==other && (tot+=tstMove(player,x,y,-1,1,dispMove)))
        if(!chkAll)
            return 1;
    if(x<=(NB_CELL-3) && y<=(NB_CELL-3) && board[x+1][y+1]==other && (tot+=tstMove(player,x,y,1,1,dispMove)))
        if(!chkAll)
            return 1;

    return tot;
}

 int computeAllowed(int allowed[NB_CELL][NB_CELL],int player)
{
    int i,j,tot=0;
    for(j=0;j<NB_CELL;j++)
        for(i=0;i<NB_CELL;i++)
        {
            if(board[i][j] == EMPTY && validMove(player,i,j,JUST_TEST,JUST_TEST))
            {
                tot++;
                allowed[i][j]=1;
            }
            else
            {
                allowed[i][j]=0;
            }
        }
    return tot;
}

 int computerMove(void)
{
    int i,j,tot,x=0,y=0,max=0;
    for(j=0;j<NB_CELL;j++)
        for(i=0;i<NB_CELL;i++)
        {
            if(board[i][j] == EMPTY && (tot=validMove(ENEMY,i,j,JUST_TEST,JUST_TEST)))
            {
                if(tot>max)
                {
                    max=tot;
                    x=i;
                    y=j;
                }
            }
        }
     if(max>0)
     {
         tot=validMove(ENEMY,x,y,CHK_ALL,DO_MOVE);
         board[x][y]=ENEMY;
         drawPiece(x,y,ENEMY);
         nbPieces[ENEMY]+=tot;
         nbPieces[ENEMY]++;
         nbPieces[HUMAN]-=tot;
         return 1;
     }
     else
         return 0;

}

 void doMove(int x,int y)
{
    int tot;
    tot=validMove(HUMAN,x,y,CHK_ALL,DO_MOVE);
    board[x][y]=HUMAN;
    drawPiece(x,y,HUMAN);
    nbPieces[HUMAN]+=tot;
    nbPieces[HUMAN]++;
    nbPieces[ENEMY]-=tot;
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
            stop=0;
            break;
        }
    }
    return stop;
}

int endGame(void)
{
    if(nbPieces[HUMAN]>nbPieces[ENEMY])
        return winLoose_screen("You won!");
    else if(nbPieces[HUMAN]<nbPieces[ENEMY])
        return winLoose_screen("You lost!");
    else
        return winLoose_screen("You tied!");
}

 void nxtCursosPos(int direction,int dispMove)
{
    int i,j;
    int stop;

    if(direction == PREV_POS)
    {
        stop=0;
        j=cursor_pos.y;
        i=cursor_pos.x-1;
        while(1)
        {
            while(j>=0 && !stop)
            {
                while(i>=0 && !stop)
                {
                    if(allowedHuman[i][j]==1)
                    {
                        stop=1;
                        break;
                    }
                    i--;
                }
                if(stop)
                    break;
                i=NB_CELL-1;
                j--;
            }
            if(stop)
            {
                if(dispMove==DO_MOVE)
                {
                    moveCursor(i-cursor_pos.x,j-cursor_pos.y);
                }
                break;
            }
            i=NB_CELL-1;
            j=NB_CELL-1;
        }
    }
    else
    {
        stop=0;
        j=cursor_pos.y;
        i=cursor_pos.x+1;
        while(1)
        {
            while(j<NB_CELL)
            {
                while(i<NB_CELL)
                {
                    if(allowedHuman[i][j]==1)
                    {
                        stop=1;
                        break;
                    }
                    i++;
                }
                if(stop)
                    break;
                i=0;
                j++;
            }

            if(stop)
            {
                if(dispMove==DO_MOVE)
                    moveCursor(i-cursor_pos.x,j-cursor_pos.y);
                else
                {
                    cursor_pos.x=i;
                    cursor_pos.y=j;
                }
                break;
            }
            i=0;
            j=0;
        }
    }
}

void iniCursorPos()
{
    unSelectCell(cursor_pos.x,cursor_pos.y);
    cursor_pos.x=0;
    cursor_pos.y=0;
    nxtCursosPos(NXT_POS,NO_MOVE);
    selectCell(cursor_pos.x,cursor_pos.y);
}

 void simpleMove(int dx,int dy)
{
    int cx = cursor_pos.x;
    int cy = cursor_pos.y;

    if(cx+dx>=0 && cy+dy>=0 && cx+dx<=NB_CELL-1 && cy+dy<=NB_CELL-1)
        moveCursor(dx,dy);
}



void eventLoop()
{
    int evt;
    int stop_othello=0;
    
    evt_handler=evt_getHandler(BTN_CLASS);
    if(evt_handler<0)             /* we need a proper error handling in api */
    {
        printf("[othello init] can't register to evt\n");
        return;
    }
    
    while(!stop_othello)
    {
        evt=evt_getStatusBlocking(evt_handler);
        if(evt==NO_EVENT)
            continue;
        if(evt==ACTION_UP && cursorMoveMode==0)
        {
            simpleMove(0,-1);
        }
        else if(evt==ACTION_LEFT)
        {
            if(cursorMoveMode==0)
                simpleMove(-1,0);
            else
                nxtCursosPos(PREV_POS,DO_MOVE);
        }
        else if(evt==ACTION_DOWN && cursorMoveMode==0)
        {
            simpleMove(0,1);
        }
        else if(evt==ACTION_RIGHT)
        {
            if(cursorMoveMode==0)
                simpleMove(1,0);
            else
                nxtCursosPos(NXT_POS,DO_MOVE);
        }
        else if(evt==ACTION_PLAY && (cursorMoveMode!=0 || allowedHuman[cursor_pos.x][cursor_pos.y]==1))
        {

            doMove(cursor_pos.x,cursor_pos.y);
            computerMove();
            drawNbPiece();
            while(!computeAllowed(allowedHuman,HUMAN))
            {
                if(!computerMove())
                {
                    endOfGame=1;
                    endGame();
                    break;
                }
            }
            if(!endOfGame)
                iniCursorPos();
        }
        else if(evt==ACTION_TOGGLE_MODE)
        {   
            cursorMoveMode = !cursorMoveMode;
            drawMenu();
        }
        else if(evt==ACTION_EXIT)
        {
            stop_othello=1;
        }
        else if(evt==ACTION_NEW_GAME)
        {
            iniBoard();
            computeAllowed(allowedHuman,HUMAN);
            redraw();
        }
    }
    evt_freeHandler(evt_handler);
}

void redraw(void)
{
    drawBoard();      
    drawMenu();
    iniCursorPos();
    drawNbPiece();
}

void iniArch()
{
    int arch;
    int i=0;
    int width,height;
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
    getResolution(&width,&height);
    if(cur_conf->center_Y)
        y_ini=((height-((cur_conf->cellSize+1)*NB_CELL)-1)/2);
    else
        y_ini=cur_conf->y_ini;

    if(cur_conf->center_X)
        x_ini=((width-((cur_conf->cellSize+1)*NB_CELL)-1)/2);
    else
        x_ini=cur_conf->x_ini;
    
    piece_offset=(cur_conf->cellSize-PIECE_SIZE)/2+1;
}

void app_main(int argc, char ** argv)
{
/* gfx init */
    gfx_openGraphics();
    gfx_clearScreen(COLOR_WHITE);
/* std font */
    gfx_fontSet(STD8X13);
/* getting arch config */
    iniArch();
/* reall init */
    iniBoard();
    computeAllowed(allowedHuman,HUMAN);
    redraw();
/* let's go */
    eventLoop();
    printf("\nout othello\n");
}


