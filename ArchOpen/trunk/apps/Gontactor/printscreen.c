//  Gontactor
//   apps for MediOS project
//   Copyright (c) 2006 by Captain H@dock (tanguycano@hotmail.com)
//
// All files in this archive are subject to the GNU General Public License.
// See the file COPYING in the source tree root for full license agreement.
// This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
// KIND, either express of implied.

//------------------------------------------------------------------------------
//  File : printscreen.c		                         Author : Captain H@dock
//  Last change : 13/12/2006                    Created : 11/12/2006
//
//  Save current screen in a file
//------------------------------------------------------------------------------
#include <api.h>

#include "printscreen.h"
//------------------------------------------------------------------------------
//  gui_printscreen function
//
//  Print screen in a bitmap file
//  gui_printscreen("/printscreen.bmp");
//
//  Input args : (1)
//      filename		=>  Bitmap file name (char*) (ie "/printscreen.bmp")
//
//  Output arg : (0)
//
//------------------------------------------------------------------------------
void gui_printscreen(char *filename)
{
	int LCD_w, LCD_h;
	int bm_file= -1;                    //File pointer
	int x,y,pix;

	char *buff=NULL;
	
	//Load screen res
	getResolution(&LCD_w,&LCD_h);


	printf("Printing screen...   ");
	bm_file = open(filename, O_WRONLY | O_CREAT | O_TRUNC);     //Open (create if needed) and truncate
	if (bm_file<=0)                      //open error
	{
		printf("Erreur d'ouverture du log.\n");
		return;
	}

	buff = (char *) malloc(9*sizeof(char));
	
	//Print headers infos (with arch res)

	*(buff+0) = 0x42;			//0x00
	*(buff+1) = 0x4D;
	*(buff+2) = 0xF6;
	*(buff+3) = 0xC5;
	*(buff+4) = 0x01;
	*(buff+5) = 0x00;
	*(buff+6) = 0x00;
	*(buff+7) = 0x00;
	write(bm_file, buff,8);           //Écriture

	*(buff+0) = 0x00;			//0x08
	*(buff+1) = 0x00;
	*(buff+2) = 0x36;
	*(buff+3) = 0x00;
	*(buff+4) = 0x00;
	*(buff+5) = 0x00;
	*(buff+6) = 0x28;
	*(buff+7) = 0x00;
	write(bm_file, buff,8);           //Écriture

	*(buff+0) = 0x00;			//0x10
	*(buff+1) = 0x00;
	*(buff+2) = LCD_w%0x100;	//Save Width low-word
	*(buff+3) = LCD_w/0x100;	//Save Width high-word
	*(buff+4) = 0x00;
	*(buff+5) = 0x00;
	*(buff+6) = LCD_h%0x100;	//Save Heigth low-word
	*(buff+7) = LCD_h/0x100;	//Save Heigth high-word

	//~ *(buff+0) = 0x00;			//0x10
	//~ *(buff+1) = 0x00;
	//~ *(buff+2) = 0xDC;
	//~ *(buff+3) = 0x00;
	//~ *(buff+4) = 0x00;
	//~ *(buff+5) = 0x00;
	//~ *(buff+6) = 0xB0;
	//~ *(buff+7) = 0x00;
	write(bm_file, buff,8);           //Écriture

	*(buff+0) = 0x00;			//0x18
	*(buff+1) = 0x00;
	*(buff+2) = 0x01;
	*(buff+3) = 0x00;
	*(buff+4) = 0x18;
	*(buff+5) = 0x00;
	*(buff+6) = 0x00;
	*(buff+7) = 0x00;
	write(bm_file, buff,8);           //Écriture

	*(buff+0) = 0x00;			//0x20
	*(buff+1) = 0x00;
	*(buff+2) = 0xC0;
	*(buff+3) = 0xC5;
	*(buff+4) = 0x01;
	*(buff+5) = 0x00;
	*(buff+6) = 0x12;
	*(buff+7) = 0x0B;
	write(bm_file, buff,8);           //Écriture

	*(buff+0) = 0x00;			//0x28
	*(buff+1) = 0x00;
	*(buff+2) = 0x12;
	*(buff+3) = 0x0B;
	*(buff+4) = 0x00;
	*(buff+5) = 0x00;
	*(buff+6) = 0x00;
	*(buff+7) = 0x00;
	write(bm_file, buff,8);           //Écriture

	*(buff+0) = 0x00;			//0x30
	*(buff+1) = 0x00;
	*(buff+2) = 0x00;
	*(buff+3) = 0x00;
	*(buff+4) = 0x00;
	*(buff+5) = 0x00;
	write(bm_file, buff,6);           //Écriture

	//print picture
	for(y=1;y<=LCD_h;y++)
	{
		for(x=1;x<=LCD_w;x++)
		{
			pix = gfx_readPixel(x-1,LCD_h-y);	//read each pixel form left bottom corner to top rigth corner
			
			pal_to_BGR(pix, buff);		//get BGR value from hudge switch :p
			
			write(bm_file, buff,3);           //write color
			
		}		
	}



	close(bm_file);                        //Close file
	bm_file=-1;

	printf("Done.\n");
	
	free(buff);				//free mem
	return;	
}


