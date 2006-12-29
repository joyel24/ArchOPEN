/*
* include/gui/icons_data.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __GUI_ICONS_DATA_H
#define __GUI_ICONS_DATA_H

/* brown */
#define BR 135

/****************************
 * Large 65x10 "LINAV" logo */
/*unsigned char linav_logo[10][65] = {
{01,01,01,38,38,38,38,38,38,38,38,38,38,38,01,01,01,38,38,38,01,01,01,01,01,38,38,38,38,01,01,01,38,38,38,38,38,38,38,38,01,01,01,38,38,38,38,38,01,01,01,01,38,38,38,38,38,38,38,38,38,01,01,01,01},
{01,01,01,38,38,38,38,38,38,38,38,38,38,38,01,01,01,38,38,01,01,01,01,01,01,01,38,38,38,01,01,01,38,38,38,38,38,38,01,01,01,01,01,01,01,38,38,38,38,01,01,01,01,38,38,38,38,38,38,38,01,01,01,01,38},
{01,01,01,38,38,38,38,38,38,38,38,38,38,38,01,01,01,38,38,01,01,01,01,01,01,01,38,38,38,01,01,01,38,38,38,38,38,01,01,01,01,01,01,01,01,01,38,38,38,01,01,01,01,01,38,38,38,38,38,01,01,01,01,01,38},
{01,01,01,38,38,38,38,38,38,38,38,38,38,38,01,01,01,38,38,01,01,01,38,01,01,01,01,38,38,01,01,01,38,38,38,38,01,01,01,01,01,38,01,01,01,01,01,38,38,38,01,01,01,01,38,38,38,38,38,01,01,01,01,38,38},
{01,01,01,38,38,38,38,38,38,38,38,38,38,38,01,01,01,38,38,01,01,01,38,38,01,01,01,38,38,01,01,01,38,38,38,38,01,01,01,01,38,38,38,01,01,01,01,38,38,38,38,01,01,01,01,38,38,38,01,01,01,01,38,38,38},
{01,01,01,38,38,38,38,38,38,38,38,38,38,38,01,01,01,38,38,01,01,01,38,38,01,01,01,01,38,01,01,01,38,38,38,01,01,01,01,01,38,38,38,01,01,01,01,01,38,38,38,01,01,01,01,38,38,38,01,01,01,01,38,38,38},
{01,01,01,38,38,38,38,38,38,38,38,38,38,38,01,01,01,38,38,01,01,01,38,38,38,01,01,01,01,01,01,01,38,38,38,01,01,01,01,01,01,01,01,01,01,01,01,01,38,38,38,38,01,01,01,01,01,01,01,01,01,38,38,38,38},
{01,01,01,38,38,38,38,38,38,38,38,38,38,38,01,01,01,38,38,01,01,01,38,38,38,01,01,01,01,01,01,01,38,38,01,01,01,01,01,01,01,01,01,01,01,01,01,01,01,38,38,38,01,01,01,01,01,01,01,01,01,38,38,38,38},
{38,01,01,01,01,01,01,01,01,01,01,01,38,38,01,01,01,38,38,01,01,01,38,38,38,38,01,01,01,01,01,01,38,38,01,01,01,01,38,38,38,38,38,38,38,01,01,01,01,38,38,38,38,01,01,01,01,01,01,01,38,38,38,38,38},
{38,38,01,01,01,01,01,01,01,01,01,01,38,38,01,01,01,38,38,01,01,01,38,38,38,38,38,01,01,01,01,38,38,01,01,01,01,38,38,38,38,38,38,38,38,38,01,01,01,01,38,38,38,38,38,01,01,01,38,38,38,38,38,38,38} };
*/
//BITMAP linavLogo = {(unsigned int) linav_logo, 65, 10, 0, 0};

