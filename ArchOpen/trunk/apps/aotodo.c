/*  aotodo
   apps for MediOS project
   Copyright (c) 2006 by Clem-vangelis (clem_joy@msn.com.com)

 All files in this archive are subject to the GNU General Public License.
 See the file COPYING in the source tree root for full license agreement.
 This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 KIND, either express of implied.
------------------------------------------------------------------------------
  File : aotodo.c		                Author : Clem-vangelis
  Created : 18/12/2006
  

----------------------------------------------------------------------------*/
#include 	<api.h>

/*************************
*   Define		 *
**************************/

int LCD_WIDTH=220;
int LCD_HEIGTH=176;
int LCD_MIDDLE=110;

#define MAXTASKLENGTH			300
#define MAX_CHAR_PER_LINE               33//max de char a afficher sur une ligne 
#define BAR_HEIGHT		        20//hauteur du header
#define BUTTON_HEIGHT			12
#define BUTTON_WIDTH			74
#define Y_RECT_MENU			55
#define X_RECT_MENU                     14
#define RECT_MENU_HEIGHT		21
#define HEADER_EDIT                     0
#define DESCRIPTION_EDIT                1
#define ADVANCEMENT_EDIT                2
#define PRIORITY_EDIT                   3
#define NEXT_TASK			1	
#define PREV_TASK			2
#define BACKTOMENU			3
#define SELTASK				4
#define EDIT_TASK			5
#define DEL_TASK                        6
#define CREATE_TASK                     7
#define TODOFILE			"/medios/data/aotodo/todo.aot"
#define THEMEFILE			"/medios/data/aotodo/theme.aot"
#define X_INFOBOX                       40
#define Y_INFOBOX                       25
#define TASK_MENU		        0	
#define MAIN_MENU	                1
#define EDIT_MENU		        2
#define DEL_MENU		        3
#define CREATE_MENU                     4
#define TILE_KEYBOARD                   20
#define WT_KEYBOARD                     200
#define HT_KEYBOARD                     100
#define NUMBER_OF_TILE                  (WT_KEYBOARD/TILE_KEYBOARD)*(HT_KEYBOARD/TILE_KEYBOARD)
//coordonnées dépendantes des variables LCD               
int Y_KEYBOARD;
int X_KEYBOARD;
int RECT_MENU_WIDTH;

/*************************
*   bitmap definitions   *
**************************/
//00 = black
//15 = white


unsigned char arrondigauche[14][14]= {
{00,00,00,00,00,00,00,00,00,00,00,00,00,00},
{00,00,00,00,00,00,00,00,00,00,00,15,15,15},
{00,00,00,00,00,00,00,00,00,15,15,15,15,15},
{00,00,00,00,00,00,00,15,15,15,15,15,15,15},
{00,00,00,00,00,00,15,15,15,15,15,15,15,15},
{00,00,00,00,00,15,15,15,15,15,15,15,15,15},
{00,00,00,00,15,15,15,15,15,15,15,15,15,15},
{00,00,00,15,15,15,15,15,15,15,15,15,15,15},
{00,00,00,15,15,15,15,15,15,15,15,15,15,15},
{00,00,15,15,15,15,15,15,15,15,15,15,15,15},
{00,00,15,15,15,15,15,15,15,15,15,15,15,15},
{00,15,15,15,15,15,15,15,15,15,15,15,15,15},
{00,15,15,15,15,15,15,15,15,15,15,15,15,15},
{00,15,15,15,15,15,15,15,15,15,15,15,15,15},
};

unsigned char arrondidroit[14][14]= {
{00,00,00,00,00,00,00,00,00,00,00,00,00,00},
{15,15,15,00,00,00,00,00,00,00,00,00,00,00},
{15,15,15,15,15,00,00,00,00,00,00,00,00,00},
{15,15,15,15,15,15,15,00,00,00,00,00,00,00},
{15,15,15,15,15,15,15,15,00,00,00,00,00,00},
{15,15,15,15,15,15,15,15,15,00,00,00,00,00},
{15,15,15,15,15,15,15,15,15,15,00,00,00,00},
{15,15,15,15,15,15,15,15,15,15,15,00,00,00},
{15,15,15,15,15,15,15,15,15,15,15,00,00,00},
{15,15,15,15,15,15,15,15,15,15,15,15,00,00},
{15,15,15,15,15,15,15,15,15,15,15,15,00,00},
{15,15,15,15,15,15,15,15,15,15,15,15,15,00},
{15,15,15,15,15,15,15,15,15,15,15,15,15,00},
{15,15,15,15,15,15,15,15,15,15,15,15,15,00},
};

unsigned char arrondibasdroit[14][14]= {
{15,15,15,15,15,15,15,15,15,15,15,15,15,00},
{15,15,15,15,15,15,15,15,15,15,15,15,15,00},
{15,15,15,15,15,15,15,15,15,15,15,15,15,00},
{15,15,15,15,15,15,15,15,15,15,15,15,00,00},
{15,15,15,15,15,15,15,15,15,15,15,15,00,00},
{15,15,15,15,15,15,15,15,15,15,15,00,00,00},
{15,15,15,15,15,15,15,15,15,15,15,00,00,00},
{15,15,15,15,15,15,15,15,15,15,00,00,00,00},
{15,15,15,15,15,15,15,15,15,00,00,00,00,00},
{15,15,15,15,15,15,15,15,00,00,00,00,00,00},
{15,15,15,15,15,15,15,00,00,00,00,00,00,00},
{15,15,15,15,15,00,00,00,00,00,00,00,00,00},
{15,15,15,00,00,00,00,00,00,00,00,00,00,00},
{00,00,00,00,00,00,00,00,00,00,00,00,00,00},
};

unsigned char arrondibasgauche[14][14]= {
{00,15,15,15,15,15,15,15,15,15,15,15,15,15},
{00,15,15,15,15,15,15,15,15,15,15,15,15,15},
{00,15,15,15,15,15,15,15,15,15,15,15,15,15},
{00,00,15,15,15,15,15,15,15,15,15,15,15,15},
{00,00,15,15,15,15,15,15,15,15,15,15,15,15},
{00,00,00,15,15,15,15,15,15,15,15,15,15,15},
{00,00,00,15,15,15,15,15,15,15,15,15,15,15},
{00,00,00,00,15,15,15,15,15,15,15,15,15,15},
{00,00,00,00,00,15,15,15,15,15,15,15,15,15},
{00,00,00,00,00,00,15,15,15,15,15,15,15,15},
{00,00,00,00,00,00,00,15,15,15,15,15,15,15},
{00,00,00,00,00,00,00,00,00,15,15,15,15,15},
{00,00,00,00,00,00,00,00,00,00,00,15,15,15},
{00,00,00,00,00,00,00,00,00,00,00,00,00,00},
};

