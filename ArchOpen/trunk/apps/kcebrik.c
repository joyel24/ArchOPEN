#include 	<api.h>
#include 	<math.h>

#define BRIQUE(X,Y) (briques[(Y)*nbbriquesx+(X)])

//Si le jeu doit etre relanc�
int restart=1;
//Pour stocker la largeur et hauteur de l'�cran
int screenw,screenh;
//Pour r�cup�rer les �v�nements
int evt_handler;
//Existence des briques
static int *briques;
//dimensions des briques
static unsigned int widthbrique, heightbrique;
//Nb de briques en x et en y
static unsigned int nbbriquesx=10,nbbriquesy=3;
//Propri�t�s balle
int rayonballe=7;
int xballe,yballe;
int angle;

//Propri�t�s du pad
int padw,padh,padx,pady;
//Pas de d�placement du pad
int pas;

//tests
int tw,th;

int touche(int xbrique,int ybrique,int xba,int yba){
	xbrique*=widthbrique;
	ybrique*=heightbrique;
	//Arrive par le bas
	if((yba-rayonballe)<heightbrique+ybrique&&(yba-rayonballe)>ybrique+heightbrique/2&&xba<widthbrique+xbrique-1&&xba>=xbrique)
		return -2;
	//Arrive par le haut	
	if((yba+rayonballe)<ybrique+heightbrique/2&&(yba+rayonballe)>ybrique&&xba<widthbrique+xbrique-1&&xba>=xbrique)
		return 2;
	//Arrive par la droite
	if((xba-rayonballe)<xbrique+widthbrique&&(xba-rayonballe)>xbrique+widthbrique/2&&yba<ybrique+heightbrique-1&&yba>=ybrique)
		return -1;
	//Arrive par la gauche
	if((xba+rayonballe)<xbrique+widthbrique/2&&(xba+rayonballe)>xbrique&&yba<heightbrique+ybrique-1&&yba>=ybrique)
		return 1;
	return 0;
}

void dessinerballe(int x, int y, int dx, int dy){
	int xx,zone;
	zone=0;
	
	//effacer la balle pr�c�dente
	for(xx=(x)-rayonballe-4;xx<=(x)+rayonballe+4;xx++){
			
			if(xx<=x){
				zone=sqrt(rayonballe*rayonballe-(x-xx)*(x-xx));
			}else{
				zone=sqrt(rayonballe*rayonballe-(xx-x)*(xx-x));
			}
			gfx_drawLine(COLOR_BLACK,xx,y-zone-4,xx,y+zone+4);
			
			
	}
	//dessiner la nouvelle balle
	for(xx=(x+dx)-rayonballe;xx<=(x+dx)+rayonballe;xx++){
			if(xx<=x){
				zone=sqrt(rayonballe*rayonballe-(x-xx)*(x-xx));
			}else{
				zone=sqrt(rayonballe*rayonballe-(xx-x)*(xx-x));
			}
			//if(xx==x+dx){
			//	gfx_drawLine(COLOR_WHITE,xx,y+dy-zone+1,xx,y+dy+zone-1);
			//}else{
				gfx_drawLine(COLOR_WHITE,xx,y+dy-zone,xx,y+dy+zone);
			//}
	}
}


void dessinerbriques(){
	//Redessine les "briques"
	int i=0,j=0;
	for(i=0;i<nbbriquesx ;i++){
		for(j=0;j<nbbriquesy;j++){
			switch(BRIQUE(i,j)){
				case 1: gfx_fillRect(COLOR_GREEN,i*widthbrique,j*heightbrique,widthbrique,heightbrique);
					   gfx_drawRect(COLOR_WHITE,i*widthbrique,j*heightbrique,widthbrique,heightbrique);
					   gfx_putS(COLOR_WHITE,COLOR_GREEN,i*widthbrique+widthbrique/2,j*heightbrique+heightbrique/2,"1");
					   break;
				case 2: gfx_fillRect(COLOR_BLUE,i*widthbrique,j*heightbrique,widthbrique,heightbrique);
					   gfx_drawRect(COLOR_WHITE,i*widthbrique,j*heightbrique,widthbrique,heightbrique);
					   gfx_putS(COLOR_WHITE,COLOR_BLUE,i*widthbrique+widthbrique/2,j*heightbrique+heightbrique/2,"2");
					   break;	
				case 3: gfx_fillRect(COLOR_RED,i*widthbrique,j*heightbrique,widthbrique,heightbrique);
					   gfx_drawRect(COLOR_WHITE,i*widthbrique,j*heightbrique,widthbrique,heightbrique);
					   gfx_putS(COLOR_WHITE,COLOR_RED,i*widthbrique+widthbrique/2,j*heightbrique+heightbrique/2,"3");
					   break;
				default:gfx_fillRect(COLOR_BLACK,i*widthbrique,j*heightbrique,widthbrique,heightbrique);
					   break;
			}
		}
	}
}