//------------------------------------------------------------------------------
//  pal_to_BGR function
//
//  Return Blue Green and Red hexa value for one of the palette colors
//
//  Input args : (2)
//      pix		=>  pix color id (int)
//      buff		=>  color hexa code string (char *)
//
//  Output arg : (0)
//
//------------------------------------------------------------------------------
void pal_to_BGR(int pix, char *buff)
{
	int BGR[3]={0x00,0x00,0x00};
	
//Computer generated switch (pal_to_switch )	
switch(pix)
{
case 0:
BGR[2]= 0x0;
BGR[1]= 0x0;
BGR[0]= 0x0;
break;

case 1:
BGR[2]= 0x0;
BGR[1]= 0x0;
BGR[0]= 0x80;
break;

case 2:
BGR[2]= 0x0;
BGR[1]= 0x80;
BGR[0]= 0x0;
break;

case 3:
BGR[2]= 0xFF;
BGR[1]= 0xFF;
BGR[0]= 0xFF;
break;

case 4:
BGR[2]= 0x80;
BGR[1]= 0x0;
BGR[0]= 0x0;
break;

case 5:
BGR[2]= 0x80;
BGR[1]= 0x0;
BGR[0]= 0x80;
break;

case 6:
BGR[2]= 0x80;
BGR[1]= 0x40;
BGR[0]= 0x0;
break;

case 7:
BGR[2]= 0xC0;
BGR[1]= 0xC0;
BGR[0]= 0xC0;
break;

case 8:
BGR[2]= 0x80;
BGR[1]= 0x80;
BGR[0]= 0x80;
break;

case 9:
BGR[2]= 0x0;
BGR[1]= 0x0;
BGR[0]= 0xFF;
break;

case 10:
BGR[2]= 0x0;
BGR[1]= 0xFF;
BGR[0]= 0x0;
break;

case 11:
BGR[2]= 0x0;
BGR[1]= 0xFF;
BGR[0]= 0xFF;
break;

case 12:
BGR[2]= 0xFF;
BGR[1]= 0x0;
BGR[0]= 0x0;
break;

case 13:
BGR[2]= 0xFF;
BGR[1]= 0x0;
BGR[0]= 0xFF;
break;

case 14:
BGR[2]= 0xFF;
BGR[1]= 0xFF;
BGR[0]= 0x0;
break;

case 15:
BGR[2]= 0xFF;
BGR[1]= 0xFF;
BGR[0]= 0xFF;
break;

case 16:
BGR[2]= 0x20;
BGR[1]= 0x20;
BGR[0]= 0x20;
break;

case 17:
BGR[2]= 0x80;
BGR[1]= 0x80;
BGR[0]= 0x0;
break;

case 18:
BGR[2]= 0xDF;
BGR[1]= 0xDF;
BGR[0]= 0xDF;
break;

case 19:
BGR[2]= 0xA0;
BGR[1]= 0xA0;
BGR[0]= 0xA0;
break;

case 20:
BGR[2]= 0xEA;
BGR[1]= 0xE6;
BGR[0]= 0xDD;
break;

case 21:
BGR[2]= 0xD5;
BGR[1]= 0xCC;
BGR[0]= 0xBB;
break;

case 22:
BGR[2]= 0xA2;
BGR[1]= 0x8D;
BGR[0]= 0x68;
break;

case 23:
BGR[2]= 0x0;
BGR[1]= 0x40;
BGR[0]= 0x80;
break;

case 24:
BGR[2]= 0x0;
BGR[1]= 0x80;
BGR[0]= 0xFF;
break;

case 25:
BGR[2]= 0x0;
BGR[1]= 0xFF;
BGR[0]= 0x80;
break;

case 26:
BGR[2]= 0x40;
BGR[1]= 0x0;
BGR[0]= 0x0;
break;

case 27:
BGR[2]= 0x40;
BGR[1]= 0x0;
BGR[0]= 0x40;
break;

case 28:
BGR[2]= 0x40;
BGR[1]= 0x0;
BGR[0]= 0x80;
break;

case 29:
BGR[2]= 0x40;
BGR[1]= 0x80;
BGR[0]= 0x80;
break;

case 30:
BGR[2]= 0x80;
BGR[1]= 0x0;
BGR[0]= 0x40;
break;

case 31:
BGR[2]= 0x80;
BGR[1]= 0x0;
BGR[0]= 0xFF;
break;

case 32:
BGR[2]= 0x80;
BGR[1]= 0x40;
BGR[0]= 0x40;
break;

case 33:
BGR[2]= 0x80;
BGR[1]= 0x80;
BGR[0]= 0x40;
break;

case 34:
BGR[2]= 0x80;
BGR[1]= 0x80;
BGR[0]= 0xC0;
break;

case 35:
BGR[2]= 0x80;
BGR[1]= 0x80;
BGR[0]= 0xFF;
break;

case 36:
BGR[2]= 0x80;
BGR[1]= 0xFF;
BGR[0]= 0x0;
break;

case 37:
BGR[2]= 0x80;
BGR[1]= 0xFF;
BGR[0]= 0xFF;
break;

case 38:
BGR[2]= 0xA4;
BGR[1]= 0xC8;
BGR[0]= 0xF0;
break;

case 39:
BGR[2]= 0xC0;
BGR[1]= 0xDC;
BGR[0]= 0xC0;
break;

case 40:
BGR[2]= 0xFF;
BGR[1]= 0x0;
BGR[0]= 0x80;
break;

case 41:
BGR[2]= 0xFF;
BGR[1]= 0x80;
BGR[0]= 0x0;
break;

case 42:
BGR[2]= 0xFF;
BGR[1]= 0x80;
BGR[0]= 0xC0;
break;

case 43:
BGR[2]= 0xFF;
BGR[1]= 0x80;
BGR[0]= 0xFF;
break;

case 44:
BGR[2]= 0xFF;
BGR[1]= 0x80;
BGR[0]= 0x80;
break;

case 45:
BGR[2]= 0xFF;
BGR[1]= 0xFF;
BGR[0]= 0x80;
break;

case 46:
BGR[2]= 0xFF;
BGR[1]= 0xFB;
BGR[0]= 0xF0;
break;

case 47:
BGR[2]= 0xFF;
BGR[1]= 0xFF;
BGR[0]= 0xE8;
break;

case 48:
BGR[2]= 0x0;
BGR[1]= 0x0;
BGR[0]= 0x33;
break;

case 49:
BGR[2]= 0x0;
BGR[1]= 0x0;
BGR[0]= 0x66;
break;

case 50:
BGR[2]= 0x0;
BGR[1]= 0x0;
BGR[0]= 0x99;
break;

case 51:
BGR[2]= 0x0;
BGR[1]= 0x0;
BGR[0]= 0xCC;
break;

case 52:
BGR[2]= 0x33;
BGR[1]= 0x0;
BGR[0]= 0x0;
break;

case 53:
BGR[2]= 0x33;
BGR[1]= 0x0;
BGR[0]= 0x33;
break;

case 54:
BGR[2]= 0x33;
BGR[1]= 0x0;
BGR[0]= 0x66;
break;

case 55:
BGR[2]= 0x33;
BGR[1]= 0x0;
BGR[0]= 0x99;
break;

case 56:
BGR[2]= 0x33;
BGR[1]= 0x0;
BGR[0]= 0xCC;
break;

case 57:
BGR[2]= 0x33;
BGR[1]= 0x0;
BGR[0]= 0xFF;
break;

case 58:
BGR[2]= 0x66;
BGR[1]= 0x0;
BGR[0]= 0x0;
break;

case 59:
BGR[2]= 0x66;
BGR[1]= 0x0;
BGR[0]= 0x33;
break;

case 60:
BGR[2]= 0x66;
BGR[1]= 0x0;
BGR[0]= 0x66;
break;

case 61:
BGR[2]= 0x66;
BGR[1]= 0x0;
BGR[0]= 0x99;
break;

case 62:
BGR[2]= 0x66;
BGR[1]= 0x0;
BGR[0]= 0xCC;
break;

case 63:
BGR[2]= 0x66;
BGR[1]= 0x0;
BGR[0]= 0xFF;
break;

case 64:
BGR[2]= 0x99;
BGR[1]= 0x0;
BGR[0]= 0x0;
break;

case 65:
BGR[2]= 0x99;
BGR[1]= 0x0;
BGR[0]= 0x33;
break;

case 66:
BGR[2]= 0x99;
BGR[1]= 0x0;
BGR[0]= 0x66;
break;

case 67:
BGR[2]= 0x99;
BGR[1]= 0x0;
BGR[0]= 0x99;
break;

case 68:
BGR[2]= 0x99;
BGR[1]= 0x0;
BGR[0]= 0xCC;
break;

case 69:
BGR[2]= 0x99;
BGR[1]= 0x0;
BGR[0]= 0xFF;
break;

case 70:
BGR[2]= 0xCC;
BGR[1]= 0x0;
BGR[0]= 0x0;
break;

case 71:
BGR[2]= 0xCC;
BGR[1]= 0x0;
BGR[0]= 0x33;
break;

case 72:
BGR[2]= 0xCC;
BGR[1]= 0x0;
BGR[0]= 0x66;
break;

case 73:
BGR[2]= 0xCC;
BGR[1]= 0x0;
BGR[0]= 0x99;
break;

case 74:
BGR[2]= 0xCC;
BGR[1]= 0x0;
BGR[0]= 0xCC;
break;

case 75:
BGR[2]= 0xCC;
BGR[1]= 0x0;
BGR[0]= 0xFF;
break;

case 76:
BGR[2]= 0xFF;
BGR[1]= 0x0;
BGR[0]= 0x33;
break;

case 77:
BGR[2]= 0xFF;
BGR[1]= 0x0;
BGR[0]= 0x66;
break;

case 78:
BGR[2]= 0xFF;
BGR[1]= 0x0;
BGR[0]= 0x99;
break;

case 79:
BGR[2]= 0xFF;
BGR[1]= 0x0;
BGR[0]= 0xCC;
break;

case 80:
BGR[2]= 0x0;
BGR[1]= 0x33;
BGR[0]= 0x0;
break;

case 81:
BGR[2]= 0x0;
BGR[1]= 0x33;
BGR[0]= 0x33;
break;

case 82:
BGR[2]= 0x0;
BGR[1]= 0x33;
BGR[0]= 0x66;
break;

case 83:
BGR[2]= 0x0;
BGR[1]= 0x33;
BGR[0]= 0x99;
break;

case 84:
BGR[2]= 0x0;
BGR[1]= 0x33;
BGR[0]= 0xCC;
break;

case 85:
BGR[2]= 0x0;
BGR[1]= 0x33;
BGR[0]= 0xFF;
break;

case 86:
BGR[2]= 0x33;
BGR[1]= 0x33;
BGR[0]= 0x0;
break;

case 87:
BGR[2]= 0x33;
BGR[1]= 0x33;
BGR[0]= 0x33;
break;

case 88:
BGR[2]= 0x33;
BGR[1]= 0x33;
BGR[0]= 0x66;
break;

case 89:
BGR[2]= 0x33;
BGR[1]= 0x33;
BGR[0]= 0x99;
break;

case 90:
BGR[2]= 0x33;
BGR[1]= 0x33;
BGR[0]= 0xCC;
break;

case 91:
BGR[2]= 0x33;
BGR[1]= 0x33;
BGR[0]= 0xFF;
break;

case 92:
BGR[2]= 0x66;
BGR[1]= 0x33;
BGR[0]= 0x0;
break;

case 93:
BGR[2]= 0x66;
BGR[1]= 0x33;
BGR[0]= 0x33;
break;

case 94:
BGR[2]= 0x66;
BGR[1]= 0x33;
BGR[0]= 0x66;
break;

case 95:
BGR[2]= 0x66;
BGR[1]= 0x33;
BGR[0]= 0x99;
break;

case 96:
BGR[2]= 0x66;
BGR[1]= 0x33;
BGR[0]= 0xCC;
break;

case 97:
BGR[2]= 0x66;
BGR[1]= 0x33;
BGR[0]= 0xFF;
break;

case 98:
BGR[2]= 0x99;
BGR[1]= 0x33;
BGR[0]= 0x0;
break;

case 99:
BGR[2]= 0x99;
BGR[1]= 0x33;
BGR[0]= 0x33;
break;

case 100:
BGR[2]= 0x99;
BGR[1]= 0x33;
BGR[0]= 0x66;
break;

case 101:
BGR[2]= 0x99;
BGR[1]= 0x33;
BGR[0]= 0x99;
break;

case 102:
BGR[2]= 0x99;
BGR[1]= 0x33;
BGR[0]= 0xCC;
break;

case 103:
BGR[2]= 0x99;
BGR[1]= 0x33;
BGR[0]= 0xFF;
break;

case 104:
BGR[2]= 0xCC;
BGR[1]= 0x33;
BGR[0]= 0x0;
break;

case 105:
BGR[2]= 0xCC;
BGR[1]= 0x33;
BGR[0]= 0x33;
break;

case 106:
BGR[2]= 0xCC;
BGR[1]= 0x33;
BGR[0]= 0x66;
break;

case 107:
BGR[2]= 0xCC;
BGR[1]= 0x33;
BGR[0]= 0x99;
break;

case 108:
BGR[2]= 0xCC;
BGR[1]= 0x33;
BGR[0]= 0xCC;
break;

case 109:
BGR[2]= 0xCC;
BGR[1]= 0x33;
BGR[0]= 0xFF;
break;

case 110:
BGR[2]= 0xFF;
BGR[1]= 0x33;
BGR[0]= 0x0;
break;

case 111:
BGR[2]= 0xFF;
BGR[1]= 0x33;
BGR[0]= 0x33;
break;

case 112:
BGR[2]= 0xFF;
BGR[1]= 0x33;
BGR[0]= 0x66;
break;

case 113:
BGR[2]= 0xFF;
BGR[1]= 0x33;
BGR[0]= 0x99;
break;

case 114:
BGR[2]= 0xFF;
BGR[1]= 0x33;
BGR[0]= 0xCC;
break;

case 115:
BGR[2]= 0xFF;
BGR[1]= 0x33;
BGR[0]= 0xFF;
break;

case 116:
BGR[2]= 0x0;
BGR[1]= 0x66;
BGR[0]= 0x0;
break;

case 117:
BGR[2]= 0x0;
BGR[1]= 0x66;
BGR[0]= 0x33;
break;

case 118:
BGR[2]= 0x0;
BGR[1]= 0x66;
BGR[0]= 0x66;
break;

case 119:
BGR[2]= 0x0;
BGR[1]= 0x66;
BGR[0]= 0x99;
break;

case 120:
BGR[2]= 0x0;
BGR[1]= 0x66;
BGR[0]= 0xCC;
break;

case 121:
BGR[2]= 0x0;
BGR[1]= 0x66;
BGR[0]= 0xFF;
break;

case 122:
BGR[2]= 0x33;
BGR[1]= 0x66;
BGR[0]= 0x0;
break;

case 123:
BGR[2]= 0x33;
BGR[1]= 0x66;
BGR[0]= 0x33;
break;

case 124:
BGR[2]= 0x33;
BGR[1]= 0x66;
BGR[0]= 0x66;
break;

case 125:
BGR[2]= 0x33;
BGR[1]= 0x66;
BGR[0]= 0x99;
break;

case 126:
BGR[2]= 0x33;
BGR[1]= 0x66;
BGR[0]= 0xCC;
break;

case 127:
BGR[2]= 0x33;
BGR[1]= 0x66;
BGR[0]= 0xFF;
break;

case 128:
BGR[2]= 0x66;
BGR[1]= 0x66;
BGR[0]= 0x0;
break;

case 129:
BGR[2]= 0x66;
BGR[1]= 0x66;
BGR[0]= 0x33;
break;

case 130:
BGR[2]= 0x66;
BGR[1]= 0x66;
BGR[0]= 0x66;
break;

case 131:
BGR[2]= 0x66;
BGR[1]= 0x66;
BGR[0]= 0x99;
break;

case 132:
BGR[2]= 0x66;
BGR[1]= 0x66;
BGR[0]= 0xCC;
break;

case 133:
BGR[2]= 0x66;
BGR[1]= 0x66;
BGR[0]= 0xFF;
break;

case 134:
BGR[2]= 0x99;
BGR[1]= 0x66;
BGR[0]= 0x0;
break;

case 135:
BGR[2]= 0x99;
BGR[1]= 0x66;
BGR[0]= 0x33;
break;

case 136:
BGR[2]= 0x99;
BGR[1]= 0x66;
BGR[0]= 0x66;
break;

case 137:
BGR[2]= 0x99;
BGR[1]= 0x66;
BGR[0]= 0x99;
break;

case 138:
BGR[2]= 0x99;
BGR[1]= 0x66;
BGR[0]= 0xCC;
break;

case 139:
BGR[2]= 0x99;
BGR[1]= 0x66;
BGR[0]= 0xFF;
break;

case 140:
BGR[2]= 0xCC;
BGR[1]= 0x66;
BGR[0]= 0x0;
break;

case 141:
BGR[2]= 0xCC;
BGR[1]= 0x66;
BGR[0]= 0x33;
break;

case 142:
BGR[2]= 0xCC;
BGR[1]= 0x66;
BGR[0]= 0x66;
break;

case 143:
BGR[2]= 0xCC;
BGR[1]= 0x66;
BGR[0]= 0x99;
break;

case 144:
BGR[2]= 0xCC;
BGR[1]= 0x66;
BGR[0]= 0xCC;
break;

case 145:
BGR[2]= 0xCC;
BGR[1]= 0x66;
BGR[0]= 0xFF;
break;

case 146:
BGR[2]= 0xFF;
BGR[1]= 0x66;
BGR[0]= 0x0;
break;

case 147:
BGR[2]= 0xFF;
BGR[1]= 0x66;
BGR[0]= 0x33;
break;

case 148:
BGR[2]= 0xFF;
BGR[1]= 0x66;
BGR[0]= 0x66;
break;

case 149:
BGR[2]= 0xFF;
BGR[1]= 0x66;
BGR[0]= 0x99;
break;

case 150:
BGR[2]= 0xFF;
BGR[1]= 0x66;
BGR[0]= 0xCC;
break;

case 151:
BGR[2]= 0xFF;
BGR[1]= 0x66;
BGR[0]= 0xFF;
break;

case 152:
BGR[2]= 0x0;
BGR[1]= 0x99;
BGR[0]= 0x0;
break;

case 153:
BGR[2]= 0x0;
BGR[1]= 0x99;
BGR[0]= 0x33;
break;

case 154:
BGR[2]= 0x0;
BGR[1]= 0x99;
BGR[0]= 0x66;
break;

case 155:
BGR[2]= 0x0;
BGR[1]= 0x99;
BGR[0]= 0x99;
break;

case 156:
BGR[2]= 0x0;
BGR[1]= 0x99;
BGR[0]= 0xCC;
break;

case 157:
BGR[2]= 0x0;
BGR[1]= 0x99;
BGR[0]= 0xFF;
break;

case 158:
BGR[2]= 0x33;
BGR[1]= 0x99;
BGR[0]= 0x0;
break;

case 159:
BGR[2]= 0x33;
BGR[1]= 0x99;
BGR[0]= 0x33;
break;

case 160:
BGR[2]= 0x33;
BGR[1]= 0x99;
BGR[0]= 0x66;
break;

case 161:
BGR[2]= 0x33;
BGR[1]= 0x99;
BGR[0]= 0x99;
break;

case 162:
BGR[2]= 0x33;
BGR[1]= 0x99;
BGR[0]= 0xCC;
break;

case 163:
BGR[2]= 0x33;
BGR[1]= 0x99;
BGR[0]= 0xFF;
break;

case 164:
BGR[2]= 0x66;
BGR[1]= 0x99;
BGR[0]= 0x0;
break;

case 165:
BGR[2]= 0x66;
BGR[1]= 0x99;
BGR[0]= 0x33;
break;

case 166:
BGR[2]= 0x66;
BGR[1]= 0x99;
BGR[0]= 0x66;
break;

case 167:
BGR[2]= 0x66;
BGR[1]= 0x99;
BGR[0]= 0x99;
break;

case 168:
BGR[2]= 0x66;
BGR[1]= 0x99;
BGR[0]= 0xCC;
break;

case 169:
BGR[2]= 0x66;
BGR[1]= 0x99;
BGR[0]= 0xFF;
break;

case 170:
BGR[2]= 0x99;
BGR[1]= 0x99;
BGR[0]= 0x0;
break;

case 171:
BGR[2]= 0x99;
BGR[1]= 0x99;
BGR[0]= 0x33;
break;

case 172:
BGR[2]= 0x99;
BGR[1]= 0x99;
BGR[0]= 0x66;
break;

case 173:
BGR[2]= 0x99;
BGR[1]= 0x99;
BGR[0]= 0x99;
break;

case 174:
BGR[2]= 0x99;
BGR[1]= 0x99;
BGR[0]= 0xCC;
break;

case 175:
BGR[2]= 0x99;
BGR[1]= 0x99;
BGR[0]= 0xFF;
break;

case 176:
BGR[2]= 0xCC;
BGR[1]= 0x99;
BGR[0]= 0x0;
break;

case 177:
BGR[2]= 0xCC;
BGR[1]= 0x99;
BGR[0]= 0x33;
break;

case 178:
BGR[2]= 0xCC;
BGR[1]= 0x99;
BGR[0]= 0x66;
break;

case 179:
BGR[2]= 0xCC;
BGR[1]= 0x99;
BGR[0]= 0x99;
break;

case 180:
BGR[2]= 0xCC;
BGR[1]= 0x99;
BGR[0]= 0xCC;
break;

case 181:
BGR[2]= 0xCC;
BGR[1]= 0x99;
BGR[0]= 0xFF;
break;

case 182:
BGR[2]= 0xFF;
BGR[1]= 0x99;
BGR[0]= 0x0;
break;

case 183:
BGR[2]= 0xFF;
BGR[1]= 0x99;
BGR[0]= 0x33;
break;

case 184:
BGR[2]= 0xFF;
BGR[1]= 0x99;
BGR[0]= 0x66;
break;

case 185:
BGR[2]= 0xFF;
BGR[1]= 0x99;
BGR[0]= 0x99;
break;

case 186:
BGR[2]= 0xFF;
BGR[1]= 0x99;
BGR[0]= 0xCC;
break;

case 187:
BGR[2]= 0xFF;
BGR[1]= 0x99;
BGR[0]= 0xFF;
break;

case 188:
BGR[2]= 0x0;
BGR[1]= 0xCC;
BGR[0]= 0x0;
break;

case 189:
BGR[2]= 0x0;
BGR[1]= 0xCC;
BGR[0]= 0x33;
break;

case 190:
BGR[2]= 0x0;
BGR[1]= 0xCC;
BGR[0]= 0x66;
break;

case 191:
BGR[2]= 0x0;
BGR[1]= 0xCC;
BGR[0]= 0x99;
break;

case 192:
BGR[2]= 0x0;
BGR[1]= 0xCC;
BGR[0]= 0xCC;
break;

case 193:
BGR[2]= 0x0;
BGR[1]= 0xCC;
BGR[0]= 0xFF;
break;

case 194:
BGR[2]= 0x33;
BGR[1]= 0xCC;
BGR[0]= 0x0;
break;

case 195:
BGR[2]= 0x33;
BGR[1]= 0xCC;
BGR[0]= 0x33;
break;

case 196:
BGR[2]= 0x33;
BGR[1]= 0xCC;
BGR[0]= 0x66;
break;

case 197:
BGR[2]= 0x33;
BGR[1]= 0xCC;
BGR[0]= 0x99;
break;

case 198:
BGR[2]= 0x33;
BGR[1]= 0xCC;
BGR[0]= 0xCC;
break;

case 199:
BGR[2]= 0x33;
BGR[1]= 0xCC;
BGR[0]= 0xFF;
break;

case 200:
BGR[2]= 0x66;
BGR[1]= 0xCC;
BGR[0]= 0x0;
break;

case 201:
BGR[2]= 0x66;
BGR[1]= 0xCC;
BGR[0]= 0x33;
break;

case 202:
BGR[2]= 0x66;
BGR[1]= 0xCC;
BGR[0]= 0x66;
break;

case 203:
BGR[2]= 0x66;
BGR[1]= 0xCC;
BGR[0]= 0x99;
break;

case 204:
BGR[2]= 0x66;
BGR[1]= 0xCC;
BGR[0]= 0xCC;
break;

case 205:
BGR[2]= 0x66;
BGR[1]= 0xCC;
BGR[0]= 0xFF;
break;

case 206:
BGR[2]= 0x99;
BGR[1]= 0xCC;
BGR[0]= 0x0;
break;

case 207:
BGR[2]= 0x99;
BGR[1]= 0xCC;
BGR[0]= 0x33;
break;

case 208:
BGR[2]= 0x99;
BGR[1]= 0xCC;
BGR[0]= 0x66;
break;

case 209:
BGR[2]= 0x99;
BGR[1]= 0xCC;
BGR[0]= 0x99;
break;

case 210:
BGR[2]= 0x99;
BGR[1]= 0xCC;
BGR[0]= 0xCC;
break;

case 211:
BGR[2]= 0x99;
BGR[1]= 0xCC;
BGR[0]= 0xFF;
break;

case 212:
BGR[2]= 0xCC;
BGR[1]= 0xCC;
BGR[0]= 0x0;
break;

case 213:
BGR[2]= 0xCC;
BGR[1]= 0xCC;
BGR[0]= 0x33;
break;

case 214:
BGR[2]= 0xCC;
BGR[1]= 0xCC;
BGR[0]= 0x66;
break;

case 215:
BGR[2]= 0xCC;
BGR[1]= 0xCC;
BGR[0]= 0x99;
break;

case 216:
BGR[2]= 0xCC;
BGR[1]= 0xCC;
BGR[0]= 0xCC;
break;

case 217:
BGR[2]= 0xCC;
BGR[1]= 0xCC;
BGR[0]= 0xFF;
break;

case 218:
BGR[2]= 0xFF;
BGR[1]= 0xCC;
BGR[0]= 0x0;
break;

case 219:
BGR[2]= 0xFF;
BGR[1]= 0xCC;
BGR[0]= 0x33;
break;

case 220:
BGR[2]= 0xFF;
BGR[1]= 0xCC;
BGR[0]= 0x66;
break;

case 221:
BGR[2]= 0xFF;
BGR[1]= 0xCC;
BGR[0]= 0x99;
break;

case 222:
BGR[2]= 0xFF;
BGR[1]= 0xCC;
BGR[0]= 0xCC;
break;

case 223:
BGR[2]= 0xFF;
BGR[1]= 0xCC;
BGR[0]= 0xFF;
break;

case 224:
BGR[2]= 0x0;
BGR[1]= 0xFF;
BGR[0]= 0x33;
break;

case 225:
BGR[2]= 0x0;
BGR[1]= 0xFF;
BGR[0]= 0x66;
break;

case 226:
BGR[2]= 0x0;
BGR[1]= 0xFF;
BGR[0]= 0x99;
break;

case 227:
BGR[2]= 0x0;
BGR[1]= 0xFF;
BGR[0]= 0xCC;
break;

case 228:
BGR[2]= 0x33;
BGR[1]= 0xFF;
BGR[0]= 0x0;
break;

case 229:
BGR[2]= 0x33;
BGR[1]= 0xFF;
BGR[0]= 0x33;
break;

case 230:
BGR[2]= 0x33;
BGR[1]= 0xFF;
BGR[0]= 0x66;
break;

case 231:
BGR[2]= 0x33;
BGR[1]= 0xFF;
BGR[0]= 0x99;
break;

case 232:
BGR[2]= 0x33;
BGR[1]= 0xFF;
BGR[0]= 0xCC;
break;

case 233:
BGR[2]= 0x33;
BGR[1]= 0xFF;
BGR[0]= 0xFF;
break;

case 234:
BGR[2]= 0x66;
BGR[1]= 0xFF;
BGR[0]= 0x0;
break;

case 235:
BGR[2]= 0x66;
BGR[1]= 0xFF;
BGR[0]= 0x33;
break;

case 236:
BGR[2]= 0x66;
BGR[1]= 0xFF;
BGR[0]= 0x66;
break;

case 237:
BGR[2]= 0x66;
BGR[1]= 0xFF;
BGR[0]= 0x99;
break;

case 238:
BGR[2]= 0x66;
BGR[1]= 0xFF;
BGR[0]= 0xCC;
break;

case 239:
BGR[2]= 0x66;
BGR[1]= 0xFF;
BGR[0]= 0xFF;
break;

case 240:
BGR[2]= 0x99;
BGR[1]= 0xFF;
BGR[0]= 0x0;
break;

case 241:
BGR[2]= 0x99;
BGR[1]= 0xFF;
BGR[0]= 0x33;
break;

case 242:
BGR[2]= 0x99;
BGR[1]= 0xFF;
BGR[0]= 0x66;
break;

case 243:
BGR[2]= 0x99;
BGR[1]= 0xFF;
BGR[0]= 0x99;
break;

case 244:
BGR[2]= 0x99;
BGR[1]= 0xFF;
BGR[0]= 0xCC;
break;

case 245:
BGR[2]= 0x99;
BGR[1]= 0xFF;
BGR[0]= 0xFF;
break;

case 246:
BGR[2]= 0xCC;
BGR[1]= 0xFF;
BGR[0]= 0x0;
break;

case 247:
BGR[2]= 0xCC;
BGR[1]= 0xFF;
BGR[0]= 0x33;
break;

case 248:
BGR[2]= 0xCC;
BGR[1]= 0xFF;
BGR[0]= 0x66;
break;

case 249:
BGR[2]= 0xCC;
BGR[1]= 0xFF;
BGR[0]= 0x99;
break;

case 250:
BGR[2]= 0xCC;
BGR[1]= 0xFF;
BGR[0]= 0xCC;
break;

case 251:
BGR[2]= 0xCC;
BGR[1]= 0xFF;
BGR[0]= 0xFF;
break;

case 252:
BGR[2]= 0xFF;
BGR[1]= 0xFF;
BGR[0]= 0x33;
break;

case 253:
BGR[2]= 0xFF;
BGR[1]= 0xFF;
BGR[0]= 0x66;
break;

case 254:
BGR[2]= 0xFF;
BGR[1]= 0xFF;
BGR[0]= 0x99;
break;

case 255:
BGR[2]= 0xFF;
BGR[1]= 0xFF;
BGR[0]= 0xCC;
break;

}
	*(buff+0) = BGR[0];
	*(buff+1) = BGR[1];
	*(buff+2) = BGR[2];

	return;
}