BITMAP arrondid = {(unsigned int) arrondidroit, 14, 14, 0, 0};
BITMAP arrondig = {(unsigned int) arrondigauche, 14, 14, 0, 0};
BITMAP arrondibd = {(unsigned int) arrondibasdroit, 14, 14, 0, 0};
BITMAP arrondibg = {(unsigned int) arrondibasgauche, 14, 14, 0, 0};

/*************************
*   functions headers    *
**************************/
//init_arch
void init_arch();


//gfx stuff
void draw_header(char header []);
void draw_advancement_bar(int bar_color, int bar_bg_color,int border_color , int x, int y ,int w , int h,int advancement);
void draw_buttons(char f1[],char f2[],char f3[]);
void draw_priority(int priority,int color1,int color2, int x,int y);
void draw_task();
void draw_infobox(char text1 [],char text2 []);
void draw_vkeyboard(int sel);
void draw_vkeyboard_letters(int firstdisp,int face);
void draw_text(char text[]);
//misc
void del_last_char();
void draw_intro();
//menus
void draw_main_menu();
void draw_edit_menu();
void draw_edit_priority_menu();
void draw_edit_advancement_menu();
void draw_edit_text_menu();

//events
void launch_sel_menu(int sel);
void evt_switcher(int evt_number);

//verifications
void verify_tasks();
void verify_color_value();

//tasklist operations
int delete_or_save_task(int op);
void really_delete_task();
int dump_file();
int create_tasklist();

//theme related
int parse_theme_file();
int create_theme_file();


/*********************
*   task struct      *
**********************/
typedef struct tache
{    
	char* task;
	char* todo;
	int priority;
	int advancement;
} tache;

/*********************
*   global vars      *
**********************/
//temp struct to add a task
tache tmp;//={NULL,NULL,0,0};

//global vars
int tasks;
int cur_task;
int in_menu;
tache* liste=NULL;
char tmpstring[50];//var temporaire pour affichage de chaine
char tmpmod[MAXTASKLENGTH];//var de stockage de chaine pour modification
int stop_app=0;
int evt_handler;
int intro=0;

/*********************
*   colors vars      *
**********************/
int color[6]={COLOR_BLACK,COLOR_WHITE,COLOR_GREY,COLOR_LIGHT_YELLOW,COLOR_LIGHT_BLUE,COLOR_LIGHT_GREEN};
              //color[0]  color [1]    color[2]
/*********************
*   GFX functions    *
**********************/
void draw_header(char header []){
	int h,w;
	gfx_fillRect(color[0],0,0,LCD_WIDTH,BAR_HEIGHT);
	//on met du blanc sur le fond
	gfx_fillRect(color[1],14,BAR_HEIGHT,LCD_WIDTH-(14*2),14);
	gfx_fillRect(color[1],0,BAR_HEIGHT+14,LCD_WIDTH,LCD_HEIGTH-BAR_HEIGHT-(14*2)-BUTTON_HEIGHT);
	gfx_fillRect(color[1],14,LCD_HEIGTH-BUTTON_HEIGHT-14,LCD_WIDTH-(14*2),14);
		
	gfx_drawBitmap(&arrondig,0,BAR_HEIGHT);
	gfx_drawBitmap(&arrondid,LCD_WIDTH-14,BAR_HEIGHT);
	
	gfx_getStringSize(header,&w,&h);
	gfx_putS(color[1],color[0],LCD_MIDDLE-(w/2),(BAR_HEIGHT/2)-(h/2),header);
}

/***********************************************
*   draw advancement bar in function of menu   *
************************************************/
void draw_advancement_bar(int bar_color, int bar_bg_color,int border_color , int x, int y ,int w , int h,int advancement){
        int w1,h1;
	if(in_menu == MAIN_MENU){
		gfx_fillRect(bar_bg_color, x, y,w,h);
		gfx_fillRect(bar_color, x, y,((advancement*w)/100),h);
	}
	else if ( in_menu == TASK_MENU || in_menu == EDIT_MENU || in_menu == CREATE_TASK){
		gfx_fillRect(bar_bg_color, x, y,w,h);
		gfx_fillRect(bar_color, x, y,((advancement*w)/100),h);
		gfx_drawRect(border_color, x, y,w,h);
		gfx_drawRect(border_color, x+1, y+1,w-2,h-2);
		gfx_drawRect(color[1],x,y,2,2);
		gfx_drawRect(color[1],x+w-2,y,2,2);
		gfx_drawRect(color[1],x+w-2,y+h-2,2,2);
		gfx_drawRect(color[1],x,y+h-2,2,2);
		gfx_drawPixel(border_color,x+1,y+1);//haut gauche
		gfx_drawPixel(border_color,x+w-2,y+1);//haut droit
		gfx_drawPixel(border_color,x+1,y+h-2);//bas gauche
		gfx_drawPixel(border_color,x+w-2,y+h-2);//bas droit
                sprintf(tmpstring,"%ld %",advancement);
                gfx_getStringSize(tmpstring,&w1,&h1);
                gfx_fillRect(color[1],LCD_WIDTH-15-40,y-10,40,h1);
                gfx_putS(color[0],color[1],LCD_WIDTH-15-w1,y-10,tmpstring);
	}
}

