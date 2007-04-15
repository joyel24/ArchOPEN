//  Gontactor
//   apps for MediOS project
//   Copyright (c) 2006 by Captain H@dock (tanguycano@hotmail.com)
//
// All files in this archive are subject to the GNU General Public License.
// See the file COPYING in the source tree root for full license agreement.
// This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
// KIND, either express of implied.

//------------------------------------------------------------------------------
//  File : bmp_load.c                  		Author : Captain H@dock
//  Last change : 30/12/2006            	Created : 30/12/2006
//
//  Function to load and print bmp from disk to screen
//------------------------------------------------------------------------------
#include <api.h>

#include "bmp_load.h"

#include "newout.h"
//------------------------------------------------------------------------------
//  print_pal_bmp32 function
//
//  Load and print a bmp32 only using colors fo the palette from a file to screen
//
//  Input args : (2)
//      filepath    =>      BMP file path (char *)
//      x           =>      Horizontal position (int)
//      y           =>      Vertical position (int)
//
//  Input arg : (1)
//          0   =   OK
//          -2  =   File openning error
//------------------------------------------------------------------------------

int print_pal_bmp32(char *filepath, int x, int y)
{
	char *out_buff=NULL;
	char *buff=NULL;
	int width=0, heigth=0;
	int cur_x,cur_y, pix;
	int bmp_file=-1;

    //Open file
	bmp_file = open(filepath, O_RDONLY);     //Open File
	if (bmp_file<0)                      //if openning fail
	{
		#ifdef LOG_ON
		out_buff = (char *) malloc(80 * sizeof(char));
		sprintf(out_buff,"Erreur d'ouverture (r) du fichier bmp (%s).\n",filepath);
		newout(out_buff);
		free(out_buff);
		#endif
		return -2;                          //Bad file
	}


    //Load BMP header
	buff = (char *) malloc(0x37*sizeof(char));    //mem alloc to save header (38 octets)
	read(bmp_file,buff,0x36);

    width = (int) *(buff+0x13);
    width*= 0x100;
    width += (int) *(buff+0x12);

    heigth = (int) *(buff+0x17);
    heigth*= 0x100;
    heigth += (int) *(buff+0x16);

    free(buff);
	#ifdef LOG_ON
    out_buff = (char *) malloc(80 * sizeof(char));
	sprintf(out_buff,"Fichier bmp \"%s\" : %dx%d px.\n",filepath, width, heigth);
	newout(out_buff);
	free(out_buff);
	#endif


    //Load and print each pix
    buff = (char *) malloc(3*sizeof(char));    //mem alloc for a pix (4 octets)
    for(cur_y=heigth-1;cur_y>=0;cur_y--)
    {
        for(cur_x=0;cur_x<width;cur_x++)
        {
		read(bmp_file,buff,3);
		pix = BGR_to_pal(buff);
		if(pix==-1)
		{
			#ifdef LOG_ON
			newout("Couleur n'appartenant pas �la palette.\n");
			#endif
			//~ pix=COLOR_RED;
		}
		else
		{
			gfx_drawPixel(pix,x+cur_x,y+cur_y);
		}
	}
    }

    #ifdef LOG_ON
    newout("Image charg� avec succ�.\n");
    #endif


    free(buff);
    close(bmp_file);

    return 0;
}


