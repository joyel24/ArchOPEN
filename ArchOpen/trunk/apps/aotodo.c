

/**
TODO:
#define backgroundcolor
add scrollbar
add Optionmenu
**/

#include <api.h>
#include <sys_def/evt.h>
#include <sys_def/stddef.h>
#include <sys_def/colordef.h>
#include <sys_def/font.h>

#include "aoTodo.h"

/**Define Color**/
#define colorLOW		 COLOR_BLUE
#define colorNORMAL 	 COLOR_DARK_BLUE
#define colorHIGH 		 COLOR_RED

int evt_handler,evt;
bool editFix;

char a[50];
char b[100];

struct Tdata{
	/**Counterstart = 1**/
	int dataLengh;
	char topic[50];
	char description[100];
	int importance;
	int day;
	int month;
	int year;
	char dayName[15];
	
	/**1000 Items max**/
} data[1000],dummy[1000];			

struct Tdata data[1000],dummy[1000];

void SaveToFile(void) {

int f;

        f=open("aoTodo.dat",O_RDWR|O_CREAT);
        if(f>=0){
            write(f,data,sizeof(data));
			close(f);
		}

}

void OpenSaveFile(void) {

int f;

	f=open("aoTodo.dat"/**"/medios/apps/aoTodo.dat"**/,O_RDONLY);
	if (f>=0){
		read(f,data,sizeof(data));
		close(f);
	}
	else {	/**ON ERROR**/
		msgBox_show("Load","Error: Loading database!",MSGBOX_TYPE_OK,MSGBOX_ICON_EXCLAMATION,evt_handler);
		data[1].dataLengh=0;
	}
	
}
void SaveToArr(int dataLengh) {

    WIDGETMENU_ITEM mi;
    
    mi=(WIDGETMENU_ITEM)editMenu->widgetAt(editMenu,1);
    strcpy(data[dataLengh].topic,mi->caption);
    mi=(WIDGETMENU_ITEM)editMenu->widgetAt(editMenu,4);
    strcpy(data[dataLengh].description,mi->caption);

    data[dataLengh].day=editMenu->getTrackbar(editMenu,editMenu->indexFromCaption(editMenu,"Day"))->value;
    data[dataLengh].month=editMenu->getTrackbar(editMenu,editMenu->indexFromCaption(editMenu,"Month"))->value;
    data[dataLengh].year=2000 + 
            editMenu->getTrackbar(editMenu,editMenu->indexFromCaption(editMenu,"Year 20**"))->value;

    mi=(WIDGETMENU_ITEM)editMenu->widgetAt(editMenu,10);
    strcpy(data[dataLengh].dayName,mi->caption);

    data[dataLengh].importance=editMenu->getChooser(editMenu,editMenu->indexFromCaption(editMenu,"Priorite"))->index;

}

void displayItems(void) {

    int dataLengh;
    int i;
    char * s;

	dataLengh=data[1].dataLengh;
	
	for(i=1;i<=dataLengh;++i) {
	
		s=malloc(20);
		sprintf(s,"- %s: %s (%s: %d.%d.%d)",
                data[i].topic,data[i].description,data[i].dayName,data[i].day,data[i].month,data[i].year);
		
        ((TEXTMENU_ITEM)textMenu->widgetAt(textMenu,i-1))->caption=s;
		
		switch(data[i].importance) {
			case 0: {((MENU_ITEM)textMenu->widgetAt(textMenu,i-1))->foreColor=colorLOW;break;}
			case 1: {((MENU_ITEM)textMenu->widgetAt(textMenu,i-1))->foreColor=colorNORMAL;break;}
			case 2: {((MENU_ITEM)textMenu->widgetAt(textMenu,i-1))->foreColor=colorHIGH;break;}
		}
		
	}
	
	/**clear last**/
	((TEXTMENU_ITEM)textMenu->widgetAt(textMenu,dataLengh))->caption=" ";
	
	mainMenu->paint(mainMenu);
	mainMenu->setFocusedWidget(mainMenu,textMenu);
	mainMenu->focusedWidget->paint(mainMenu->focusedWidget);
	
	/**Paint Icons**/
	gfx_drawBitmap (&add_bitmap,7,(screen_height-23));
	gfx_drawBitmap (&delete_bitmap,2+5*2+((screen_width-20)/3),(screen_height-23));
	gfx_drawBitmap (&help_bitmap,2+5*3+((screen_width-20)/3)*2,(screen_height-23));
	
}