/*********************
*   draw buttons     *
**********************/
void draw_buttons(char f1[],char f2[],char f3[]){
	int w,h,x,y,i;
	
	if(in_menu == MAIN_MENU){
		//dessin de la fleche du haut
		x=LCD_MIDDLE-1;
		y= BAR_HEIGHT+8;
		for (i=0; i<=12 ; i++) {
			gfx_drawLine(color[2],x-i,y+i,x+i,y+i);
		}
		//dessin de la fleche du bas
		x=LCD_MIDDLE-1;
		y= LCD_HEIGTH-16;
		for (i=0; i<=12 ; i++) {
			gfx_drawLine(color[2],x-i,y-i,x+i,y-i);
		}
	}
                gfx_drawBitmap(&arrondibg,0,LCD_HEIGTH-BUTTON_HEIGHT-14);
                gfx_drawBitmap(&arrondibd,LCD_WIDTH-14,LCD_HEIGTH-BUTTON_HEIGHT-14);
                gfx_fillRect(color[0],0,LCD_HEIGTH-BUTTON_HEIGHT,LCD_WIDTH,BUTTON_HEIGHT);
                //bouton edit
                gfx_getStringSize(f1,&w,&h);
                gfx_putS(color[1],color[0],(LCD_WIDTH/4)-(w/2)-6,((LCD_HEIGTH-(BUTTON_HEIGHT/2)))-(h/2),f1);
                //bouton delete
                gfx_getStringSize(f2,&w,&h);
                gfx_putS(color[1],color[0],(LCD_WIDTH/2)-(w/2),((LCD_HEIGTH-(BUTTON_HEIGHT/2)))-(h/2),f2);
                //bouton create
                gfx_getStringSize(f3,&w,&h);
                gfx_putS(color[1],color[0],(3*(LCD_WIDTH/4))-(w/2)+6,((LCD_HEIGTH-(BUTTON_HEIGHT/2)))-(h/2),f3);
}
/******************************
   draw a virtual keyboard    *
*******************************/
void draw_vkeyboard_letters(int firstdisp ,int face)
{
        int i=0,j=0,h,w;
        int x=X_KEYBOARD; 
	int y=Y_KEYBOARD;
	if(firstdisp)
                gfx_fillRect(color[0],x,y,WT_KEYBOARD,HT_KEYBOARD);
        
        //on affiche les caracteres
	int da=0,fa=0;
	if(face == 1){
	        da=33;
	        fa=77;
	}
	if(face == 2){
	        da=78;
	        fa=122;
	}
	//on vérifie que les chars sont affichables
	int l=0;
	i=0;
	gfx_getStringSize("a",&w,&h);
	for (l=da; l<=fa; l++) {
                if (i!=0 && i%WT_KEYBOARD==0){
                        i=0;
                        j+=TILE_KEYBOARD;        
                }
		gfx_putC(color[1],color[0],x+i+(TILE_KEYBOARD/2)-(w/2),y+j+(TILE_KEYBOARD/2)-(w/2),l);
		i+=TILE_KEYBOARD;
	}
	//on affiche space
	gfx_getStringSize("SPACE",&w,&h);
	gfx_putS(color[1],color[0],x+6*TILE_KEYBOARD+(TILE_KEYBOARD/2)-(w/2),y+(4*TILE_KEYBOARD)+(TILE_KEYBOARD/2)-(h/2),"SPACE");
	//on affiche le bouton delete
	gfx_getStringSize("DEL",&w,&h);
	gfx_putS(color[1],color[0],x+(9*TILE_KEYBOARD)-(w/2),y+(4*TILE_KEYBOARD)+(TILE_KEYBOARD/2)-(h/2),"DEL");

}
//trace la grille du clavier virtuel et affiche la case sélectionnée en vert
void draw_vkeyboard(int sel)
{
	int i=0,h=0;
	int x=X_KEYBOARD; 
	int y=Y_KEYBOARD;
	int position[2]={0,0};//premiere case = ligne , 2eme = colonne
	
	//on trace la grille
	while(i<=HT_KEYBOARD){
	        gfx_drawLine(color[4],x,y+i,x+WT_KEYBOARD,y+i);
	        i+=TILE_KEYBOARD;
	}
	i=0;
	while(i<=WT_KEYBOARD){
	        if(i==6*TILE_KEYBOARD || i==7*TILE_KEYBOARD || i==9*TILE_KEYBOARD)
	                h=TILE_KEYBOARD;
	        gfx_drawLine(color[4],x+i,y,x+i,y+HT_KEYBOARD-h);
	        i+=TILE_KEYBOARD;
	        h=0;	
	}
	
	//affichage de la sélection
	//sel 45 = espace= code char 32 / sel 46 = del
	if ( sel <= 44 && sel >=0){
	        position[0]=sel/10;//ligne
	        position[1]=sel%10;//colonne
	        gfx_drawRect(color[5],x+(position[1]*TILE_KEYBOARD),y+(position[0]*TILE_KEYBOARD),TILE_KEYBOARD+1,TILE_KEYBOARD+1);
	}
	if(sel==45 )
	        gfx_drawRect(color[5],x+(5*TILE_KEYBOARD),y+(4*TILE_KEYBOARD),3*TILE_KEYBOARD+1,TILE_KEYBOARD+1);
	if(sel==46)
	        gfx_drawRect(color[5],x+(8*TILE_KEYBOARD),y+(4*TILE_KEYBOARD),2*TILE_KEYBOARD+1,TILE_KEYBOARD+1);
}
/*************************
*  draw an info box      *
**************************/
void draw_infobox(char text1 [],char text2 []){
        int w,h;
        gfx_fillRect(color[1],X_INFOBOX,Y_INFOBOX,LCD_WIDTH-X_INFOBOX*2,X_INFOBOX);
        gfx_drawRect(color[0],X_INFOBOX,Y_INFOBOX,LCD_WIDTH-X_INFOBOX*2,X_INFOBOX);
        gfx_getStringSize(text1,&w,&h);
        gfx_putS(color[0],color[1],(LCD_WIDTH/2)-(w/2),Y_INFOBOX+h,text1);
        gfx_getStringSize(text2,&w,&h);
        gfx_putS(color[0],color[1],(LCD_WIDTH/2)-(w/2),Y_INFOBOX+h+h,text2);

}


/*********************
*   draw priority    *
**********************/
void draw_priority(int priority,int color1,int color2, int x,int y){
	switch (priority)
	{
		case 1:
			sprintf(tmpstring,"Priorite : basse");
		break;
		case 2:
			sprintf(tmpstring,"Priorite : moyenne");
		break;
		case 3:
			sprintf(tmpstring,"Priorite : haute");
		break;
		case 4:
			sprintf(tmpstring,"Priorite : urgent");
		break;
		default:
			sprintf(tmpstring,"Priorite : inconnu");
	}
	gfx_fillRect(color[1],x,y,150,8);
	gfx_putS(color1,color2,x,y,tmpstring);	
}
/*************************************
*   draw text in center of screen    *
**************************************/
void draw_text(char text[]){
	int w,h;
	int i=0,j=0,k=0;
        //taille d'une lettre
	gfx_getStringSize("a",&w,&h);
	while(i<= MAXTASKLENGTH && text[i] != '\0' && j < 11){
		if( ((k > 0) && (k%MAX_CHAR_PER_LINE == 0 )) || ((i > 0) && (text[i] == '-') )){
			j++;
			k=0;
		}
		gfx_putC(color[0],color[1],10+(k*w),BAR_HEIGHT+2+h+(j*h),text[i]);
		i++;
		k++;
	}
}
/***************************************
*   draw current task on the screen    *
****************************************/
void draw_task(){
	printf("drawing task %ld\n", cur_task);
	in_menu=TASK_MENU;
	sprintf(tmpstring,".: %s :.",liste[cur_task].task);
	draw_header(tmpstring);
	draw_buttons("Edit","Delete","Create");
	draw_text(liste[cur_task].todo);
	draw_priority(liste[cur_task].priority,color[0],color[1],12,LCD_HEIGTH-45);
	draw_advancement_bar(color[3],color[2],color[4],15,LCD_HEIGTH-32 ,LCD_WIDTH-(15*2), 12,liste[cur_task].advancement);
}