void effacerbrique(int x,int y,int w,int h){
	gfx_fillRect(COLOR_BLACK,x*w,y*h,w,h);
}

int game_init(void)
{
	
	briques=(int*)malloc(nbbriquesx*nbbriquesy*sizeof(int));
	//we init the graphics
  	gfx_openGraphics();
  	//we clear the screen in black color ( other colors are defined in /include/sys_def/color*.h )
	gfx_clearScreen(COLOR_BLACK);
	//while we don't decide to stop our app
	
	
	
	//R�cup�ration des dimensions de l'�cran
	getResolution(&screenw,&screenh);
	//Dimensionnement des briques
	widthbrique=screenw/10;
	heightbrique = screenw/10;
	
	
	
	//Mise en place des "briques"
	int i=0,j=0;
	for(i=0;i<nbbriquesx ;i++){
		for(j=0;j<nbbriquesy;j++){
			BRIQUE(i,j)=3;
			gfx_fillRect(COLOR_RED,i*widthbrique,j*heightbrique,widthbrique,heightbrique);
			gfx_drawRect(COLOR_WHITE,i*widthbrique,j*heightbrique,widthbrique,heightbrique);
		}
	}
	
	//Position et dimensions du pad
	padw=screenw/5;
	padh=screenw/15;
	padx=screenw/2-padw/2;
	pady=screenh-padh;
	pas = 3;
	//Position balle
	xballe=screenw/2;
	yballe=screenh-padh*2-rayonballe;
	//Angle valeur absolue (4 valeurs possibles 0,1,2,3)
	angle=0;
	//Mise en place du pad
	gfx_fillRect(COLOR32_GREENYELLOW,padx,pady,padw,padh);
	

	dessinerballe(xballe,yballe,0,0);
	int evt;
	while(1)
	{
		evt=evt_getStatus(evt_handler);
		if(evt==NO_EVENT)
			continue;
		switch(evt)
		{
		case BTN_ON:
		case BTN_OFF:
		case BTN_LEFT:
		case BTN_RIGHT:
		case BTN_UP:
		case BTN_DOWN:
			return 1;
		}
	}
}


