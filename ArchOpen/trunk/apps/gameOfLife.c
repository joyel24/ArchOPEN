#include 	<api.h>
#include <sys_def/stddef.h>

/* here is the main med function */



void app_main(int argc, char ** argv){
	//our needed vars for width and height of the screen
	int w=0, h=0;
	
  	int stop_app = 0;
  	//will be useful to stock the event
	int evt;
	//event handler on buttons
	int evt_handler = evt_getHandler(BTN_CLASS);
	//little message to say we have started the app
  	printf("\nHi skeleton");
  	//we init the graphics
  	gfx_openGraphics();
  	//we clear the screen in black color ( other colors are defined in /include/sys_def/color*.h )
	//gfx_clearScreen(COLOR_BLACK);
	//while we don't decide to stop our app

		int i;
		int j;
		int pixelVal;
		int width = 480;
		int height = 272;
		int Tableau[width][height];
		int TableauDraw[width][height];
		int readPix;
		int count;
		int random;
		int randomLevier = 10;
		int n = 0;
		char stringN[30];
		char stringA[30];

	int countArroud(int xx, int yy){
		int result = 0;
		if (15 == TableauDraw[xx - 1][yy - 1])
		{
			result++;
		}
		if (15 == TableauDraw[xx][yy - 1])
		{
			result++;
		}
		if (15 == TableauDraw[xx + 1][yy - 1])
		{
			result++;
		}
		if (15 == TableauDraw[xx - 1][yy])
		{
			result++;
		}
		if (15 == TableauDraw[xx + 1][yy])
		{
			result++;
		}
		if (15 == TableauDraw[xx - 1][yy + 1])
		{
			result++;
		}
		if (15 == TableauDraw[xx][yy + 1])
		{
			result++;
		}
		if (15 == TableauDraw[xx + 1][yy + 1])
		{
			result++;
		}
		return result;
	}



		for (i = 0; i < width; i++)
		{
			for (j = 0; j < height; j++)
			{
					TableauDraw [i][j] = 3;
					gfx_drawPixel(COLOR_BLACK, i, j);
			}
		}

		while (true){
			gfx_clearScreen(COLOR_BLACK);
			sprintf(stringA,"countarround %04d",randomLevier);
			gfx_putS(COLOR_BLACK, COLOR_WHITE, 0, 250, stringA);
			evt=evt_getStatus(evt_handler);
			if (evt == BTN_UP)
			{
				randomLevier+=1;
			}
			if (evt == BTN_DOWN)
			{
				randomLevier-=1;
			}
			if (evt == BTN_ON)
			{
				break;
			}
		}
		

		for (i = 0; i < width; i++)
		{
			for (j = 0; j < height; j++)
			{
				random = rand()%randomLevier;
				if (random <= 1){
					TableauDraw [i][j] = 15;
					gfx_drawPixel(COLOR_WHITE, i, j);
				}
				else {
					TableauDraw [i][j] = 3;
					gfx_drawPixel(COLOR_BLACK, i, j);
				}
			}
		}
		//mdelay(700);


	while( !stop_app )
	{
		//we get the current event
		evt=evt_getStatus(evt_handler);

		
		/*
		gfx_drawPixel(COLOR_WHITE, 12, 11);
		gfx_drawPixel(COLOR_WHITE, 11, 12);
		gfx_drawPixel(COLOR_WHITE, 10, 12);
		*/

		if (!evt){


/*
			for (i = 0; i < width; i++)
			{
				for (j = 0; j < height; j++)
				{
					Tableau [i][j] = gfx_readPixel(i, j);
				}
			}

*/
			for (i = 0; i < width; i++)
			{
				for (j = 0; j < height; j++)
				{
					
					readPix = TableauDraw[i][j];

					if (readPix == 15)
					{
						count = countArroud(i, j);
						if (count == 2 || count == 3)
						{
							Tableau [i][j] = 15;
						}
						else {
							Tableau [i][j] = 3;
						}
					}

					if (readPix == 3)
					{
						count = countArroud(i, j);
						if (count == 3)
						{
							Tableau [i][j] = 15;
						}
						
					}
				}
			}

			gfx_clearScreen(COLOR_BLACK);
			//mdelay(2);

			for (i = 0; i < width; i++)
			{
				for (j = 0; j < height; j++)
				{
					pixelVal = Tableau [i][j];
					if (pixelVal == 15)
					{
						gfx_drawPixel(pixelVal, i, j);
					}
					TableauDraw [i][j] = pixelVal;
				}
			}
			n++;
			sprintf(stringN,"%04d",n);
			gfx_putS(COLOR_BLACK, COLOR_WHITE, 0, 5, stringN);
			//mdelay(500);


			/*
			gfx_drawPixel(COLOR_WHITE, 0, 0);
			gfx_drawPixel(COLOR_WHITE, 1, 1);
			gfx_drawPixel(COLOR_WHITE, 1, 2);
			gfx_drawPixel(COLOR_WHITE, 2, 1);
			gfx_drawPixel(COLOR_WHITE, 1, 3);
			*/

/*
			int countA;
			char stringA[30];
			countA = countArroud(10, 10);
			sprintf(stringA,"countarround %04d",countA);
			gfx_putS(COLOR_BLACK, COLOR_WHITE, 0, 250, stringA);
			udelay(1000);
*/
/*
			int i;
			int j;
			int x = 0;
			int y = 0;
			int count = 0;
			int readPix;
			char stringTestX[30];
			char stringTestY[30];
			int Tableau [200][200]; //0 black -- 1 white
			int TableauNow [200][200];

			for (i = 5; i < 200; i++)
			{
					x=i;
					sprintf(stringTestX,"%04d",x);
					gfx_putS(COLOR_BLACK, COLOR_WHITE, 0, 220, stringTestX);
					//mdelay(100);

				for (j = 5; j < 200; j++)
				{
					y=j;
					sprintf(stringTestY,"%04d",y);
					gfx_putS(COLOR_BLACK, COLOR_WHITE, 0, 230, stringTestY);

					//COLOR_WHITE=15 (gfx_readPixel)
					readPix = gfx_readPixel(x, y);
					//sprintf(stringTest,"%04d",readPix);
					//gfx_putS(COLOR_BLACK, COLOR_WHITE, 20, 50, stringTest);
					//mdelay(100);

					if (readPix == 15)
					{
						TableauNow [x][y] = 1;

						count = countArroud(x, y);
						if (count < 2 || count > 3)
						{
							Tableau [x][y] = 0;
						}
					}
				}

					if (readPix == 3)
					{
						TableauNow [x][y] = 0;

						count = countArroud(x, y);
						if (count == 3)
						{
							Tableau [x][y] = 1;
							//gfx_drawPixel(COLOR_WHITE, x, y);
						}
						
					}
			}

			gfx_clearScreen(COLOR_BLACK);

			for (i = 0; i <= 200 ; i++)
			{
				for (j = 0; j <= 200 ; j++)
				{
					if (TableauNow [i][j] == 1){
						gfx_drawPixel(COLOR_WHITE, i, j);
					}

				}
			}

			for (i = 0; i <= 200 ; i++)
			{
				for (j = 0; j <= 200 ; j++)
				{
					if (Tableau [i][j] == 1){
						gfx_drawPixel(COLOR_WHITE, i, j);
					}
					if (Tableau [i][j] == 0){
						gfx_drawPixel(COLOR_BLACK, i, j);
					}					

				}
			}
			
*/
			continue;
		}
		//if there is an event
		else{
			//we can make a different action per button
			switch(evt)
			{
				//and you can make your archos speaking to you :D
				case BTN_LEFT:
                     /*                   //we fill a rectangle with the previous width and height of the string , if it's the firstime widht and height will be zero du to their initialization
					gfx_fillRect(COLOR_BLACK,10,10,w, h);
                                        //then we put the string
					gfx_putS(COLOR_BLUE,COLOR_BLACK,10,10,"Archos : hey you pressed my left button !");
                                        //and we get it size to clear it at the next display of text
					gfx_getStringSize("Archos : hey you pressed my left button !",&w, &h);
					*/
					if(width >= 10){
						width = width-10;
					}
					break;
				case BTN_RIGHT:
                     /*                   //we fill a rectangle with the previous width and height of the string , if it's the firstime widht and height will be zero du to their initialization
					gfx_fillRect(COLOR_BLACK,10,10,w, h);
                                        //then we put the string
					gfx_putS(COLOR_BLUE,COLOR_BLACK,10,10,"Archos : hey you pressed my left button !");
                                        //and we get it size to clear it at the next display of text
					gfx_getStringSize("Archos : hey you pressed my left button !",&w, &h);
					*/
					if(width <= 470){
						width = width+10;
					}
					break;

				case BTN_UP:
				/*
					gfx_fillRect(COLOR_BLACK,10,10,w, h);
					gfx_putS(COLOR_BLUE,COLOR_BLACK,10,10,"Archos : hey you pressed my down button !");
					gfx_getStringSize("Archos : hey you pressed my down button !",&w, &h);
					*/
					if(height >= 10){
						height = height-10;
					}
					break;
				case BTN_DOWN:
				/*
					gfx_fillRect(COLOR_BLACK,10,10,w, h);
					gfx_putS(COLOR_BLUE,COLOR_BLACK,10,10,"Archos : hey you pressed my down button !");
					gfx_getStringSize("Archos : hey you pressed my down button !",&w, &h);
					*/
					if(height <= 262){
						height = height+10;
					}
					break;

				case BTN_1:
					gfx_fillRect(COLOR_BLACK,10,10,w, h);
					gfx_putS(COLOR_BLUE,COLOR_BLACK,10,10,"Archos : I will shutdown if you continue !");
					gfx_getStringSize("Archos : I will shutdown if you continue !",&w, &h);
					break;
				case BTN_2:
					gfx_fillRect(COLOR_BLACK,10,10,w, h);
					gfx_putS(COLOR_BLUE,COLOR_BLACK,10,10,"Archos : Let me alone please !");
					gfx_getStringSize("Archos : Let me alone please !",&w, &h);
					break;
				//if button off is press then we decide to stop our app
				case BTN_OFF:
					stop_app = 1;
					break;
				default:
					continue;
					break;
			}
		}
	}
	//we free the handler on button
	evt_freeHandler(evt_handler);
	
	printf("\nBye skeleton");
}