/*************************************************
*   draw main menu with current task selected    *
**************************************************/
void draw_main_menu(){
	int h,w,prevtask,nextask;
	
	printf("drawing menu\n");
	in_menu=MAIN_MENU;
	sprintf(tmpstring,".: Menu | %ld/%ld :.",(cur_task+1),tasks);
	draw_header(tmpstring);
	draw_buttons("Edit","Delete","Create");
	//tache affichée dans la premiere case
	if(cur_task != 0 && tasks > 1 ){
		prevtask=cur_task-1;	
	}
	else{
		prevtask=tasks-1;
	}
	gfx_drawRect(color[4],X_RECT_MENU,Y_RECT_MENU,LCD_WIDTH-(X_RECT_MENU*2),RECT_MENU_HEIGHT);
	gfx_getStringSize(liste[prevtask].task,&w,&h);
	gfx_putS(color[0],color[1],LCD_MIDDLE-(w/2),Y_RECT_MENU+2,liste[prevtask].task);
	//barre de pourcentage
	draw_advancement_bar(color[3],color[2],color[1],LCD_MIDDLE-((RECT_MENU_WIDTH-10)/2),Y_RECT_MENU+15 ,(RECT_MENU_WIDTH-10),4,liste[prevtask].advancement);
	//tache affichée dans la deuxieme case
	gfx_drawRect(color[5],X_RECT_MENU,Y_RECT_MENU+(8+RECT_MENU_HEIGHT),LCD_WIDTH-(X_RECT_MENU*2),RECT_MENU_HEIGHT);
	gfx_getStringSize(liste[cur_task].task,&w,&h);
	gfx_putS(color[0],color[1],LCD_MIDDLE-(w/2),Y_RECT_MENU+(8+RECT_MENU_HEIGHT)+2,liste[cur_task].task);
	//barre de pourcentage
	draw_advancement_bar(color[3],color[2],color[1],LCD_MIDDLE-((RECT_MENU_WIDTH-10)/2),Y_RECT_MENU+15+RECT_MENU_HEIGHT+8,(RECT_MENU_WIDTH-10),4,liste[cur_task].advancement);
	
	// tache affichée dans la 3eme case
	if( cur_task != tasks-1  && tasks > 1 ){
		nextask=cur_task+1;
	}
	else{
		nextask=0;
	}
	gfx_drawRect(color[4],X_RECT_MENU,Y_RECT_MENU+(8+RECT_MENU_HEIGHT)*2,LCD_WIDTH-(X_RECT_MENU*2),RECT_MENU_HEIGHT);
	gfx_getStringSize(liste[nextask].task,&w,&h);
	gfx_putS(color[0],color[1],LCD_MIDDLE-(w/2),2+(Y_RECT_MENU+(8+RECT_MENU_HEIGHT)*2),liste[nextask].task);
	//barre de pourcentage
	draw_advancement_bar(color[3],color[2],color[1],LCD_MIDDLE-((RECT_MENU_WIDTH-10)/2),Y_RECT_MENU+15+((RECT_MENU_HEIGHT+8)*2),(RECT_MENU_WIDTH-10),4,liste[nextask].advancement);
}


void draw_edit_field(){
        int len=strlen(tmpmod);
        int i=0,h,w;
        gfx_getStringSize("A",&w,&h);
        gfx_drawRect(color[0],20-2,BAR_HEIGHT+10-2,27*w+4,h+4);
        gfx_fillRect(color[1],20,BAR_HEIGHT+10,27*w,h);
        if(len>25){
                for (i=0; i<=25; i++) {
                	tmpstring[i]=tmpmod[len-25+i];
                }
        }
        else{
                sprintf(tmpstring,"%s",tmpmod);
        } 
        gfx_putS(color[0],color[1],20,BAR_HEIGHT+10,tmpstring);
}

void del_last_char(){
        int len=strlen(tmpmod);
        if(len>0)
                tmpmod[len-1]='\0';
}
/***************************************
*   draw the menu to edit taskheader or description  *
****************************************/
void draw_edit_text_menu(int modif){
        int evt,finish=0,face=2,sel=0,firstdisp=1,lettre=0;
        if(modif==HEADER_EDIT)
                sprintf(tmpstring,".: Edit task header :.");
        else if(modif == DESCRIPTION_EDIT)
                sprintf(tmpstring,".: Edit task description :.");
        draw_header(tmpstring);
        if (in_menu==CREATE_TASK) {
        	draw_buttons("Save & Next","Switch","del");
        }
        else{
                draw_buttons("Save & Quit","Switch","del");
        }
        
        draw_vkeyboard_letters(firstdisp,face);
        draw_vkeyboard(sel);
        firstdisp=0;
        if(modif==HEADER_EDIT){
                if ( in_menu == CREATE_TASK){
                        sprintf(tmpmod,"\0");
                }
                else{
                        sprintf(tmpmod,"%s",liste[cur_task].task);
                }
        }
        else if ( modif==DESCRIPTION_EDIT){
                if ( in_menu == CREATE_TASK){
                        sprintf(tmpmod,"\0");
                }
                else{
                        sprintf(tmpmod,"%s",liste[cur_task].todo);
                }
        }
        draw_edit_field();
        while(finish==0)
        {
                evt=evt_getStatus(evt_handler);
                switch(evt)
                {
                case BTN_F1:
                        if(modif==DESCRIPTION_EDIT){
                                if (in_menu==CREATE_TASK) {
                                	free(tmp.todo);
                                	tmp.todo=strdup(tmpmod);
                                }
                                else {
                                        free(liste[cur_task].todo);
                                        liste[cur_task].todo=strdup(tmpmod);
                                }
                        }
                        else if(modif==HEADER_EDIT){
                                if (in_menu==CREATE_TASK) {
                                	free(tmp.task);
                                	tmp.task=strdup(tmpmod);
                                }
                                else {
                                        free(liste[cur_task].task);
                                        liste[cur_task].task=strdup(tmpmod);
                                }
                        }
                        finish=1;
                break;           
                case BTN_F2:
                        if(face == 1 ){
                                face=2;
                        }
                        else if(face == 2 ){
                                face=1;
                        }
                        draw_vkeyboard_letters(firstdisp,face);
                break;     
                case BTN_OFF:
                case BTN_2:
                        finish=1;
                break;
                case BTN_F3:
                        del_last_char();
                        draw_edit_field();
                break;
                case BTN_RIGHT:
                        if(sel < 46)
                                sel++;
                        draw_vkeyboard(sel);
                break;
                case BTN_LEFT:
                        if(sel>0)
                                sel--;
                        draw_vkeyboard(sel);
                break;
                case BTN_DOWN:
                        if(sel<=34)
                                sel+=10;
                        else if(sel == 35 || sel == 36 || sel == 37)
                                sel=45;
                        else if(sel==38 || sel==39)
                                sel=46;  
                        draw_vkeyboard(sel);
                break;
                case BTN_UP:
                        if(sel == 45)
                                sel=36;
                        else if(sel == 46)
                                sel=38; 
                        else if(sel > 9)
                                sel-=10;                             
                        draw_vkeyboard(sel);
                break;
                case BTN_1:
                case BTN_ON:
                        if(face==1)
                                lettre=sel+33;
                        if(face==2)
                                lettre=sel+78;
                        if(sel == 45)
                                lettre=32;//espace
                        if(sel==46)
                                del_last_char();
                        if(modif==HEADER_EDIT){
                                if(strlen(tmpmod)<27)
                                        sprintf(tmpmod,"%s%c",tmpmod,lettre);
                        }
                        else if(modif==DESCRIPTION_EDIT){
                                if(strlen(tmpmod)<MAXTASKLENGTH-1)
                                        sprintf(tmpmod,"%s%c",tmpmod,lettre);
                        }
                        draw_edit_field();
                break;
                }
                
        }
}