unsigned char medios_logo[10][51] = {
{38,01,01,01,01,38,38,38,38,01,01,01,01,38,38,38,01,01,01,01,38,38,01,01,01,01,01,01,38,38,38,01,01,01,38,38,38,01,01,01,01,01,38,38,38,38,38,01,01,01,01},
{01,01,01,01,01,01,38,38,01,01,01,01,01,01,38,01,01,01,01,01,01,38,01,01,01,01,01,01,01,38,38,01,01,01,38,38,01,01,01,01,01,01,01,38,38,38,01,01,01,01,01},
{01,01,01,01,01,01,01,01,01,01,01,01,01,01,38,01,01,01,01,01,01,38,01,01,01,38,38,01,01,01,38,01,01,01,38,01,01,01,01,01,01,01,01,01,38,01,01,01,38,38,38},
{01,01,01,38,01,01,01,01,01,01,38,01,01,01,38,01,01,01,38,38,38,38,01,01,01,38,38,38,01,01,38,01,01,01,38,01,01,01,38,38,38,01,01,01,38,01,01,01,38,38,38},
{01,01,01,38,38,01,01,01,01,38,38,01,01,01,38,01,01,01,01,01,01,38,01,01,01,38,38,38,01,01,38,01,01,01,38,01,01,01,38,38,38,01,01,01,38,38,01,01,01,38,38},
{01,01,01,38,38,38,01,01,38,38,38,01,01,01,38,01,01,01,01,01,01,38,01,01,01,38,38,38,01,01,38,01,01,01,38,01,01,01,38,38,38,01,01,01,38,38,38,01,01,01,38},
{01,01,01,38,38,38,38,38,38,38,38,01,01,01,38,01,01,01,38,38,38,38,01,01,01,38,38,38,01,01,38,01,01,01,38,01,01,01,38,38,38,01,01,01,38,38,38,38,01,01,01},
{01,01,01,38,38,38,38,38,38,38,38,01,01,01,38,01,01,01,01,01,01,38,01,01,01,38,38,01,01,01,38,01,01,01,38,01,01,01,01,01,01,01,01,01,38,38,38,38,01,01,01},
{01,01,01,38,38,38,38,38,38,38,38,01,01,01,38,01,01,01,01,01,01,38,01,01,01,01,01,01,01,38,38,01,01,01,38,38,01,01,01,01,01,01,01,38,38,01,01,01,01,01,38},
{01,01,01,38,38,38,38,38,38,38,38,01,01,01,38,38,01,01,01,01,38,38,01,01,01,01,01,01,38,38,38,01,01,01,38,38,38,01,01,01,01,01,38,38,38,01,01,01,01,38,38} };

/************************
 * 15x6 USB status icon */
unsigned char usb_icon[6][15] = {
{38,38,38,38,38,01,01,01,01,01,01,38,38,38,38},
{38,38,38,38,01,01,01,01,01,01,01,01,01,01,01},
{01,01,01,01,01,01,01,01,01,01,01,38,01,38,01},
{01,01,01,01,01,01,01,01,01,01,01,38,01,38,01},
{38,38,38,38,01,01,01,01,01,01,01,01,01,01,01},
{38,38,38,38,38,01,01,01,01,01,01,38,38,38,38} };
//BITMAP usbIcon = {(unsigned int) usb_icon, 15, 6, 0, 0};

/************************
 * 15x6 FW status icon */
unsigned char fw_ext_icon[6][15] = {
{38,38,38,01,01,38,38,38,38,38,01,01,38,38,38},
{38,38,38,38,01,01,38,38,38,01,01,38,38,38,38},
{38,38,38,38,38,01,01,38,01,01,38,38,38,38,38},
{38,38,38,38,38,38,01,01,01,38,38,38,38,38,38},
{38,38,38,38,38,38,01,01,01,38,38,38,38,38,38},
{38,38,38,38,38,38,01,01,01,38,38,38,38,38,38} };
//BITMAP fwExtIcon = {(unsigned int) fw_ext_icon, 15, 6, 0, 0};

/************************
 * 15x6 CF status icon */