void eventHandlerLoop(void)
{
	
	
	int stop_app = 0;
	restart = 1;
  	//will be useful to stock the event
	int evt;
	//Quand sensy==1 (sensx) >0 la balle va vers l'origine y (x), inversement si sens==-1<0
	int sensy = 1,sensx=0;
	//Variable � -1 ou 1 selon le sens o� elle doit repartir en x � 0 si pas touch�
	int atouchefx;
	//Variable � -1 ou 1 selon le sens o� elle doit repartir si touche au moins une fois dans le cycle,  x � 0 si pas touch�
	int atouche;
	//Variables locales pour boucles for
	int a,b;
				
	int pasx=0,pasy=3;
	
	while( !stop_app )
	{
		atouchefx=0;
		atouche=0;
		
		
		// Si la balle ne risque pas de toucher les briques (3/4 de la hauteur de l'�cran) on v�rifie si elle touche le pad
		if(yballe>3*screenh/4){
			if(yballe+rayonballe>=pady&&xballe>=padx&&xballe<=padx+padw){
				sensy=1;
				if(xballe<=(padx+padw/7)){
					sensx=1;
					angle=3;
				}
				if(xballe>(padx+padw/7)&&xballe<=(padx+2*padw/7)){
					sensx=1;
					angle=2;
				}
				if(xballe>(padx+2*padw/7)&&xballe<=(padx+3*padw/7)){
					sensx=1;
					angle=1;
				}
				if(xballe>(padx+3*padw/7)&&xballe<=(padx+4*padw/7)){
					sensx=0;
					angle=0;
				}
				if(xballe>(padx+4*padw/7)&&xballe<=(padx+5*padw/7)){
					sensx=-1;
					angle=1;
				}
				if(xballe>(padx+5*padw/7)&&xballe<=(padx+6*padw/7)){
					sensx=-1;
					angle=2;
				}
				if(xballe>(padx+6*padw/7)&&xballe<=(padx+7*padw/7)){
					sensx=-1;
					angle=3;
				}
				
			}
		}else{
			for(a=0;a<nbbriquesx;a++){
				for(b=0;b<nbbriquesy;b++){
					if(BRIQUE(a,b)>0){
						atouchefx=touche(a,b,xballe,yballe);
						if(atouchefx!=0){
							BRIQUE(a,b)=BRIQUE(a,b)-1;
							atouche=atouchefx;
						}
					}
				}
			}
		}
		
		
		
		dessinerbriques();
		
		if(atouche!=0){
			switch(atouche){
				case -2:sensy=-1; break;
				case 2: sensy=1; break;
				case -1: if(sensx==0){ angle=1;} sensx=-1; break;
				case 1: if(sensx==0){ angle=1;} sensx=1; break;
			}
		}
		
		switch(angle){
			case 0: pasx=0;pasy=3;break;
			case 1: pasx=1;pasy=3;break;
			case 2: pasx=1;pasy=2;break;
			case 3: pasx=2;pasy=2;break;
			default:break;
		}
		
        	//permet de tout redessiner en m�me temps pour la balle
		int pastmpy=0;
		
		//Gestion de la balle en Y
		if(yballe-rayonballe>0){
			if(sensy>0){
				/*dessinerballe(xballe,yballe,0,-(pasy));
				yballe=yballe-pasy;*/
				pastmpy=-pasy;
			}
		}else{ sensy = -1; }
		
		if(yballe+rayonballe<screenh){
			if(!(sensy>0)&&sensy<0){
				/*dessinerballe(xballe,yballe,0,pasy);
				yballe=yballe+pasy;*/
				pastmpy=pasy;
			}
		}else{ stop_app=1;}
		
		//Gestion de la balle en X
		if(xballe+rayonballe<screenw){
			if(!(sensx>0)&&sensx<0){
				dessinerballe(xballe,yballe,pasx,pastmpy);
				xballe=xballe+pasx;
				yballe=yballe+pastmpy;
			}
		}else{ sensx=1;dessinerballe(xballe,yballe,-pasx,pastmpy);}
		
		if(xballe-rayonballe>0){
			if(sensx>0){
				dessinerballe(xballe,yballe,-(pasx),pastmpy);
				xballe=xballe-pasx;
				yballe=yballe+pastmpy;
			}
		}else{ sensx=-1;dessinerballe(xballe,yballe,pasx,pastmpy);}
		
		if(sensx==0){
			dessinerballe(xballe,yballe,0,pastmpy);
			yballe=yballe+pastmpy;	
		}

		//we get the current event
		evt=evt_getStatus(evt_handler);
		
		if (evt!=NO_EVENT){
			
			switch(evt)
			{
				case BTN_LEFT:
					gfx_fillRect(COLOR_BLACK,padx,pady,padw, padh);
					if(padx-pas>=0) padx-=pas;
					gfx_fillRect(COLOR32_GREENYELLOW,padx,pady,padw,padh);
					break;
				case BTN_DOWN:
					break;
				case BTN_RIGHT:
					gfx_fillRect(COLOR_BLACK,padx,pady,padw, padh);
					if(padx+pas<=screenw-padw) padx+=pas;
					gfx_fillRect(COLOR32_GREENYELLOW,padx,pady,padw,padh);
					break;
				case BTN_UP:
					break;
				case BTN_OFF:
					stop_app = 1;
					restart=0;
					break;
				default:
					continue;
					break;
			}
		}else{
			gfx_fillRect(COLOR32_GREENYELLOW,padx,pady,padw,padh);
			yield();
			}
	
	mdelay(40);
	
	}
}


/* here is the main med function */
void app_main(int argc, char ** argv){
   
  while(restart!=0){
		  
	game_init();
	//event handler on buttons
	evt_handler = evt_getHandler(BTN_CLASS);
	eventHandlerLoop();
   }
	//we free the handler on button
	evt_freeHandler(evt_handler);
	if(briques) free(briques);
}