/***************************************
*   draw the menu to edit advancement  *
****************************************/
void draw_edit_advancement_menu(){
        int evt,finish=0,tmpadvancement=50;
        if(in_menu == CREATE_TASK){
                tmpadvancement=50;
        }
        else{
                tmpadvancement=liste[cur_task].advancement;
        }
        sprintf(tmpstring,".: Edit advancement :.");
        draw_header(tmpstring);
        draw_buttons("Save & Quit","default","Quit");
        draw_advancement_bar(color[3],color[2],color[4],15,LCD_HEIGTH-32 ,LCD_WIDTH-(15*2), 12,tmpadvancement);
        draw_text("-press left to decrease by 1%-press right to increase by 1%-press down to decrease by 10%-press up to increase by 10%-press f2 to go back to default");
        while(finish==0)
        {
                evt=evt_getStatus(evt_handler);
                switch(evt)
                {
                case BTN_F1:
                        if(in_menu == CREATE_TASK){
                                tmp.advancement=tmpadvancement;
                        }
                        else{
                                liste[cur_task].advancement=tmpadvancement;
                        }
                        finish=1;
                break;  
                case BTN_F2:
                        if(in_menu == CREATE_TASK){
                                tmpadvancement=tmp.advancement;
                        }
                        else{
                                tmpadvancement=liste[cur_task].advancement;
                        }
                        draw_advancement_bar(color[3],color[2],color[4],15,LCD_HEIGTH-32 ,LCD_WIDTH-(15*2), 12,tmpadvancement);
                break;    
                case BTN_OFF:  
                case BTN_2:         
                case BTN_F3:
                        finish=1;
                break;
                case BTN_RIGHT :
                        if( tmpadvancement < 100) 
                                tmpadvancement++;
                        draw_advancement_bar(color[3],color[2],color[4],15,LCD_HEIGTH-32 ,LCD_WIDTH-(15*2), 12,tmpadvancement);
                break;
                case BTN_LEFT:
                        if( tmpadvancement > 0) 
                                tmpadvancement--;
                        draw_advancement_bar(color[3],color[2],color[4],15,LCD_HEIGTH-32 ,LCD_WIDTH-(15*2), 12,tmpadvancement);
                break;
                case BTN_UP: 
                        tmpadvancement+=10;
                        if (tmpadvancement > 100)
                                tmpadvancement=100;
                        draw_advancement_bar(color[3],color[2],color[4],15,LCD_HEIGTH-32 ,LCD_WIDTH-(15*2), 12,tmpadvancement);
                break;
                case BTN_DOWN:
                        tmpadvancement-=10;
                        if (tmpadvancement < 0)
                                tmpadvancement=0;
                        draw_advancement_bar(color[3],color[2],color[4],15,LCD_HEIGTH-32 ,LCD_WIDTH-(15*2), 12,tmpadvancement);
                break;
                }
        }
}

/***************************************
*   draw the menu to edit priority     *
****************************************/
void draw_edit_priority_menu(){
        int evt,finish=0,tmppriority=1;
        if(in_menu == CREATE_TASK){
                tmppriority=1;
        }
        else{
                tmppriority=liste[cur_task].priority;
        }
        sprintf(tmpstring,".: Edit priority :.");
        draw_header(tmpstring);
        draw_buttons("Save & Quit","","Quit");
        draw_priority(tmppriority,color[0],color[1],12,LCD_HEIGTH-45);
        draw_text("-press left to decrease priority-press right to increase priority-press down to set to low-press up to set to urgent-press f2 to go back to default");
        while(!finish)
        {
                evt=evt_getStatus(evt_handler);
                switch(evt)
                {
                case BTN_F1:
                        if(in_menu == CREATE_TASK){
                                tmp.priority=tmppriority;
                        }
                        else{
                                liste[cur_task].priority=tmppriority;
                        }
                        finish=1;
                break;         
                case BTN_F2:
                        if(in_menu == CREATE_TASK){
                                tmppriority=tmp.priority; 
                        }
                        else{
                                tmppriority=liste[cur_task].priority; 
                        }
                        draw_priority(tmppriority,color[0],color[1],12,LCD_HEIGTH-45);
                break;     
                case BTN_F3:
                case BTN_2:
                case BTN_OFF:
                        finish=1;
                break;
                case BTN_RIGHT:
                        if( tmppriority < 4 ) 
                                tmppriority++;
                        draw_priority(tmppriority,color[0],color[1],12,LCD_HEIGTH-45);
                break;
                case BTN_LEFT:
                        if( tmppriority > 1) 
                                tmppriority--;
                        draw_priority(tmppriority,color[0],color[1],12,LCD_HEIGTH-45);
                break;
                case BTN_UP:
                        tmppriority=4;
                        draw_priority(tmppriority,color[0],color[1],12,LCD_HEIGTH-45);
                break;
                case BTN_DOWN:
                        tmppriority=1;
                        draw_priority(tmppriority,color[0],color[1],12,LCD_HEIGTH-45);
                break;
                }
                
        }
}

void draw_create_menu(){
        in_menu=CREATE_TASK;
        tmp.task=strdup("\0");
        tmp.todo=strdup("\0");
        draw_edit_text_menu(HEADER_EDIT);
        draw_edit_text_menu(DESCRIPTION_EDIT);
        draw_edit_priority_menu();
        draw_edit_advancement_menu();
        msgBox_info("Saving task...");
        delete_or_save_task(3);
        dump_file();
        cur_task=0;
        in_menu=MAIN_MENU;
        draw_main_menu(cur_task);
}