unsigned char cf_icon[6][15] = {
{38,38,01,01,01,38,38,38,01,01,01,01,38,38,38},
{38,01,01,01,01,38,38,38,01,01,01,38,38,38,38},
{01,01,38,38,38,38,38,01,01,38,38,38,38,38,38},
{01,01,38,38,38,38,38,01,01,01,01,38,38,38,38},
{38,01,01,01,01,38,01,01,01,01,38,38,38,38,38},
{38,38,01,01,01,38,01,01,38,38,38,38,38,38,38} };
//BITMAP cfIcon = {(unsigned int) cf_icon, 15, 6, 0, 0};


/**************************
 * 13x6 Power status icon */
unsigned char power_icon[6][13] = {
{38,38,38,38,38,01,01,01,01,38,38,38,38},
{38,38,38,38,01,01,01,01,01,01,01,01,01},
{01,01,01,01,01,01,01,01,01,38,38,38,38},
{01,01,01,01,01,01,01,01,01,38,38,38,38},
{38,38,38,38,01,01,01,01,01,01,01,01,01},
{38,38,38,38,38,01,01,01,01,38,38,38,38} };
//BITMAP powerIcon = {(unsigned int) power_icon, 13, 6, 0, 0};

/*********************
 * 9x9 Up Arrow icon */
unsigned char upArrow[9][9] =
{   {19,19,19,19,00,19,19,19,19},
    {19,19,19,00,00,00,19,19,19},
    {19,19,00,19,00,19,00,19,19},
    {19,00,19,19,00,19,19,00,19},
    {19,19,19,19,00,19,19,19,19},
    {19,19,19,19,00,19,19,19,19},
    {19,19,19,19,00,19,19,19,19},
    {19,19,19,19,00,19,19,19,19},
    {19,19,19,19,00,19,19,19,19},
};
//BITMAP upBitmap = {(unsigned int) upArrow, 9, 9, 0, 0};

/***********************
 * 9x9 Down Arrow icon */
unsigned char dwArrow[9][9] =
{   {19,19,19,19,00,19,19,19,19},
    {19,19,19,19,00,19,19,19,19},
    {19,19,19,19,00,19,19,19,19},
    {19,19,19,19,00,19,19,19,19},
    {19,19,19,19,00,19,19,19,19},
    {19,00,19,19,00,19,19,00,19},
    {19,19,00,19,00,19,00,19,19},
    {19,19,19,00,00,00,19,19,19},
    {19,19,19,19,00,19,19,19,19},
};
//BITMAP dwBitmap = {(unsigned int) dwArrow, 9, 9, 0, 0};

/**********************
 * 8x8 Directory icon */
unsigned char dir[8][8] =
{   {00,00,00,15,15,15,15,15},
    {00,22,00,00,00,15,15,15},
    {00,22,22,22,00,15,15,15},
    {00,22,22,22,00,15,15,15},
    {00,22,22,22,00,BR,BR,00},
    {00,22,22,22,00,BR,00,15},
    {00,22,22,22,00,00,15,15},
    {00,00,00,00,00,15,15,15},
};
//BITMAP dirBitmap = {(unsigned int) dir, 8, 8, 0, 0};

/*********************
 * 8x8 MP3 file icon */
unsigned char mp3[8][8] =
{   {15,15,00,00,00,00,00,00},
    {15,15,00,00,00,00,00,00},
    {15,15,00,15,15,15,15,00},
    {15,15,00,15,15,15,15,00},
    {15,15,00,15,15,15,00,00},
    {15,00,00,15,15,00,22,00},
    {00,22,00,15,15,15,00,15},
    {15,00,15,15,15,15,15,15},
};
//BITMAP mp3Bitmap = {(unsigned int) mp3, 8, 8, 0, 0};

/*********************
 * 8x8 TXT file icon */
unsigned char text[8][8] =
{   {15,00,00,00,00,00,00,15},
    {15,00,15,15,15,15,00,15},
    {15,00,15,00,00,15,00,15},
    {15,00,15,15,15,15,00,15},
    {15,00,15,00,00,15,00,15},
    {15,00,15,15,15,15,00,15},
    {15,00,15,15,15,15,00,15},
    {15,00,00,00,00,00,00,15},
};
//BITMAP textBitmap = {(unsigned int) text, 8, 8, 0, 0};

