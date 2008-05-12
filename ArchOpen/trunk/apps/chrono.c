/*
*   chrono.c
*
*   MediOS project
*   Copyright (c) 2008 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
* apps created by ios, May 2008
*
*/

#include <api.h>
#include "chrono_digits.h"

#if defined(GMINI4XX) || defined(GMINI402)
#define LCD_WIDTH 220
#define LCD_HEIGHT 176
#define Xstart 5
#define Ystart 166
#define Xreset 95
#define Yreset 166
#define Xsave 180
#define Ysave 166
#endif

#if defined(AV3XX) || defined(AV4XX)
#define LCD_WIDTH 320
#define LCD_HEIGHT 240
#define Xstart 250
#define Ystart 240-70
#define Xreset 260
#define Yreset 240-40
#define Xsave 260
#define Ysave 240-10
#endif


#if defined(AV5XX)
#define LCD_WIDTH 480
#define LCD_HEIGHT 272
#define Xstart 420
#define Ystart 272-50
#define Xreset 420
#define Yreset 272-30
#define Xsave 420
#define Ysave 272-10
#endif


#if defined(PMA)
#define LCD_WIDTH 320
#define LCD_HEIGHT 240
#define Xstart 250
#define Ystart 240-70
#define Xreset 260
#define Yreset 240-40
#define Xsave 260
#define Ysave 240-10
#endif




char chaine[14];
int time0,time,timepause;
int seconde,minute,centieme;
int evt,start;
int quitter=1;
int evt_handler;
char score[5][14];
int i,k,l;


void dessiner(int ch,int x ,int y )
 {
     //dessine le chiffre ch auxcoordon�es x,y
for(k=0;k<30;k++)for(i=0;i<54;i++) gfx_drawPixel(chiffre[ch][i][k], x+k, y+i);
 }



void afficher()
 {
  sprintf(chaine,"%2d : %2d : %2d ",minute,seconde,centieme);
  
  //affiche le chrono�etre
  dessiner(minute/10,LCD_WIDTH/2-3*34-5,40 );dessiner(minute-(minute/10)*10,LCD_WIDTH/2-2*34-5 ,40 );
  dessiner(seconde/10,LCD_WIDTH/2-34 ,40 );dessiner(seconde-(seconde/10)*10,LCD_WIDTH/2 ,40 );
  dessiner(centieme/10,34+5+LCD_WIDTH/2 ,40 );  dessiner(centieme-(centieme/10)*10,2*34+LCD_WIDTH/2+5 ,40 );
  
  
 
  //affiche les sauvegarde
  gfx_putS(COLOR_BLACK,COLOR_WHITE,LCD_WIDTH/2-50,70+30,score[0]); gfx_putS((5-l==5)*COLOR_BLACK,COLOR_WHITE,LCD_WIDTH/2+40,70+30,"<=");
  gfx_putS(COLOR_BLACK,COLOR_WHITE,LCD_WIDTH/2-50,81+30,score[1]);gfx_putS((6-l==5)*COLOR_BLACK,COLOR_WHITE,LCD_WIDTH/2+40,81+30,"<=");
  gfx_putS(COLOR_BLACK,COLOR_WHITE,LCD_WIDTH/2-50,92+30,score[2]);gfx_putS((7-l==5)*COLOR_BLACK,COLOR_WHITE,LCD_WIDTH/2+40,92+30,"<=");
  gfx_putS(COLOR_BLACK,COLOR_WHITE,LCD_WIDTH/2-50,103+30,score[3]);gfx_putS((8-l==5)*COLOR_BLACK,COLOR_WHITE,LCD_WIDTH/2+40,103+30,"<=");
  gfx_putS(COLOR_BLACK,COLOR_WHITE,LCD_WIDTH/2-50,114+30,score[4]);gfx_putS((9-l==5)*COLOR_BLACK,COLOR_WHITE,LCD_WIDTH/2+40,114+30,"<=");
  
  
 }
 void save()
 {
    //sauvegarder
    sprintf(score[l],chaine);
    afficher();
    l=(l+1)-((l+1)/5)*5;
}
      

void eventHandler(int evt)
{  
    switch(evt)
    {
        case BTN_F1:
             //start/stop
            start= !start;
            if(start==0) timepause=centieme+100*seconde+6000*minute;
            if(start==1) time0=tmr_getTick()-timepause;
            
            break;

        case BTN_F2:
             //reszt
            start=0;
            timepause=0;
            seconde=0;centieme=0;minute=0;
            l=0;
            afficher();
            break;
            
        case BTN_OFF:
             //quit
            quitter=0;
            break;
            
        case BTN_ON:
            save();
            break;
        case BTN_F3:
             //save
            save();
            break;
     }
}



void app_main(int argc,char * * argv)
{
    gfx_clearScreen(COLOR_WHITE);
    
    gfx_putS(COLOR_BLACK,COLOR_WHITE,LCD_WIDTH/2-35,10,"chronometre");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,Xreset,Yreset,"reset");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,Xstart,Ystart,"start/stop");
    gfx_putS(COLOR_BLACK,COLOR_WHITE,Xsave,Ysave,"save");
    
    time0=tmr_getTick();
    evt_handler=evt_getHandler(BTN_CLASS);
    
    afficher();
    
    
    //boucle du programme
    while(quitter==1){
                if (start==1){
                   centieme=(tmr_getTick()-time0)-((tmr_getTick()-time0)/100)*100;
                   seconde=((tmr_getTick()-time0)/100)-(((tmr_getTick()-time0)/100)/60)*60;
                   minute=(tmr_getTick()-time0)/6000;
                   
                   afficher();};
                
                
                evt=evt_getStatus(evt_handler);
                eventHandler(evt);
                };
    
evt_freeHandler(evt_handler);
}