/***************************************
*   draw edit menu for current_task    *
****************************************/
void draw_rect_edit_menu(int selected){
        int tmpcolor=0,i,w,h;
        sprintf(tmpstring,".: Edit task %ld :.", cur_task+1);
        draw_header(tmpstring);
        draw_buttons("Quit","Save file","Create");
        for (i=0; i<4; i++) {
        	if (i!=selected)
        	        tmpcolor=color[4];
        	else
        	        tmpcolor=color[5];
        	gfx_drawRect(tmpcolor,X_RECT_MENU,BAR_HEIGHT+15+((RECT_MENU_HEIGHT+8)*i),RECT_MENU_WIDTH,RECT_MENU_HEIGHT);
        }
        gfx_getStringSize("Edit task header",&w,&h);
	gfx_putS(color[0],color[1],LCD_MIDDLE-(w/2),BAR_HEIGHT+15+(RECT_MENU_HEIGHT/2)+((RECT_MENU_HEIGHT+8)*0)-(h/2),"Edit task header");
	gfx_getStringSize("Edit task description",&w,&h);
	gfx_putS(color[0],color[1],LCD_MIDDLE-(w/2),BAR_HEIGHT+15+(RECT_MENU_HEIGHT/2)+((RECT_MENU_HEIGHT+8)*1)-(h/2),"Edit task description");
	gfx_getStringSize("Edit task advancement",&w,&h);
	gfx_putS(color[0],color[1],LCD_MIDDLE-(w/2),BAR_HEIGHT+15+(RECT_MENU_HEIGHT/2)+((RECT_MENU_HEIGHT+8)*2)-(h/2),"Edit task advancement");
	gfx_getStringSize("Edit task priority",&w,&h);
	gfx_putS(color[0],color[1],LCD_MIDDLE-(w/2),BAR_HEIGHT+15+(RECT_MENU_HEIGHT/2)+((RECT_MENU_HEIGHT+8)*3)-(h/2),"Edit task priority");  
}

//affichage du menu d'édition
void draw_edit_menu(){
        int sel=0,quit_edit_menu=0,evt;
        in_menu = EDIT_MENU;
        
        draw_rect_edit_menu(sel);
        while(!quit_edit_menu)
        {
                evt=evt_getStatus(evt_handler);
                switch(evt)
                {
                case BTN_UP:
                        if(sel>0)
                                sel--;
                        draw_rect_edit_menu(sel);
                        break;
                case BTN_LEFT://prev task
                        if (cur_task !=0 ){
                                cur_task--;
                        }
                        else if ( cur_task == 0){
                                cur_task=tasks-1;
                        }
                        draw_rect_edit_menu(sel);
			break;
                case BTN_DOWN:
                        if (sel < 3)
                                sel++;
                        draw_rect_edit_menu(sel);
                        break;
                case BTN_RIGHT://next task
                        if (cur_task < tasks-1){
                                cur_task++;
                        }
                        else if ( cur_task == tasks-1){
                                cur_task=0;
                        }
                        draw_rect_edit_menu(sel);
                        break;
                case BTN_ON:
                case BTN_1: 
                        launch_sel_menu(sel);
                        //en ressortant on réaffiche le menu
                        draw_rect_edit_menu(sel);
                        break;
                case BTN_2:
                case BTN_OFF:
                case BTN_F1://quit
                        quit_edit_menu=1;
                        break;
                case BTN_F2://save task
                        delete_or_save_task(2);
                        break;
                case BTN_F3://create new task
                        break;
                }
        }
        
}
//on lance le menu d'édition voulu
void launch_sel_menu(int sel){
        switch(sel){
                case 0://edit task header
                        draw_edit_text_menu(HEADER_EDIT);
                break;
                case 1://edit task description
                        draw_edit_text_menu(DESCRIPTION_EDIT);
                break;
                case 2://edit task advancement
                        draw_edit_advancement_menu();
                break;
                case 3://edit task priority
                        draw_edit_priority_menu();
                break;
        }

}

/*************************
*  Main Event handler    *
**************************/

void evt_switcher(int evt_number){
	switch(evt_number)
		{
			case NEXT_TASK:
			if( in_menu == TASK_MENU || in_menu == MAIN_MENU){
				if (cur_task < tasks-1){
					cur_task++;
				}
				else if ( cur_task == tasks-1){
					cur_task=0;
				}
			}
				//affichage en fonction du choix
				if(in_menu==MAIN_MENU){
					draw_main_menu(cur_task);
				}
				else if( in_menu == TASK_MENU){
					draw_task(cur_task);
				}
			break;
			case PREV_TASK:
			if( in_menu == TASK_MENU || in_menu == MAIN_MENU){
				if (cur_task !=0 ){
					cur_task--;
				}
				else if ( cur_task == 0){
					cur_task=tasks-1;
				}
			}
				//affichage en fonction du choix
				if(in_menu==MAIN_MENU){
					draw_main_menu(cur_task);
				}
				else if( in_menu == TASK_MENU){
					draw_task(cur_task);
				}
			break;
			case BACKTOMENU:
				if (in_menu == TASK_MENU || in_menu == EDIT_MENU ){
					draw_main_menu(cur_task);
				}
			break;
			case SELTASK:
				if(in_menu==MAIN_MENU){
					draw_task(cur_task);
				}
				
			break;
			case EDIT_TASK://f1
			        if(in_menu != EDIT_MENU)
			                draw_edit_menu();//on lance le menu éditer
			        draw_main_menu();
			        
			break;
			case DEL_TASK://f2
			        if( tasks != 1){
			                really_delete_task();
			                
			        }
			        else{
			                msgBox_info("Can't delete last task !");
			                mdelay(1500);
			        }
			        draw_main_menu();
			break;
			case CREATE_TASK://f3
			      in_menu=CREATE_TASK;
			      draw_create_menu();
			break;
			
	}

}

void check_data_folder(){
	//todo
}

/********************************
*       theme related stuff     *
*********************************/

int create_theme_file(){
	CFG_DATA * themefile=NULL;
	themefile=cfg_newFile();
	if(themefile==NULL){
		return 1;
	}
	else{
		cfg_addDummyLine(themefile,"#default theme file for aotodo");
		cfg_addDummyLine(themefile,"#colors values are between 0 and 254");
		cfg_addDummyLine(themefile,"#just delete this file to go back to default values");
		cfg_writeInt(themefile,"header",COLOR_BLACK);
		cfg_writeInt(themefile,"background",COLOR_WHITE);
		cfg_writeInt(themefile,"advancement_background",COLOR_GREY);
		cfg_writeInt(themefile,"advancement_bar",COLOR_LIGHT_YELLOW);
		cfg_writeInt(themefile,"advancement_border",COLOR_LIGHT_BLUE);
		cfg_writeInt(themefile,"selected_task_border",COLOR_LIGHT_GREEN);
		cfg_writeInt(themefile,"intro",intro);
		cfg_addDummyLine(themefile,"");
	}
	cfg_writeFile(themefile,THEMEFILE);
	cfg_clear(themefile);
	return 0;	
}
/**************************************************
*       avoid error by verifying color values     *
***************************************************/
void verify_color_value(){
	int i;
	for (i=0; i<6; i++) {
		if(color[i]>254){
			color[i]=254;
		}
		else if (color[i]<0) {
			color[i]=0;
		}
	}
}

/************************************
*       changing bitmap colors      *
*************************************/
void change_bitmap_color(){
	int i,j;
        /*  il doit y avoir une fonction qui permet de remplacer une expression par une autre ( pas cherché , la flemme )							
        */
	for (i=0; i<14; i++) {
		for (j=0; j<14; j++) {
			if(arrondigauche[i][j]==00){
				arrondigauche[i][j]=color[0];
			}	
			else{
				arrondigauche[i][j]=color[1];
			}
			if(arrondidroit[i][j]==00){
				arrondidroit[i][j]=color[0];
			}	
			else{
				arrondidroit[i][j]=color[1];
			}
			if(arrondibasgauche[i][j]==00){
				arrondibasgauche[i][j]=color[0];
			}	
			else{
				arrondibasgauche[i][j]=color[1];
			}
			if(arrondibasdroit[i][j]==00){
				arrondibasdroit[i][j]=color[0];
			}	
			else{
				arrondibasdroit[i][j]=color[1];
			}	
		}
	}
}