bool isleapyear(int year){

	if ((year % 400)==0) 
		return true;
	else if ((year % 100)==0)
		return false;
	else if ((year % 4)==0)
		return true;
		
	return false;

}

int getDaysInMonth(int uMonth, int uYear){
  
  int arrDaysInMonth[13] = {  0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

  if (uMonth == 2)
  {
    if (isleapyear(uYear))
      return 29;
    else
      return 28;
  }

  if ((uMonth >= 1) && (uMonth <= 12))
    return arrDaysInMonth[uMonth];
  else
  {
    return 0;
  }
} 

void handleeditMenu(bool load) {

int stop=0;int index;
	
	if (load==true){
        index=textMenu->indexOf(textMenu,textMenu->index);
		((WIDGETMENU_ITEM)editMenu->widgetAt(editMenu,1))->caption=data[index+1].topic;
		((WIDGETMENU_ITEM)editMenu->widgetAt(editMenu,4))->caption=data[index+1].description;
		editMenu->getTrackbar(editMenu,editMenu->indexFromCaption(editMenu,"Day"))->value =
                data[index+1].day;
		editMenu->getTrackbar(editMenu,editMenu->indexFromCaption(editMenu,"Month"))->value =
                data[index+1].month;
		editMenu->getTrackbar(editMenu,editMenu->indexFromCaption(editMenu,"Year 20**"))->value =
                data[index+1].year-2000;
		((WIDGETMENU_ITEM)editMenu->widgetAt(editMenu,10))->caption=data[index+1].dayName;
		editMenu->getChooser(editMenu,editMenu->indexFromCaption(editMenu,"Priorite"))->index =
                data[index+1].importance;
	}
	else {	/**default**/
		((WIDGETMENU_ITEM)editMenu->widgetAt(editMenu,1))->caption="...";
		((WIDGETMENU_ITEM)editMenu->widgetAt(editMenu,4))->caption="...";
		editMenu->getTrackbar(editMenu,editMenu->indexFromCaption(editMenu,"Day"))->value=1;
		editMenu->getTrackbar(editMenu,editMenu->indexFromCaption(editMenu,"Month"))->value=1;
		editMenu->getTrackbar(editMenu,editMenu->indexFromCaption(editMenu,"Year 20**"))->value=8;
		((WIDGETMENU_ITEM)editMenu->widgetAt(editMenu,10))->caption=" ";
		editMenu->getChooser(editMenu,editMenu->indexFromCaption(editMenu,"Priorite"))->index=1;
	}
	
	gfx_clearScreen(COLOR_GREY);
	editList->paint(editList);
	/**Icons**/
	gfx_drawBitmap (&uebernehmen_bitmap,7,(screen_height-23));
	gfx_drawBitmap (&abort_bitmap,2+5*2+((screen_width-20)/2),(screen_height-23));
	
	do{	
		evt=evt_getStatus(evt_handler);
		
		if (!evt) {
			continue;
			}
		else {
		
			editMenu->handleEvent(editMenu,evt);
	
			switch(evt) {
				case BTN_F1: 
				{
					if (getDaysInMonth(editMenu->getTrackbar(editMenu,editMenu->indexFromCaption(editMenu,"Month"))->value,
					editMenu->getTrackbar(editMenu,editMenu->indexFromCaption(editMenu,"Year 20**"))->value)>=
					editMenu->getTrackbar(editMenu,editMenu->indexFromCaption(editMenu,"Day"))->value)
					{
				        index=textMenu->indexOf(textMenu,textMenu->index);
						if (load==false) {
							data[1].dataLengh+=1;
							SaveToArr(data[1].dataLengh);
						} else {
							SaveToArr(index+1);
						}
						displayItems();
						stop=1;
						break;
					
					} else {
						if (msgBox_show("Error","Incorrect Date!"
						,MSGBOX_TYPE_OK,MSGBOX_ICON_NO_ICON,evt_handler)==MSGBOX_OK)
							{
								gfx_clearScreen(COLOR_GREY);
								editList->paint(editList);
								/**Icons**/
								gfx_drawBitmap (&uebernehmen_bitmap,7,(screen_height-23));
								gfx_drawBitmap (&abort_bitmap,2+5*2+((screen_width-20)/2),(screen_height-23));
								continue;
							}
					}
				}
				case BTN_F3: {stop=1;break;}
				default: break;
			}
		}
		
	}while (!stop);
	
	/**Paint**/
	mainMenu->paint(mainMenu);
	mainMenu->setFocusedWidget(mainMenu,textMenu);
	mainMenu->focusedWidget->paint(mainMenu->focusedWidget);
	
	/**Paint Icons**/
	gfx_drawBitmap (&add_bitmap,7,(screen_height-23));
	gfx_drawBitmap (&delete_bitmap,2+5*2+((screen_width-20)/3),(screen_height-23));
	gfx_drawBitmap (&help_bitmap,2+5*3+((screen_width-20)/3)*2,(screen_height-23));
}

void getDay(void) {

	int t,m,j;
	t=editMenu->getTrackbar(editMenu,editMenu->indexFromCaption(editMenu,"Day"))->value;
	m=editMenu->getTrackbar(editMenu,editMenu->indexFromCaption(editMenu,"Month"))->value;
	j=editMenu->getTrackbar(editMenu,editMenu->indexFromCaption(editMenu,"Year 20**"))->value;


	int m2;
	int j2;
	
	j2=j;
	
	if(m<=2) {
		m2=m+10;
		j2=j-1;
	}
	else m2=m-2;
	
	int c = j2/100;
	int y = j2%100;
 
	int h = (((26*m2-2)/10)+t+y+y/4+c/4-2*c)%7;
	
	if(h<0)
			h=h+7;
	
	switch(h) {
		
		case 0: {((WIDGETMENU_ITEM)editMenu->widgetAt(editMenu,10))->caption="Sunday";break;}
		case 1: {((WIDGETMENU_ITEM)editMenu->widgetAt(editMenu,10))->caption="Monday";break;}
		case 2: {((WIDGETMENU_ITEM)editMenu->widgetAt(editMenu,10))->caption="Tuesday";break;}
		case 3: {((WIDGETMENU_ITEM)editMenu->widgetAt(editMenu,10))->caption="Wednesday";break;}
		case 4: {((WIDGETMENU_ITEM)editMenu->widgetAt(editMenu,10))->caption="Thursday";break;}
		case 5: {((WIDGETMENU_ITEM)editMenu->widgetAt(editMenu,10))->caption="Friday";break;}
		case 6: {((WIDGETMENU_ITEM)editMenu->widgetAt(editMenu,10))->caption="Saturday";break;}
		default: {((WIDGETMENU_ITEM)editMenu->widgetAt(editMenu,10))->caption=" ";break;}
		
	}
	
	//editMenu->handleEvent(editMenu,EVT_REDRAW);
	((WIDGETMENU_ITEM)editMenu->widgetAt(editMenu,10))->paint(editMenu->widgetAt(editMenu,10));
}

void editMenu_onClick(WIDGETMENU m, WIDGETMENU_ITEM mi){
int index = m->indexOf(m,m->index);
int index2 = textMenu->indexOf(textMenu,textMenu->index);

if (index==1) 
{
	gfx_clearScreen(COLOR_GREY);
	
    strcpy(a,data[index2+1].topic);
	
	virtKbd(evt_handler,a);
	
	mi->caption=a;
	
	/**Draw Editmenu**/
	gfx_clearScreen(COLOR_GREY);
	editList->paint(editList);	
}

if  (index==4) {

	gfx_clearScreen(COLOR_GREY);
	
	strcpy(b,data[index2+1].description);
	
	virtKbd(evt_handler,b);
	
	mi->caption=b;
	
	/**Draw Editmenu**/
	gfx_clearScreen(COLOR_GREY);
	editList->paint(editList);
	
}


}

void handlehelpMenu(void) {

int stop=0;

	gfx_clearScreen(COLOR_GREY);
	helpList->paint(helpList);
	/**Icon**/
	gfx_drawBitmap (&home_bitmap,7,(screen_height-23));
	
	do{	
		evt=evt_getStatus(evt_handler);
		
		if (!evt) continue;
		switch(evt) {
			case BTN_ON: {stop=1;break;}
			case BTN_OFF: {stop=1;break;}
			case BTN_F1: {stop=1;break;}
			case BTN_F2: {stop=1;break;}
			case BTN_F3: {stop=1;break;}
			case BTN_LEFT: {stop=1;break;}
			case BTN_RIGHT: {stop=1;break;}
			case BTN_DOWN: {stop=1;break;}
			case BTN_UP: {stop=1;break;}
		}
		
	}while (!stop);
	
	stop=0;
	mainMenu->paint(mainMenu);
	
	/**Paint Icons**/
	gfx_drawBitmap (&add_bitmap,7,(screen_height-23));
	gfx_drawBitmap (&delete_bitmap,2+5*2+((screen_width-20)/3),(screen_height-23));
	gfx_drawBitmap (&help_bitmap,2+5*3+((screen_width-20)/3)*2,(screen_height-23));
}

void deleteItem(void) {

int i,a,dataLengh;
int index=textMenu->indexOf(textMenu,textMenu->index);
	a=index+1;
	dataLengh=data[1].dataLengh; /**whole lengh**/

	if (msgBox_show("Item delete?","Are you sure that you want to delete the selected item?",MSGBOX_TYPE_YESNO,MSGBOX_ICON_QUESTION,evt_handler)==MSGBOX_YES && dataLengh>0) {

			
			for(i=a;i<=dataLengh;++i) {
				/**one up**/
				strcpy(data[i].topic,data[i+1].topic);
				strcpy(data[i].description,data[i+1].description);
				data[i].day=data[i+1].day;
				data[i].month=data[i+1].month;
				data[i].year=data[i+1].year;
				strcpy(data[i].dayName,data[i+1].dayName);
				data[i].importance=data[i+1].importance;
			}
			
			/**Kill the last**/
			strcpy(data[dataLengh].topic," ");
			strcpy(data[dataLengh].description," ");
			data[dataLengh].day=0;
			data[dataLengh].month=0;
			data[dataLengh].year=0;
			strcpy(data[dataLengh].dayName," ");
			data[i].importance=1;
	
			data[1].dataLengh=data[1].dataLengh-1;
			
			/**select one over deleted**/
			if (index>=1) {
				textMenu->index=textMenu->index->prev;
				((WIDGET)textMenu->index->nxt)->focused=false;
				textMenu->index->focused=true;
			}
			else {
				textMenu->index=(MENU_ITEM)textMenu->firstWidget;
				((WIDGET)textMenu->index->nxt)->focused=false;
				textMenu->index->focused=true;
			}
	}		
	//else data[1].dataLengh=data[1].dataLengh;

	
}

void handletextMenu(void) {

int stop=0;
int index;
	
	/**Paint Icons**/
	gfx_drawBitmap (&add_bitmap,7,(screen_height-23));
	gfx_drawBitmap (&delete_bitmap,2+5*2+((screen_width-20)/3),(screen_height-23));
	gfx_drawBitmap (&help_bitmap,2+5*3+((screen_width-20)/3)*2,(screen_height-23));
	
	do{	
		evt=evt_getStatus(evt_handler);
		
		if (!evt) continue;
		index=textMenu->indexOf(textMenu,textMenu->index);
		switch(evt) {
			/**select**/		
			case BTN_DOWN: {
                
				if(index<data[1].dataLengh-1)
					textMenu->handleEvent(textMenu,evt);
				break;
			}
			case BTN_UP: {
				if (index>0)
					textMenu->handleEvent(textMenu,evt);
				break;
			}
			/**ADD-FKT**/
			case BTN_F1: {handleeditMenu(false);break;}
			/**DELETE**/
			case BTN_F2: {deleteItem();displayItems();break;}
			/**HELP**/
			case BTN_F3: {handlehelpMenu();break;}
			/**EDIT-FKT**/
			case BTN_LEFT: {handleeditMenu(true);break;}
			case BTN_RIGHT: {handleeditMenu(true);break;}
			/**EXIT**/
			case BTN_OFF: {stop=1;break;}
			
		}
		
	}while (!stop);
	
	
}

void app_main(int argc, char ** argv) {
	
	gfx_openGraphics();
	evt_handler = evt_getHandler(BTN_CLASS);
	
	/**Loading Screen**/
	gfx_fontSet(STD7X13);
	gfx_clearScreen(COLOR_GREY);
	gfx_putS(COLOR_BLACK,COLOR_GREY,10,10,"Loading items...");
	
	OpenSaveFile();
	
	/**Initialisieren**/
	init();
	
	/**Main-Loop**/
	handletextMenu();
	
	/**Saving Screen**/
	gfx_fontSet(STD7X13);
	gfx_clearScreen(COLOR_GREY);
	gfx_putS(COLOR_BLACK,COLOR_GREY,10,10,"Saving items...");
	
	SaveToFile();
	
	gfx_putS(COLOR_BLACK,COLOR_GREY,10,25,"Canceling...");
	
	//textMenu->destroy(textMenu);
	mainMenu->destroy(mainMenu);
	editList->destroy(editList);
	helpList->destroy(helpList);
	
	evt_freeHandler(evt_handler);
	gfx_closeGraphics();
	

	exit(0);


}