/***********************
 * 8x8 Image file icon */
unsigned char image[8][8] =
{   {15,15,15,15,15,15,15,15},
    {15,15,00,00,00,00,15,15},
    {15,00,BR,00,00,BR,00,15},
    {00,21,00,BR,BR,00,21,00},
    {00,21,00,BR,BR,00,21,00},
    {15,00,BR,00,00,BR,00,15},
    {15,15,00,00,00,00,15,15},
    {15,15,15,15,15,15,15,15},
};
//BITMAP imageBitmap = {(unsigned int) image, 8, 8, 0, 0};

/////////////////////////////////////////////////////////////////
// Icons for message boxes
unsigned char MsgBoxExclamation[18][18] =
{
{
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x0e, 0x0e, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x0e, 0x0e, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x0e, 0x0e, 0x0e, 0x0e, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x0e, 0x00, 0x00, 0x0e, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x0e, 0x0e, 0x00, 0x00, 0x0e, 0x0e, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x03, 0x00, 0x0e, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x0e, 0x00, 0x03, 0x03, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x03, 0x00, 0x0e, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x0e, 0x00, 0x03, 0x03, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x00, 0x0e, 0x0e, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x0e, 0x0e, 0x00, 0x03, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x00, 0x0e, 0x0e, 0x0e, 0x0e, 0x00, 0x00, 0x0e, 0x0e, 0x0e, 0x0e, 0x00, 0x03, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x00, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x00, 0x00, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x00, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x00, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x00, 0x00, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x00, 0x03, 0x03
},
{ 
    0x03, 0x00, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x00, 0x03
},
{ 
    0x03, 0x00, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x00, 0x00, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x00, 0x03
},
{
    0x00, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x00, 0x00, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x00
},
{ 
    0x00, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x00
},
{ 
    0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03
}
};
//BITMAP MsgBoxExclamationBitmap = {(unsigned int) MsgBoxExclamation, 18, 18, 0, 0};

unsigned char MsgBoxQuestion[18][18] =
{
{
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x00, 0x03, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x03, 0x03, 0x02, 0x02, 0x02, 0x02, 0x02, 0x00, 0x03, 0x03
},
{ 
    0x03, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x02, 0x02, 0x02, 0x02, 0x02, 0x00, 0x03
},
{ 
    0x03, 0x00, 0x02, 0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x02, 0x02, 0x02, 0x02, 0x00, 0x03
},
{ 
    0x03, 0x00, 0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x02, 0x02, 0x03, 0x03, 0x03, 0x02, 0x02, 0x02, 0x00, 0x03
},
{ 
    0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x03, 0x02, 0x02, 0x02, 0x02, 0x03, 0x03, 0x02, 0x02, 0x02, 0x02, 0x00
},
{ 
    0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x02, 0x02, 0x02, 0x02, 0x00
},
{ 
    0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x02, 0x02, 0x02, 0x02, 0x02, 0x00
},
{ 
    0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x03, 0x03, 0x03, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x00
},
{ 
    0x03, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x03, 0x03, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x00, 0x03
},
{ 
    0x03, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x00, 0x03
},
{ 
    0x03, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x03, 0x03, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x00, 0x03
},
{ 
    0x03, 0x03, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x03, 0x03, 0x02, 0x02, 0x02, 0x02, 0x02, 0x00, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x00, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x00, 0x03, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03
}
};
//BITMAP MsgBoxQuestionBitmap = {(unsigned int) MsgBoxQuestion, 18, 18, 0, 0};

