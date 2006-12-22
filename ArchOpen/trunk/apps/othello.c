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

#include <api.h>
#include <sys_def/evt.h>

struct position {
    int x;
    int y;
};

struct position cursor_pos={3,2};

#define PIECE_SIZE 12
#define NB_CELL    8
#define X_INI      10

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

#define DEBUG

int screenWidth,screenHeight,arch;
int cellSize;
int nbPieceY;

 int board[NB_CELL][NB_CELL];
 int allowedHuman[NB_CELL][NB_CELL];
 int nbPieces[2]={2,2};
int endOfGame=0;
 int cursorMoveMode=0; /* 0 = move anywhere, 1 = move to possible locations */

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

int stop_othello;

void redraw(void);


#define PIECE_OFFSET (cellSize-PIECE_SIZE)/2+1
#define Y_INI      ((screenHeight-((cellSize+1)*NB_CELL)-1)/2)


 void drawPiece(int x, int y, int player)
{
    switch(player)
    {
        case HUMAN:
            gfx_drawBitmap(&hBitmap,X_INI+PIECE_OFFSET+x*(cellSize+1),Y_INI+PIECE_OFFSET+y*(cellSize+1));
            break;
        case ENEMY:
            gfx_drawBitmap(&aBitmap,X_INI+PIECE_OFFSET+x*(cellSize+1),Y_INI+PIECE_OFFSET+y*(cellSize+1));
            break;
    }
}

 void erasePiece(int x, int y)
{
    gfx_fillRect(BG_COLOR,X_INI+1+x*(cellSize+1),Y_INI+1+y*(cellSize+1),cellSize,cellSize);
}

 void drawBoard(void)
{
    int i,j;

    gfx_clearScreen(BG_COLOR);


    for(i=0;i<NB_CELL+1;i++)
    {
        /* horizontal line */
        gfx_drawLine(LINE_COLOR,X_INI,Y_INI+i*(cellSize+1),X_INI+NB_CELL*(cellSize+1),Y_INI+i*(cellSize+1));
        /* vertical line */
        gfx_drawLine(LINE_COLOR,X_INI+i*(cellSize+1),Y_INI,X_INI+i*(cellSize+1),Y_INI+NB_CELL*(cellSize+1));
    }

    for(j=0;j<NB_CELL;j++)
        for(i=0;i<NB_CELL;i++)
            if(board[i][j]!=EMPTY)
                drawPiece(i, j,board[i][j]);
}

 void selectCell(int x, int y)
{
    gfx_drawRect(SEL_COLOR,X_INI+x*(cellSize+1),Y_INI+y*(cellSize+1),cellSize+2,cellSize+2);
}

 void unSelectCell(int x, int y)
{
    gfx_drawRect(LINE_COLOR,X_INI+x*(cellSize+1),Y_INI+y*(cellSize+1),cellSize+2,cellSize+2);
}

 void drawNbPiece()
{
    char tmp[10];
    sprintf(tmp,"You: %02d",nbPieces[HUMAN]);
    gfx_putS(TXT_COLOR, BG_COLOR, 20, nbPieceY, tmp);
    gfx_drawBitmap(&hBitmap, 4, nbPieceY);
    sprintf(tmp,"Archos: %02d",nbPieces[ENEMY]);
    gfx_putS(TXT_COLOR, BG_COLOR, 120, nbPieceY, tmp);
    gfx_drawBitmap(&aBitmap, 104, nbPieceY);
}

 void drawMenu()
{
    if(cursorMoveMode == 0)
    {
        gfx_putS(TXT_COLOR, BG_COLOR, 10,2, "Nav Mode (F3): Traditional");
    }
    else
    {
        gfx_putS(TXT_COLOR, BG_COLOR, 10,2, "Nav Mode (F3): Standard   ");
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
   // printf("[tstMove] dx=%d dy=%d\n",dx,dy);
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
            printf("\n");

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

void endGame(void)
{
    if(nbPieces[HUMAN]>nbPieces[ENEMY])
        gfx_putS(COLOR_WHITE,COLOR_BLACK,10,2,"Game Over... You won! :D  ");
    else if(nbPieces[HUMAN]<nbPieces[ENEMY])
        gfx_putS(COLOR_WHITE,COLOR_BLACK,10,2,"Game Over... You lost. :( ");
    else
        gfx_putS(COLOR_WHITE,COLOR_BLACK,10,2,"Game Over... You tied. :| ");
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



 void eventHandler(int evt)
{  
    switch(evt)
    {
        case BTN_UP:
            if(cursorMoveMode==0)
                simpleMove(0,-1);
            break;

        case BTN_LEFT:
            if(cursorMoveMode==0)
                simpleMove(-1,0);
            else
                nxtCursosPos(PREV_POS,DO_MOVE);
            break;

        case BTN_DOWN:
            if(cursorMoveMode==0)
                simpleMove(0,1);
            break;

        case BTN_RIGHT:
            if(cursorMoveMode==0)
                simpleMove(1,0);
            else
                nxtCursosPos(NXT_POS,DO_MOVE);
            break;

        case BTN_1:
#ifdef AV4XX
                case BTN_ON:
#endif
#ifndef AV4XX
                case BTN_F1:
#endif
            // BTN_1=joypress on av3xx, bad button
            if(arch==AV3XX_ARCH && evt==BTN_1) break;

            if(cursorMoveMode==0 && allowedHuman[cursor_pos.x][cursor_pos.y]!=1)
                break;

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
            break;

        case BTN_F3:
            cursorMoveMode = !cursorMoveMode;
            drawMenu();
            break;

        case BTN_OFF:
            stop_othello=1;
            break;

#ifdef AV4XX
                case BTN_F1:
#endif
#ifndef AV4XX
                case BTN_ON:
#endif
            iniBoard();
            computeAllowed(allowedHuman,HUMAN);
            redraw();
            break;

       
    }
}

void redraw(void)
{
    drawBoard();      
    drawMenu();
    iniCursorPos();
    drawNbPiece();
}

void iniArch(){
    arch=getArch();
    getResolution(&screenWidth,&screenHeight);

    if(screenWidth>=320){
        cellSize=20;
        nbPieceY=220;
    }else if(screenWidth>=220){
        cellSize=16;
        nbPieceY=160;
    }else{
        cellSize=13;
        nbPieceY=120;
    }
}

void app_main(int argc, char ** argv)
{
    int evt;
    int evt_handler;

    gfx_openGraphics();
    
    gfx_clearScreen(COLOR_WHITE);

    gfx_fontSet(STD8X13);

    iniArch();

    iniBoard();

    evt_handler=evt_getHandler(BTN_CLASS);

    if(evt_handler<0)             /* we need a proper error handling in api */
    {
        printf("[othello init] can't register to evt\n");
        return;
    }

    computeAllowed(allowedHuman,HUMAN);
    redraw();

    stop_othello=0;
    printf("\nbefore loop\n");

    while(!stop_othello)
    {
      evt=evt_getStatus(evt_handler);
      eventHandler(evt);
    }

    printf("\nafter loop\n");
    evt_freeHandler(evt_handler);

    printf("\nout othello\n");

}