//------------------------------------------------------------------------------
//  BGR_to_pal function
//
//  Return pix color id in the pal from Blue Green and Red hexa value.
//
//  Input args : (1)
//      buff		=>  color hexa code string (char *)
//
//  Output arg : (1)
//      pix		=>  pix color id (int)
//
//------------------------------------------------------------------------------
int BGR_to_pal(char *buff)
{
    if((*(buff+0)==0x00) && (*(buff+1)==0x00) && (*(buff+2)==0x00)) return 0;
    if((*(buff+0)==0x80) && (*(buff+1)==0x00) && (*(buff+2)==0x00)) return 1;
    if((*(buff+0)==0x00) && (*(buff+1)==0x80) && (*(buff+2)==0x00)) return 2;
    if((*(buff+0)==0xFF) && (*(buff+1)==0xFF) && (*(buff+2)==0xFF)) return 3;
    if((*(buff+0)==0x00) && (*(buff+1)==0x00) && (*(buff+2)==0x80)) return 4;
    if((*(buff+0)==0x80) && (*(buff+1)==0x00) && (*(buff+2)==0x80)) return 5;
    if((*(buff+0)==0x00) && (*(buff+1)==0x40) && (*(buff+2)==0x80)) return 6;
    if((*(buff+0)==0xC0) && (*(buff+1)==0xC0) && (*(buff+2)==0xC0)) return 7;
    if((*(buff+0)==0x80) && (*(buff+1)==0x80) && (*(buff+2)==0x80)) return 8;
    if((*(buff+0)==0xFF) && (*(buff+1)==0x00) && (*(buff+2)==0x00)) return 9;
    if((*(buff+0)==0x00) && (*(buff+1)==0xFF) && (*(buff+2)==0x00)) return 10;
    if((*(buff+0)==0xFF) && (*(buff+1)==0xFF) && (*(buff+2)==0x00)) return 11;
    if((*(buff+0)==0x00) && (*(buff+1)==0x00) && (*(buff+2)==0xFF)) return 12;
    if((*(buff+0)==0xFF) && (*(buff+1)==0x00) && (*(buff+2)==0xFF)) return 13;
    if((*(buff+0)==0x00) && (*(buff+1)==0xFF) && (*(buff+2)==0xFF)) return 14;
    if((*(buff+0)==0xFF) && (*(buff+1)==0xFF) && (*(buff+2)==0xFF)) return 15;
    if((*(buff+0)==0x20) && (*(buff+1)==0x20) && (*(buff+2)==0x20)) return 16;
    if((*(buff+0)==0x00) && (*(buff+1)==0x80) && (*(buff+2)==0x80)) return 17;
    if((*(buff+0)==0xDF) && (*(buff+1)==0xDF) && (*(buff+2)==0xDF)) return 18;
    if((*(buff+0)==0xA0) && (*(buff+1)==0xA0) && (*(buff+2)==0xA0)) return 19;
    if((*(buff+0)==0xDD) && (*(buff+1)==0xE6) && (*(buff+2)==0xEA)) return 20;
    if((*(buff+0)==0xBB) && (*(buff+1)==0xCC) && (*(buff+2)==0xD5)) return 21;
    if((*(buff+0)==0x68) && (*(buff+1)==0x8D) && (*(buff+2)==0xA2)) return 22;
    if((*(buff+0)==0x80) && (*(buff+1)==0x40) && (*(buff+2)==0x00)) return 23;
    if((*(buff+0)==0xFF) && (*(buff+1)==0x80) && (*(buff+2)==0x00)) return 24;
    if((*(buff+0)==0x80) && (*(buff+1)==0xFF) && (*(buff+2)==0x00)) return 25;
    if((*(buff+0)==0x00) && (*(buff+1)==0x00) && (*(buff+2)==0x40)) return 26;
    if((*(buff+0)==0x40) && (*(buff+1)==0x00) && (*(buff+2)==0x40)) return 27;
    if((*(buff+0)==0x80) && (*(buff+1)==0x00) && (*(buff+2)==0x40)) return 28;
    if((*(buff+0)==0x80) && (*(buff+1)==0x80) && (*(buff+2)==0x40)) return 29;
    if((*(buff+0)==0x40) && (*(buff+1)==0x00) && (*(buff+2)==0x80)) return 30;
    if((*(buff+0)==0xFF) && (*(buff+1)==0x00) && (*(buff+2)==0x80)) return 31;
    if((*(buff+0)==0x40) && (*(buff+1)==0x40) && (*(buff+2)==0x80)) return 32;
    if((*(buff+0)==0x40) && (*(buff+1)==0x80) && (*(buff+2)==0x80)) return 33;
    if((*(buff+0)==0xC0) && (*(buff+1)==0x80) && (*(buff+2)==0x80)) return 34;
    if((*(buff+0)==0xFF) && (*(buff+1)==0x80) && (*(buff+2)==0x80)) return 35;
    if((*(buff+0)==0x00) && (*(buff+1)==0xFF) && (*(buff+2)==0x80)) return 36;
    if((*(buff+0)==0xFF) && (*(buff+1)==0xFF) && (*(buff+2)==0x80)) return 37;
    if((*(buff+0)==0xF0) && (*(buff+1)==0xC8) && (*(buff+2)==0xA4)) return 38;
    if((*(buff+0)==0xC0) && (*(buff+1)==0xDC) && (*(buff+2)==0xC0)) return 39;
    if((*(buff+0)==0x80) && (*(buff+1)==0x00) && (*(buff+2)==0xFF)) return 40;
    if((*(buff+0)==0x00) && (*(buff+1)==0x80) && (*(buff+2)==0xFF)) return 41;
    if((*(buff+0)==0xC0) && (*(buff+1)==0x80) && (*(buff+2)==0xFF)) return 42;
    if((*(buff+0)==0xFF) && (*(buff+1)==0x80) && (*(buff+2)==0xFF)) return 43;
    if((*(buff+0)==0x80) && (*(buff+1)==0x80) && (*(buff+2)==0xFF)) return 44;
    if((*(buff+0)==0x80) && (*(buff+1)==0xFF) && (*(buff+2)==0xFF)) return 45;
    if((*(buff+0)==0xF0) && (*(buff+1)==0xFB) && (*(buff+2)==0xFF)) return 46;
    if((*(buff+0)==0xE8) && (*(buff+1)==0xFF) && (*(buff+2)==0xFF)) return 47;
    if((*(buff+0)==0x33) && (*(buff+1)==0x00) && (*(buff+2)==0x00)) return 48;
    if((*(buff+0)==0x66) && (*(buff+1)==0x00) && (*(buff+2)==0x00)) return 49;
    if((*(buff+0)==0x99) && (*(buff+1)==0x00) && (*(buff+2)==0x00)) return 50;
    if((*(buff+0)==0xCC) && (*(buff+1)==0x00) && (*(buff+2)==0x00)) return 51;
    if((*(buff+0)==0x00) && (*(buff+1)==0x00) && (*(buff+2)==0x33)) return 52;
    if((*(buff+0)==0x33) && (*(buff+1)==0x00) && (*(buff+2)==0x33)) return 53;
    if((*(buff+0)==0x66) && (*(buff+1)==0x00) && (*(buff+2)==0x33)) return 54;
    if((*(buff+0)==0x99) && (*(buff+1)==0x00) && (*(buff+2)==0x33)) return 55;
    if((*(buff+0)==0xCC) && (*(buff+1)==0x00) && (*(buff+2)==0x33)) return 56;
    if((*(buff+0)==0xFF) && (*(buff+1)==0x00) && (*(buff+2)==0x33)) return 57;
    if((*(buff+0)==0x00) && (*(buff+1)==0x00) && (*(buff+2)==0x66)) return 58;
    if((*(buff+0)==0x33) && (*(buff+1)==0x00) && (*(buff+2)==0x66)) return 59;
    if((*(buff+0)==0x66) && (*(buff+1)==0x00) && (*(buff+2)==0x66)) return 60;
    if((*(buff+0)==0x99) && (*(buff+1)==0x00) && (*(buff+2)==0x66)) return 61;
    if((*(buff+0)==0xCC) && (*(buff+1)==0x00) && (*(buff+2)==0x66)) return 62;
    if((*(buff+0)==0xFF) && (*(buff+1)==0x00) && (*(buff+2)==0x66)) return 63;
    if((*(buff+0)==0x00) && (*(buff+1)==0x00) && (*(buff+2)==0x99)) return 64;
    if((*(buff+0)==0x33) && (*(buff+1)==0x00) && (*(buff+2)==0x99)) return 65;
    if((*(buff+0)==0x66) && (*(buff+1)==0x00) && (*(buff+2)==0x99)) return 66;
    if((*(buff+0)==0x99) && (*(buff+1)==0x00) && (*(buff+2)==0x99)) return 67;
    if((*(buff+0)==0xCC) && (*(buff+1)==0x00) && (*(buff+2)==0x99)) return 68;
    if((*(buff+0)==0xFF) && (*(buff+1)==0x00) && (*(buff+2)==0x99)) return 69;
    if((*(buff+0)==0x00) && (*(buff+1)==0x00) && (*(buff+2)==0xCC)) return 70;
    if((*(buff+0)==0x33) && (*(buff+1)==0x00) && (*(buff+2)==0xCC)) return 71;
    if((*(buff+0)==0x66) && (*(buff+1)==0x00) && (*(buff+2)==0xCC)) return 72;
    if((*(buff+0)==0x99) && (*(buff+1)==0x00) && (*(buff+2)==0xCC)) return 73;
    if((*(buff+0)==0xCC) && (*(buff+1)==0x00) && (*(buff+2)==0xCC)) return 74;
    if((*(buff+0)==0xFF) && (*(buff+1)==0x00) && (*(buff+2)==0xCC)) return 75;
    if((*(buff+0)==0x33) && (*(buff+1)==0x00) && (*(buff+2)==0xFF)) return 76;
    if((*(buff+0)==0x66) && (*(buff+1)==0x00) && (*(buff+2)==0xFF)) return 77;
    if((*(buff+0)==0x99) && (*(buff+1)==0x00) && (*(buff+2)==0xFF)) return 78;
    if((*(buff+0)==0xCC) && (*(buff+1)==0x00) && (*(buff+2)==0xFF)) return 79;
    if((*(buff+0)==0x00) && (*(buff+1)==0x33) && (*(buff+2)==0x00)) return 80;
    if((*(buff+0)==0x33) && (*(buff+1)==0x33) && (*(buff+2)==0x00)) return 81;
    if((*(buff+0)==0x66) && (*(buff+1)==0x33) && (*(buff+2)==0x00)) return 82;
    if((*(buff+0)==0x99) && (*(buff+1)==0x33) && (*(buff+2)==0x00)) return 83;
    if((*(buff+0)==0xCC) && (*(buff+1)==0x33) && (*(buff+2)==0x00)) return 84;
    if((*(buff+0)==0xFF) && (*(buff+1)==0x33) && (*(buff+2)==0x00)) return 85;
    if((*(buff+0)==0x00) && (*(buff+1)==0x33) && (*(buff+2)==0x33)) return 86;
    if((*(buff+0)==0x33) && (*(buff+1)==0x33) && (*(buff+2)==0x33)) return 87;
    if((*(buff+0)==0x66) && (*(buff+1)==0x33) && (*(buff+2)==0x33)) return 88;
    if((*(buff+0)==0x99) && (*(buff+1)==0x33) && (*(buff+2)==0x33)) return 89;
    if((*(buff+0)==0xCC) && (*(buff+1)==0x33) && (*(buff+2)==0x33)) return 90;
    if((*(buff+0)==0xFF) && (*(buff+1)==0x33) && (*(buff+2)==0x33)) return 91;
    if((*(buff+0)==0x00) && (*(buff+1)==0x33) && (*(buff+2)==0x66)) return 92;
    if((*(buff+0)==0x33) && (*(buff+1)==0x33) && (*(buff+2)==0x66)) return 93;
    if((*(buff+0)==0x66) && (*(buff+1)==0x33) && (*(buff+2)==0x66)) return 94;
    if((*(buff+0)==0x99) && (*(buff+1)==0x33) && (*(buff+2)==0x66)) return 95;
    if((*(buff+0)==0xCC) && (*(buff+1)==0x33) && (*(buff+2)==0x66)) return 96;
    if((*(buff+0)==0xFF) && (*(buff+1)==0x33) && (*(buff+2)==0x66)) return 97;
    if((*(buff+0)==0x00) && (*(buff+1)==0x33) && (*(buff+2)==0x99)) return 98;
    if((*(buff+0)==0x33) && (*(buff+1)==0x33) && (*(buff+2)==0x99)) return 99;
    if((*(buff+0)==0x66) && (*(buff+1)==0x33) && (*(buff+2)==0x99)) return 100;
    if((*(buff+0)==0x99) && (*(buff+1)==0x33) && (*(buff+2)==0x99)) return 101;
    if((*(buff+0)==0xCC) && (*(buff+1)==0x33) && (*(buff+2)==0x99)) return 102;
    if((*(buff+0)==0xFF) && (*(buff+1)==0x33) && (*(buff+2)==0x99)) return 103;
    if((*(buff+0)==0x00) && (*(buff+1)==0x33) && (*(buff+2)==0xCC)) return 104;
    if((*(buff+0)==0x33) && (*(buff+1)==0x33) && (*(buff+2)==0xCC)) return 105;
    if((*(buff+0)==0x66) && (*(buff+1)==0x33) && (*(buff+2)==0xCC)) return 106;
    if((*(buff+0)==0x99) && (*(buff+1)==0x33) && (*(buff+2)==0xCC)) return 107;
    if((*(buff+0)==0xCC) && (*(buff+1)==0x33) && (*(buff+2)==0xCC)) return 108;
    if((*(buff+0)==0xFF) && (*(buff+1)==0x33) && (*(buff+2)==0xCC)) return 109;
    if((*(buff+0)==0x00) && (*(buff+1)==0x33) && (*(buff+2)==0xFF)) return 110;
    if((*(buff+0)==0x33) && (*(buff+1)==0x33) && (*(buff+2)==0xFF)) return 111;
    if((*(buff+0)==0x66) && (*(buff+1)==0x33) && (*(buff+2)==0xFF)) return 112;
    if((*(buff+0)==0x99) && (*(buff+1)==0x33) && (*(buff+2)==0xFF)) return 113;
    if((*(buff+0)==0xCC) && (*(buff+1)==0x33) && (*(buff+2)==0xFF)) return 114;
    if((*(buff+0)==0xFF) && (*(buff+1)==0x33) && (*(buff+2)==0xFF)) return 115;
    if((*(buff+0)==0x00) && (*(buff+1)==0x66) && (*(buff+2)==0x00)) return 116;
    if((*(buff+0)==0x33) && (*(buff+1)==0x66) && (*(buff+2)==0x00)) return 117;
    if((*(buff+0)==0x66) && (*(buff+1)==0x66) && (*(buff+2)==0x00)) return 118;
    if((*(buff+0)==0x99) && (*(buff+1)==0x66) && (*(buff+2)==0x00)) return 119;
    if((*(buff+0)==0xCC) && (*(buff+1)==0x66) && (*(buff+2)==0x00)) return 120;
    if((*(buff+0)==0xFF) && (*(buff+1)==0x66) && (*(buff+2)==0x00)) return 121;
    if((*(buff+0)==0x00) && (*(buff+1)==0x66) && (*(buff+2)==0x33)) return 122;
    if((*(buff+0)==0x33) && (*(buff+1)==0x66) && (*(buff+2)==0x33)) return 123;
    if((*(buff+0)==0x66) && (*(buff+1)==0x66) && (*(buff+2)==0x33)) return 124;
    if((*(buff+0)==0x99) && (*(buff+1)==0x66) && (*(buff+2)==0x33)) return 125;
    if((*(buff+0)==0xCC) && (*(buff+1)==0x66) && (*(buff+2)==0x33)) return 126;
    if((*(buff+0)==0xFF) && (*(buff+1)==0x66) && (*(buff+2)==0x33)) return 127;
    if((*(buff+0)==0x00) && (*(buff+1)==0x66) && (*(buff+2)==0x66)) return 128;
    if((*(buff+0)==0x33) && (*(buff+1)==0x66) && (*(buff+2)==0x66)) return 129;
    if((*(buff+0)==0x66) && (*(buff+1)==0x66) && (*(buff+2)==0x66)) return 130;
    if((*(buff+0)==0x99) && (*(buff+1)==0x66) && (*(buff+2)==0x66)) return 131;
    if((*(buff+0)==0xCC) && (*(buff+1)==0x66) && (*(buff+2)==0x66)) return 132;
    if((*(buff+0)==0xFF) && (*(buff+1)==0x66) && (*(buff+2)==0x66)) return 133;
    if((*(buff+0)==0x00) && (*(buff+1)==0x66) && (*(buff+2)==0x99)) return 134;
    if((*(buff+0)==0x33) && (*(buff+1)==0x66) && (*(buff+2)==0x99)) return 135;
    if((*(buff+0)==0x66) && (*(buff+1)==0x66) && (*(buff+2)==0x99)) return 136;
    if((*(buff+0)==0x99) && (*(buff+1)==0x66) && (*(buff+2)==0x99)) return 137;
    if((*(buff+0)==0xCC) && (*(buff+1)==0x66) && (*(buff+2)==0x99)) return 138;
    if((*(buff+0)==0xFF) && (*(buff+1)==0x66) && (*(buff+2)==0x99)) return 139;
    if((*(buff+0)==0x00) && (*(buff+1)==0x66) && (*(buff+2)==0xCC)) return 140;
    if((*(buff+0)==0x33) && (*(buff+1)==0x66) && (*(buff+2)==0xCC)) return 141;
    if((*(buff+0)==0x66) && (*(buff+1)==0x66) && (*(buff+2)==0xCC)) return 142;
    if((*(buff+0)==0x99) && (*(buff+1)==0x66) && (*(buff+2)==0xCC)) return 143;
    if((*(buff+0)==0xCC) && (*(buff+1)==0x66) && (*(buff+2)==0xCC)) return 144;
    if((*(buff+0)==0xFF) && (*(buff+1)==0x66) && (*(buff+2)==0xCC)) return 145;
    if((*(buff+0)==0x00) && (*(buff+1)==0x66) && (*(buff+2)==0xFF)) return 146;
    if((*(buff+0)==0x33) && (*(buff+1)==0x66) && (*(buff+2)==0xFF)) return 147;
    if((*(buff+0)==0x66) && (*(buff+1)==0x66) && (*(buff+2)==0xFF)) return 148;
    if((*(buff+0)==0x99) && (*(buff+1)==0x66) && (*(buff+2)==0xFF)) return 149;
    if((*(buff+0)==0xCC) && (*(buff+1)==0x66) && (*(buff+2)==0xFF)) return 150;
    if((*(buff+0)==0xFF) && (*(buff+1)==0x66) && (*(buff+2)==0xFF)) return 151;
    if((*(buff+0)==0x00) && (*(buff+1)==0x99) && (*(buff+2)==0x00)) return 152;
    if((*(buff+0)==0x33) && (*(buff+1)==0x99) && (*(buff+2)==0x00)) return 153;
    if((*(buff+0)==0x66) && (*(buff+1)==0x99) && (*(buff+2)==0x00)) return 154;
    if((*(buff+0)==0x99) && (*(buff+1)==0x99) && (*(buff+2)==0x00)) return 155;
    if((*(buff+0)==0xCC) && (*(buff+1)==0x99) && (*(buff+2)==0x00)) return 156;
    if((*(buff+0)==0xFF) && (*(buff+1)==0x99) && (*(buff+2)==0x00)) return 157;
    if((*(buff+0)==0x00) && (*(buff+1)==0x99) && (*(buff+2)==0x33)) return 158;
    if((*(buff+0)==0x33) && (*(buff+1)==0x99) && (*(buff+2)==0x33)) return 159;
    if((*(buff+0)==0x66) && (*(buff+1)==0x99) && (*(buff+2)==0x33)) return 160;
    if((*(buff+0)==0x99) && (*(buff+1)==0x99) && (*(buff+2)==0x33)) return 161;
    if((*(buff+0)==0xCC) && (*(buff+1)==0x99) && (*(buff+2)==0x33)) return 162;
    if((*(buff+0)==0xFF) && (*(buff+1)==0x99) && (*(buff+2)==0x33)) return 163;
    if((*(buff+0)==0x00) && (*(buff+1)==0x99) && (*(buff+2)==0x66)) return 164;
    if((*(buff+0)==0x33) && (*(buff+1)==0x99) && (*(buff+2)==0x66)) return 165;
    if((*(buff+0)==0x66) && (*(buff+1)==0x99) && (*(buff+2)==0x66)) return 166;
    if((*(buff+0)==0x99) && (*(buff+1)==0x99) && (*(buff+2)==0x66)) return 167;
    if((*(buff+0)==0xCC) && (*(buff+1)==0x99) && (*(buff+2)==0x66)) return 168;
    if((*(buff+0)==0xFF) && (*(buff+1)==0x99) && (*(buff+2)==0x66)) return 169;
    if((*(buff+0)==0x00) && (*(buff+1)==0x99) && (*(buff+2)==0x99)) return 170;
    if((*(buff+0)==0x33) && (*(buff+1)==0x99) && (*(buff+2)==0x99)) return 171;
    if((*(buff+0)==0x66) && (*(buff+1)==0x99) && (*(buff+2)==0x99)) return 172;
    if((*(buff+0)==0x99) && (*(buff+1)==0x99) && (*(buff+2)==0x99)) return 173;
    if((*(buff+0)==0xCC) && (*(buff+1)==0x99) && (*(buff+2)==0x99)) return 174;
    if((*(buff+0)==0xFF) && (*(buff+1)==0x99) && (*(buff+2)==0x99)) return 175;
    if((*(buff+0)==0x00) && (*(buff+1)==0x99) && (*(buff+2)==0xCC)) return 176;
    if((*(buff+0)==0x33) && (*(buff+1)==0x99) && (*(buff+2)==0xCC)) return 177;
    if((*(buff+0)==0x66) && (*(buff+1)==0x99) && (*(buff+2)==0xCC)) return 178;
    if((*(buff+0)==0x99) && (*(buff+1)==0x99) && (*(buff+2)==0xCC)) return 179;
    if((*(buff+0)==0xCC) && (*(buff+1)==0x99) && (*(buff+2)==0xCC)) return 180;
    if((*(buff+0)==0xFF) && (*(buff+1)==0x99) && (*(buff+2)==0xCC)) return 181;
    if((*(buff+0)==0x00) && (*(buff+1)==0x99) && (*(buff+2)==0xFF)) return 182;
    if((*(buff+0)==0x33) && (*(buff+1)==0x99) && (*(buff+2)==0xFF)) return 183;
    if((*(buff+0)==0x66) && (*(buff+1)==0x99) && (*(buff+2)==0xFF)) return 184;
    if((*(buff+0)==0x99) && (*(buff+1)==0x99) && (*(buff+2)==0xFF)) return 185;
    if((*(buff+0)==0xCC) && (*(buff+1)==0x99) && (*(buff+2)==0xFF)) return 186;
    if((*(buff+0)==0xFF) && (*(buff+1)==0x99) && (*(buff+2)==0xFF)) return 187;
    if((*(buff+0)==0x00) && (*(buff+1)==0xCC) && (*(buff+2)==0x00)) return 188;
    if((*(buff+0)==0x33) && (*(buff+1)==0xCC) && (*(buff+2)==0x00)) return 189;
    if((*(buff+0)==0x66) && (*(buff+1)==0xCC) && (*(buff+2)==0x00)) return 190;
    if((*(buff+0)==0x99) && (*(buff+1)==0xCC) && (*(buff+2)==0x00)) return 191;
    if((*(buff+0)==0xCC) && (*(buff+1)==0xCC) && (*(buff+2)==0x00)) return 192;
    if((*(buff+0)==0xFF) && (*(buff+1)==0xCC) && (*(buff+2)==0x00)) return 193;
    if((*(buff+0)==0x00) && (*(buff+1)==0xCC) && (*(buff+2)==0x33)) return 194;
    if((*(buff+0)==0x33) && (*(buff+1)==0xCC) && (*(buff+2)==0x33)) return 195;
    if((*(buff+0)==0x66) && (*(buff+1)==0xCC) && (*(buff+2)==0x33)) return 196;
    if((*(buff+0)==0x99) && (*(buff+1)==0xCC) && (*(buff+2)==0x33)) return 197;
    if((*(buff+0)==0xCC) && (*(buff+1)==0xCC) && (*(buff+2)==0x33)) return 198;
    if((*(buff+0)==0xFF) && (*(buff+1)==0xCC) && (*(buff+2)==0x33)) return 199;
    if((*(buff+0)==0x00) && (*(buff+1)==0xCC) && (*(buff+2)==0x66)) return 200;
    if((*(buff+0)==0x33) && (*(buff+1)==0xCC) && (*(buff+2)==0x66)) return 201;
    if((*(buff+0)==0x66) && (*(buff+1)==0xCC) && (*(buff+2)==0x66)) return 202;
    if((*(buff+0)==0x99) && (*(buff+1)==0xCC) && (*(buff+2)==0x66)) return 203;
    if((*(buff+0)==0xCC) && (*(buff+1)==0xCC) && (*(buff+2)==0x66)) return 204;
    if((*(buff+0)==0xFF) && (*(buff+1)==0xCC) && (*(buff+2)==0x66)) return 205;
    if((*(buff+0)==0x00) && (*(buff+1)==0xCC) && (*(buff+2)==0x99)) return 206;
    if((*(buff+0)==0x33) && (*(buff+1)==0xCC) && (*(buff+2)==0x99)) return 207;
    if((*(buff+0)==0x66) && (*(buff+1)==0xCC) && (*(buff+2)==0x99)) return 208;
    if((*(buff+0)==0x99) && (*(buff+1)==0xCC) && (*(buff+2)==0x99)) return 209;
    if((*(buff+0)==0xCC) && (*(buff+1)==0xCC) && (*(buff+2)==0x99)) return 210;
    if((*(buff+0)==0xFF) && (*(buff+1)==0xCC) && (*(buff+2)==0x99)) return 211;
    if((*(buff+0)==0x00) && (*(buff+1)==0xCC) && (*(buff+2)==0xCC)) return 212;
    if((*(buff+0)==0x33) && (*(buff+1)==0xCC) && (*(buff+2)==0xCC)) return 213;
    if((*(buff+0)==0x66) && (*(buff+1)==0xCC) && (*(buff+2)==0xCC)) return 214;
    if((*(buff+0)==0x99) && (*(buff+1)==0xCC) && (*(buff+2)==0xCC)) return 215;
    if((*(buff+0)==0xCC) && (*(buff+1)==0xCC) && (*(buff+2)==0xCC)) return 216;
    if((*(buff+0)==0xFF) && (*(buff+1)==0xCC) && (*(buff+2)==0xCC)) return 217;
    if((*(buff+0)==0x00) && (*(buff+1)==0xCC) && (*(buff+2)==0xFF)) return 218;
    if((*(buff+0)==0x33) && (*(buff+1)==0xCC) && (*(buff+2)==0xFF)) return 219;
    if((*(buff+0)==0x66) && (*(buff+1)==0xCC) && (*(buff+2)==0xFF)) return 220;
    if((*(buff+0)==0x99) && (*(buff+1)==0xCC) && (*(buff+2)==0xFF)) return 221;
    if((*(buff+0)==0xCC) && (*(buff+1)==0xCC) && (*(buff+2)==0xFF)) return 222;
    if((*(buff+0)==0xFF) && (*(buff+1)==0xCC) && (*(buff+2)==0xFF)) return 223;
    if((*(buff+0)==0x33) && (*(buff+1)==0xFF) && (*(buff+2)==0x00)) return 224;
    if((*(buff+0)==0x66) && (*(buff+1)==0xFF) && (*(buff+2)==0x00)) return 225;
    if((*(buff+0)==0x99) && (*(buff+1)==0xFF) && (*(buff+2)==0x00)) return 226;
    if((*(buff+0)==0xCC) && (*(buff+1)==0xFF) && (*(buff+2)==0x00)) return 227;
    if((*(buff+0)==0x00) && (*(buff+1)==0xFF) && (*(buff+2)==0x33)) return 228;
    if((*(buff+0)==0x33) && (*(buff+1)==0xFF) && (*(buff+2)==0x33)) return 229;
    if((*(buff+0)==0x66) && (*(buff+1)==0xFF) && (*(buff+2)==0x33)) return 230;
    if((*(buff+0)==0x99) && (*(buff+1)==0xFF) && (*(buff+2)==0x33)) return 231;
    if((*(buff+0)==0xCC) && (*(buff+1)==0xFF) && (*(buff+2)==0x33)) return 232;
    if((*(buff+0)==0xFF) && (*(buff+1)==0xFF) && (*(buff+2)==0x33)) return 233;
    if((*(buff+0)==0x00) && (*(buff+1)==0xFF) && (*(buff+2)==0x66)) return 234;
    if((*(buff+0)==0x33) && (*(buff+1)==0xFF) && (*(buff+2)==0x66)) return 235;
    if((*(buff+0)==0x66) && (*(buff+1)==0xFF) && (*(buff+2)==0x66)) return 236;
    if((*(buff+0)==0x99) && (*(buff+1)==0xFF) && (*(buff+2)==0x66)) return 237;
    if((*(buff+0)==0xCC) && (*(buff+1)==0xFF) && (*(buff+2)==0x66)) return 238;
    if((*(buff+0)==0xFF) && (*(buff+1)==0xFF) && (*(buff+2)==0x66)) return 239;
    if((*(buff+0)==0x00) && (*(buff+1)==0xFF) && (*(buff+2)==0x99)) return 240;
    if((*(buff+0)==0x33) && (*(buff+1)==0xFF) && (*(buff+2)==0x99)) return 241;
    if((*(buff+0)==0x66) && (*(buff+1)==0xFF) && (*(buff+2)==0x99)) return 242;
    if((*(buff+0)==0x99) && (*(buff+1)==0xFF) && (*(buff+2)==0x99)) return 243;
    if((*(buff+0)==0xCC) && (*(buff+1)==0xFF) && (*(buff+2)==0x99)) return 244;
    if((*(buff+0)==0xFF) && (*(buff+1)==0xFF) && (*(buff+2)==0x99)) return 245;
    if((*(buff+0)==0x00) && (*(buff+1)==0xFF) && (*(buff+2)==0xCC)) return 246;
    if((*(buff+0)==0x33) && (*(buff+1)==0xFF) && (*(buff+2)==0xCC)) return 247;
    if((*(buff+0)==0x66) && (*(buff+1)==0xFF) && (*(buff+2)==0xCC)) return 248;
    if((*(buff+0)==0x99) && (*(buff+1)==0xFF) && (*(buff+2)==0xCC)) return 249;
    if((*(buff+0)==0xCC) && (*(buff+1)==0xFF) && (*(buff+2)==0xCC)) return 250;
    if((*(buff+0)==0xFF) && (*(buff+1)==0xFF) && (*(buff+2)==0xCC)) return 251;
    if((*(buff+0)==0x33) && (*(buff+1)==0xFF) && (*(buff+2)==0xFF)) return 252;
    if((*(buff+0)==0x66) && (*(buff+1)==0xFF) && (*(buff+2)==0xFF)) return 253;
    if((*(buff+0)==0x99) && (*(buff+1)==0xFF) && (*(buff+2)==0xFF)) return 254;
    if((*(buff+0)==0xCC) && (*(buff+1)==0xFF) && (*(buff+2)==0xFF)) return 255;

    return -1;
}