unsigned char MsgBoxWarning[18][18] =
{
{
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x0e, 0x0e, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x0e, 0x0e, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x0e, 0x0e, 0x0e, 0x0e, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x0e, 0x00, 0x00, 0x0e, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x0e, 0x0e, 0x00, 0x00, 0x0e, 0x0e, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x03, 0x00, 0x0e, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x0e, 0x00, 0x03, 0x03, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x03, 0x00, 0x0e, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x0e, 0x00, 0x03, 0x03, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x00, 0x0e, 0x0e, 0x0e, 0x00, 0x00, 0x00, 0x00, 0x0e, 0x0e, 0x0e, 0x00, 0x03, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x00, 0x0e, 0x0e, 0x0e, 0x0e, 0x00, 0x00, 0x0e, 0x0e, 0x0e, 0x0e, 0x00, 0x03, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x00, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x00, 0x00, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x00, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x00, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x00, 0x00, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x00, 0x03, 0x03
},
{ 
    0x03, 0x00, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x00, 0x03
},
{ 
    0x03, 0x00, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x00, 0x00, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x00, 0x03
},
{
    0x00, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x00, 0x00, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x00
},
{ 
    0x00, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x0e, 0x00
},
{ 
    0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03
}
};
//BITMAP MsgBoxWarningBitmap = {(unsigned int) MsgBoxWarning, 18, 18, 0, 0};

unsigned char MsgBoxInformation[18][18] =
{
{
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x03, 0x03, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x03, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x09, 0x09, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x03, 0x03
},
{ 
    0x03, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x09, 0x09, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x03
},
{ 
    0x03, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x03
},
{ 
    0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x09, 0x09, 0x09, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00
},
{ 
    0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x09, 0x09, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00
},
{ 
    0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x09, 0x09, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00
},
{ 
    0x03, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x09, 0x09, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x03
},
{ 
    0x03, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x09, 0x09, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x03
},
{ 
    0x03, 0x03, 0x00, 0x03, 0x03, 0x03, 0x03, 0x09, 0x09, 0x09, 0x09, 0x03, 0x03, 0x03, 0x03, 0x00, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x03, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x03, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x00, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x03, 0x00, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x03, 0x00, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x03
}
};

unsigned char MsgBoxError[18][18] =
{
{
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x0c, 0x0c, 0x0c, 0x0c, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x00, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x00, 0x03, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x00, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x00, 0x03, 0x03
},
{ 
    0x03, 0x00, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x00, 0x03
},
{ 
    0x03, 0x00, 0x0c, 0x0c, 0x0c, 0x0c, 0x03, 0x0c, 0x0c, 0x0c, 0x0c, 0x03, 0x0c, 0x0c, 0x0c, 0x0c, 0x00, 0x03
},
{ 
    0x03, 0x00, 0x0c, 0x0c, 0x0c, 0x03, 0x03, 0x03, 0x0c, 0x0c, 0x03, 0x03, 0x03, 0x0c, 0x0c, 0x0c, 0x00, 0x03
},
{ 
    0x00, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x00
},
{ 
    0x00, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x03, 0x03, 0x03, 0x03, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x00
},
{ 
    0x00, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x03, 0x03, 0x03, 0x03, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x00
},
{ 
    0x00, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x00
},
{ 
    0x03, 0x00, 0x0c, 0x0c, 0x0c, 0x03, 0x03, 0x03, 0x0c, 0x0c, 0x03, 0x03, 0x03, 0x0c, 0x0c, 0x0c, 0x00, 0x03
},
{ 
    0x03, 0x00, 0x0c, 0x0c, 0x0c, 0x0c, 0x03, 0x0c, 0x0c, 0x0c, 0x0c, 0x03, 0x0c, 0x0c, 0x0c, 0x0c, 0x00, 0x03
},
{ 
    0x03, 0x00, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x00, 0x03
},
{ 
    0x03, 0x03, 0x00, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x00, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x00, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x0c, 0x00, 0x03, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x0c, 0x0c, 0x0c, 0x0c, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x03
},
{ 
    0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03
}
};
//BITMAP MsgBoxErrorBitmap = {(unsigned int) MsgBoxError, 18, 18, 0, 0};

/////////////////////////////////////////////////////////////////


#endif