int parse_theme_file(){
	char * name;
	char * value;
	CFG_DATA * themefile=NULL;
	themefile=cfg_readFile(THEMEFILE);
	//si l'ouverture échoue on suppose que c'est que le fichier de theme n'existe pas , alors on le crée
	if(themefile==NULL){
			create_theme_file();
	}
	else{
		while (cfg_nextItem(themefile,&name,&value)){
    			if(!strcmp(name,"header")){
        		   	 color[0]=atoi(value);
            		}
            		else if(!strcmp(name,"background")){
        		    	color[1]=atoi(value);
            		}
            		else if(!strcmp(name,"advancement_background")){
        		   	 color[2]=atoi(value);
            		}
            		else if(!strcmp(name,"advancement_bar")){
        		   	 color[3]=atoi(value);
            		}
            		else if(!strcmp(name,"advancement_border")){
        		   	 color[4]=atoi(value);
            		}
            		else if(!strcmp(name,"selected_task_border")){
        		   	 color[5]=atoi(value);
            		}
            		else if(!strcmp(name,"intro")){
        		   	 intro=atoi(value);
            		}
            	}
            	verify_color_value();
            	change_bitmap_color();
	}
	return 0;
}
/***********************************
*   tasklist related stuff         *
************************************/

/***********************************
*   verify if all tasks are ok     *
************************************/
void verify_tasks(){
	int i;
	for (i=0; i<tasks; i++) {
		if(strlen(liste[i].task) > 27 ){
			liste[i].task[27]='\0';
			printf("task %ld header truncated\n",i+1);
		}
		if(strlen(liste[i].todo) > MAXTASKLENGTH ){
			liste[i].task[MAXTASKLENGTH]='\0';
			printf("task %ld description truncated\n",i+1);
		}
		if(liste[i].priority > 4){
			liste[i].priority=4;
			printf("priority of task %ld out of bounds\n", i);
		}
		if( liste[i].priority < 1 ){
			liste[i].priority=1;
			printf("priority of task %ld out of bounds\n", i);
		}
		if(liste[i].advancement > 100){
			liste[i].advancement=100;
			printf("advancement of task %ld out of bounds\n", i);
		}
		if( liste[i].advancement < 0 ){
			liste[i].advancement=0;
			printf("advancement of task %ld out of bounds\n", i);
		}
	}
}


/********************************
*        ask for delete task    *
*********************************/
void really_delete_task(){
        int evt;
        int delete=1;
        in_menu = DEL_MENU;
        sprintf(tmpstring,"Delete task %ld ?",cur_task+1);
        draw_infobox(tmpstring,"f1 = Yes  f3 = No");
        while(delete)
        {
                evt=evt_getStatus(evt_handler);
                switch(evt)
                {
                case BTN_F1:
                        delete_or_save_task(1);
			dump_file();
			sprintf(tmpstring,"Task %ld delete !",cur_task+1);
			msgBox_info(tmpstring);
			mdelay(1400);
			cur_task=0;
			delete=0;
			draw_main_menu();
                break;                
                case BTN_F3:
                        msgBox_info("Delete cancelled");
			mdelay(1500);
                        draw_main_menu();
			delete=0;
                break;
                }
                
        }
}

/**************************************
* delete a task or save current tasks *
*    if op = 1 : delete current task  *
*    if op = 2 : save current tasks   *
*    if op = 3 : add temp task        *
***************************************/
int delete_or_save_task(int op){
        int i=0,j=1,action=-1;
        CFG_DATA * tasklist=NULL;
        tasklist=cfg_newFile();
        cfg_rewindItems(tasklist);
        if(tasklist==NULL){
                return 1;
        }
        if(op == 1)
                action = cur_task;//on veut supprimer la tache courante
        else if( op == 2 || op==3)
                action=-1;//comme ça on recopie toutes les taches
                
        while(i<tasks) {
                if(i != action){
                        sprintf(tmpstring,"#task %ld", j);
                        cfg_addDummyLine(tasklist,tmpstring);
                        sprintf(tmpstring,"task=%s",liste[i].task);
                        cfg_addDummyLine(tasklist,tmpstring);
                        sprintf(tmpstring,"todo=%s",liste[i].todo);
                        cfg_addDummyLine(tasklist,tmpstring);
                        sprintf(tmpstring,"priority=%ld",liste[i].priority);
                        cfg_addDummyLine(tasklist,tmpstring);
                        sprintf(tmpstring,"advancement=%ld",liste[i].advancement);
                        cfg_addDummyLine(tasklist,tmpstring);
                        cfg_addDummyLine(tasklist,"");
                        j++;
                }
                i++;
        }
        if(op==3){//on ajoute la tache temporaire
                sprintf(tmpstring,"#task %ld", j);
                cfg_addDummyLine(tasklist,tmpstring);
                sprintf(tmpstring,"task=%s",tmp.task);
                cfg_addDummyLine(tasklist,tmpstring);
                sprintf(tmpstring,"todo=%s",tmp.todo);
                cfg_addDummyLine(tasklist,tmpstring);
                sprintf(tmpstring,"priority=%ld",tmp.priority);
                cfg_addDummyLine(tasklist,tmpstring);
                sprintf(tmpstring,"advancement=%ld",tmp.advancement);
                cfg_addDummyLine(tasklist,tmpstring);
        }
        cfg_writeFile(tasklist,TODOFILE);
        cfg_clear(tasklist);
        if(op==1 || op ==3)//on libere la mémoire alloué a la liste car on reparse todo.aot apres avoir enlevé ou ajouté une tache
                free(liste);
        return 0;
}
	
/********************************
*        create tasklist        *
*********************************/
int create_tasklist(){
	free(liste);
	CFG_DATA * tasklist=NULL;
	tasklist=cfg_newFile();
	cfg_rewindItems(tasklist);
	if(tasklist==NULL){
		return 1;
	}
	else{
	cfg_addDummyLine(tasklist,"#task example");
	cfg_addDummyLine(tasklist,"task=create a tasklist ;)");
	cfg_addDummyLine(tasklist,"todo=your tasklist is malformed or doesn't exist . see the new created tasklist in /medios/data/aotodo/todo.aot to have further information about how to make a new taskfile");
	cfg_addDummyLine(tasklist,"priority=2");
	cfg_addDummyLine(tasklist,"advancement=20");
	cfg_addDummyLine(tasklist,"#you can put several task in this file juste follow the structure");
	cfg_addDummyLine(tasklist,"#in first put the task=short task description (don't put more than 25 char anyway it will be truncated)");
	cfg_addDummyLine(tasklist,"#then put todo=detailled description , put a -text -text2 to make a list");
	cfg_addDummyLine(tasklist,"#put priority=an integer from 1 to 4 . 1 is low , 2 is medium , 3 is high and 4 is very important");
	cfg_addDummyLine(tasklist,"#and then put advancement=your advancement for the task");
	cfg_writeFile(tasklist,TODOFILE);
	cfg_clear(tasklist);
	}
	return 0;
}

/*******************************************
*                  Dumpfile                *
*					   *
* return 1 if there is a problem       	   *
* return 0 if there is no problem          *
********************************************/

//analyse du fichier de tache et répartition des taches dans la liste de structure
int dump_file(){
	char * value;
	char * name;
	int i=0;
	tasks=0;
	CFG_DATA * tasklist;
	tasklist=cfg_readFile(TODOFILE);
	
	if(tasklist==NULL) 
		return 1;
   	
   	printf("[aotodo] parsing '%s'\n",TODOFILE);
   	//on compte le nombre de taches
   	cfg_rewindItems(tasklist);
   	//on compte le nombre de taches afin d'alloué la memoire nécessaire a toutes ces taches
   	while ( cfg_nextItem(tasklist,&name,&value) ){
   		if(!strcmp(name,"task")){
   			tasks++;
   		}
   	}
   	printf("%ld tasks in todolist\n", tasks);
    	if( tasks != 0){
    		liste=(tache *)malloc(tasks * sizeof(tache));
    		cfg_rewindItems(tasklist);
    		liste[i].task=NULL;
    		liste[i].todo=NULL;
    		liste[i].priority=0;
    		liste[i].advancement=0;
    		//on met chaque champs dans la structure nécessaire
    		while (cfg_nextItem(tasklist,&name,&value)){
    			if(!strcmp(name,"task")){
        		   	 liste[i].task=strdup(value);
            		}
            		else if(!strcmp(name,"todo")){
        		    	liste[i].todo=strdup(value);
            		}
            		else if(!strcmp(name,"priority")){
        		   	 liste[i].priority=atoi(value);
            		}
            		else if(!strcmp(name,"advancement")){
        		   	liste[i].advancement=atoi(value);
        		    	i++;
        		    	if(i!=tasks){
        		    	        liste[i].task=NULL;
                                        liste[i].todo=NULL;
                                        liste[i].priority=0;
                                        liste[i].advancement=0;
        		    	}
            		}
            	}
        }
        else{
        	return 1;
        }	
        cfg_clear(tasklist);
	return 0;
}

/*********************
*   init archs *
**********************/
void init_arch(){
        getResolution(&LCD_WIDTH,&LCD_HEIGTH);
	LCD_MIDDLE=LCD_WIDTH/2;
        X_KEYBOARD=LCD_MIDDLE-(WT_KEYBOARD/2);
        Y_KEYBOARD=LCD_HEIGTH-HT_KEYBOARD-BUTTON_HEIGHT-4;//-4 pour un petit espace entre les boutons et le clavier
        RECT_MENU_WIDTH=LCD_WIDTH-(X_RECT_MENU*2);
}

/*********************
*   intro function   *
**********************/
void draw_intro(){
        char introtext[]="Stupid intro without any sense :D";
        int length=strlen(introtext);
        int* coord=NULL;
        int* sens=NULL;
        int l=0,w,h,i,j,evt,stop=0;
        coord=(int*)malloc(length*sizeof(int));
        sens=(int*)malloc(length*sizeof(int));
        for (i=0; i<length; i++) {
        	coord[i]=40;
        	sens[i]=1;
        }
        gfx_getStringSize("A",&w,&h);
        gfx_fillRect(color[0],0,40,LCD_WIDTH,30);
                i=LCD_WIDTH;
                while( i>0-(length*w) && !stop){
                        evt=evt_getStatus(evt_handler);
                        switch(evt){
                                case BTN_LEFT:
                                case BTN_RIGHT:
                                case BTN_DOWN:
                                case BTN_UP:
                                case BTN_1:
                                case BTN_2:
                                case BTN_OFF:
                                case BTN_ON:
                                case BTN_F1:
                                case BTN_F2:               
                                case BTN_F3:
                                stop=1;
                                break;
                        }
                        if(i!=0 && i%w==0 && l < length-1)  l++;
                               
                        for (j=0; j<=l; j++) {
                                if( i+(j*w) > 0  && (i+(j*w)) < (LCD_WIDTH-w)){
                                        gfx_putC(COLOR_LIGHT_GREEN,COLOR_BLACK,i+(j*w),coord[j],introtext[j]);
                                }
                                if( ((i+((j+1)*w)) > 0)  && (i+((j+1)*w)) < LCD_WIDTH-w ){
                                        gfx_fillRect(COLOR_BLACK,i+((j+1)*w),0,w,LCD_HEIGTH);
                                }
                                if(coord[j]==70)
                                        sens[j]=-1;
                                else if(coord[j]==40)
                                        sens[j]=1;
                                coord[j]+=sens[j];     
                        }
                        
                        
                        
                        mdelay(12);
                        i--;
                }
        free(coord);
        free(sens);
}

/*********************
*   main function    *
**********************/
void app_main(int argc, char ** argv)
{
  	printf("\nStarting gtodo\n");
  	gfx_clearScreen(color[0]);
  	stop_app=0;
	int evt;
	init_arch();
	if(dump_file() == 1){
		create_tasklist();
		dump_file();
	}
	verify_tasks();
	parse_theme_file();
	evt_handler=evt_getHandler(BTN_CLASS);
	if(intro==1)
	        draw_intro();
	cur_task=0;
	draw_main_menu(cur_task);
	
		while( !stop_app)
		{
			evt=evt_getStatus(evt_handler);
				switch(evt)
				{
					case BTN_LEFT:
					case BTN_DOWN:
						evt_switcher(NEXT_TASK);
						break;
				        case BTN_UP:
					case BTN_RIGHT:
						evt_switcher(PREV_TASK);
						break;
					case BTN_ON:
					case BTN_1:
						evt_switcher(SELTASK);
						break;
					case BTN_2:
						evt_switcher(BACKTOMENU);
						break;
					case BTN_OFF:
					        if (in_menu == TASK_MENU || in_menu == EDIT_MENU ){
					                evt_switcher(BACKTOMENU);
				                }
				                else{
						        stop_app=1;
				                }
						break;
					case BTN_F1:
					        evt_switcher(EDIT_TASK);
						break;
					case BTN_F2:
						evt_switcher(DEL_TASK);
						break;
					case BTN_F3:
					        evt_switcher(CREATE_TASK);
						break;
				}
		}
	evt_freeHandler(evt_handler);
    	free(liste);
	printf("\nQuit gtodo\n");
	exit(1);
}
