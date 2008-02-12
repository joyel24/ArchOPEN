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

/* color def */
#define BR 135
#define BL 03
#define WT 15
#define BG 00
#define YL 14
#define GR 02
#define GA 7

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
 * 15x6 internal HP status icon */
unsigned char intHP_icon[10][8] = {
{38,38,38,38,38,38,38,BL},
{38,38,38,38,38,38,BL,BL},
{BL,BL,BL,38,38,BL,GR,BL},
{BL,GA,BL,38,BL,GR,GR,BL},
{BL,GA,BL,BL,GR,GR,GR,BL},
{BL,GA,BL,BL,GR,GR,GR,BL},
{BL,GA,BL,38,BL,GR,GR,BL},
{BL,BL,BL,38,38,BL,GR,BL},
{38,38,38,38,38,38,BL,BL},
{38,38,38,38,38,38,38,BL}};    

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
{   {19,19,19,19,BL,19,19,19,19},
    {19,19,19,BL,BL,BL,19,19,19},
    {19,19,BL,19,BL,19,BL,19,19},
    {19,BL,19,19,BL,19,19,BL,19},
    {19,19,19,19,BL,19,19,19,19},
    {19,19,19,19,BL,19,19,19,19},
    {19,19,19,19,BL,19,19,19,19},
    {19,19,19,19,BL,19,19,19,19},
    {19,19,19,19,BL,19,19,19,19},
};
//BITMAP upBitmap = {(unsigned int) upArrow, 9, 9, 0, 0};

/***********************
 * 9x9 Down Arrow icon */
unsigned char dwArrow[9][9] =
{   {19,19,19,19,BL,19,19,19,19},
    {19,19,19,19,BL,19,19,19,19},
    {19,19,19,19,BL,19,19,19,19},
    {19,19,19,19,BL,19,19,19,19},
    {19,19,19,19,BL,19,19,19,19},
    {19,BL,19,19,BL,19,19,BL,19},
    {19,19,BL,19,BL,19,BL,19,19},
    {19,19,19,BL,BL,BL,19,19,19},
    {19,19,19,19,BL,19,19,19,19},
};
//BITMAP dwBitmap = {(unsigned int) dwArrow, 9, 9, 0, 0};

/**********************
 * 8x8 Directory icon */
unsigned char dir[8][8] =
{   {BL,BL,BL,WT,WT,WT,WT,WT},
    {BL,22,BL,BL,BL,WT,WT,WT},
    {BL,22,22,22,BL,WT,WT,WT},
    {BL,22,22,22,BL,WT,WT,WT},
    {BL,22,22,22,BL,BR,BR,BL},
    {BL,22,22,22,BL,BR,BL,WT},
    {BL,22,22,22,BL,BL,WT,WT},
    {BL,BL,BL,BL,BL,WT,WT,WT},
};
//BITMAP dirBitmap = {(unsigned int) dir, 8, 8, 0, 0};

/*********************
 * 8x8 MP3 file icon */
unsigned char mp3[8][8] =
{   {WT,WT,BL,BL,BL,BL,BL,BL},
    {WT,WT,BL,BL,BL,BL,BL,BL},
    {WT,WT,BL,WT,WT,WT,WT,BL},
    {WT,WT,BL,WT,WT,WT,WT,BL},
    {WT,WT,BL,WT,WT,WT,BL,BL},
    {WT,BL,BL,WT,WT,BL,22,BL},
    {BL,22,BL,WT,WT,WT,BL,WT},
    {WT,BL,WT,WT,WT,WT,WT,WT},
};
//BITMAP mp3Bitmap = {(unsigned int) mp3, 8, 8, 0, 0};

/*********************
 * 8x8 TXT file icon */
unsigned char text[8][8] =
{   {WT,BL,BL,BL,BL,BL,BL,WT},
    {WT,BL,WT,WT,WT,WT,BL,WT},
    {WT,BL,WT,BL,BL,WT,BL,WT},
    {WT,BL,WT,WT,WT,WT,BL,WT},
    {WT,BL,WT,BL,BL,WT,BL,WT},
    {WT,BL,WT,WT,WT,WT,BL,WT},
    {WT,BL,WT,WT,WT,WT,BL,WT},
    {WT,BL,BL,BL,BL,BL,BL,WT},
};
//BITMAP textBitmap = {(unsigned int) text, 8, 8, 0, 0};

/***********************
 * 8x8 Image file icon */
unsigned char image[8][8] =
{   {WT,WT,WT,WT,WT,WT,WT,WT},
    {WT,WT,BL,BL,BL,BL,WT,WT},
    {WT,BL,BR,BL,BL,BR,BL,WT},
    {BL,21,BL,BR,BR,BL,21,BL},
    {BL,21,BL,BR,BR,BL,21,BL},
    {WT,BL,BR,BL,BL,BR,BL,WT},
    {WT,WT,BL,BL,BL,BL,WT,WT},
    {WT,WT,WT,WT,WT,WT,WT,WT},
};
//BITMAP imageBitmap = {(unsigned int) image, 8, 8, 0, 0};

/////////////////////////////////////////////////////////////////
// Icons for message boxes
unsigned char MsgBoxExclamation[18][18] =
{
{    WT, WT, WT, WT, WT, WT, WT, WT, BL, BL, WT, WT, WT, WT, WT, WT, WT, WT},
{    WT, WT, WT, WT, WT, WT, WT, BL, YL, YL, BL, WT, WT, WT, WT, WT, WT, WT},
{    WT, WT, WT, WT, WT, WT, WT, BL, YL, YL, BL, WT, WT, WT, WT, WT, WT, WT},
{    WT, WT, WT, WT, WT, WT, BL, YL, YL, YL, YL, BL, WT, WT, WT, WT, WT, WT},
{    WT, WT, WT, WT, WT, WT, BL, YL, BL, BL, YL, BL, WT, WT, WT, WT, WT, WT},
{    WT, WT, WT, WT, WT, BL, YL, YL, BL, BL, YL, YL, BL, WT, WT, WT, WT, WT},
{    WT, WT, WT, WT, WT, BL, YL, BL, BL, BL, BL, YL, BL, WT, WT, WT, WT, WT},
{    WT, WT, WT, WT, BL, YL, YL, BL, BL, BL, BL, YL, YL, BL, WT, WT, WT, WT},
{    WT, WT, WT, WT, BL, YL, YL, BL, BL, BL, BL, YL, YL, BL, WT, WT, WT, WT},
{    WT, WT, WT, BL, YL, YL, YL, BL, BL, BL, BL, YL, YL, YL, BL, WT, WT, WT},
{    WT, WT, WT, BL, YL, YL, YL, YL, BL, BL, YL, YL, YL, YL, BL, WT, WT, WT},
{    WT, WT, BL, YL, YL, YL, YL, YL, BL, BL, YL, YL, YL, YL, YL, BL, WT, WT},
{    WT, WT, BL, YL, YL, YL, YL, YL, BL, BL, YL, YL, YL, YL, YL, BL, WT, WT},
{    WT, BL, YL, YL, YL, YL, YL, YL, YL, YL, YL, YL, YL, YL, YL, YL, BL, WT},
{    WT, BL, YL, YL, YL, YL, YL, YL, BL, BL, YL, YL, YL, YL, YL, YL, BL, WT},
{    BL, YL, YL, YL, YL, YL, YL, YL, BL, BL, YL, YL, YL, YL, YL, YL, YL, BL},
{    BL, YL, YL, YL, YL, YL, YL, YL, YL, YL, YL, YL, YL, YL, YL, YL, YL, BL},
{    WT, BL, BL, BL, BL, BL, BL, BL, BL, BL, BL, BL, BL, BL, BL, BL, BL, WT}
};
//BITMAP MsgBoxExclamationBitmap = {(unsigned int) MsgBoxExclamation, 18, 18, 0, 0};

unsigned char MsgBoxQuestion[18][18] =
{
{    WT, WT, WT, WT, WT, WT, WT, BL, BL, BL, BL, WT, WT, WT, WT, WT, WT, WT},
{    WT, WT, WT, WT, BL, BL, BL, GR, GR, GR, GR, BL, BL, BL, WT, WT, WT, WT},
{    WT, WT, WT, BL, GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, BL, WT, WT, WT},
{    WT, WT, BL, GR, GR, GR, GR, GR, WT, WT, GR, GR, GR, GR, GR, BL, WT, WT},
{    WT, BL, GR, GR, GR, GR, GR, WT, WT, WT, WT, GR, GR, GR, GR, GR, BL, WT},
{    WT, BL, GR, GR, GR, GR, WT, WT, WT, WT, WT, WT, GR, GR, GR, GR, BL, WT},
{    WT, BL, GR, GR, GR, WT, WT, WT, GR, GR, WT, WT, WT, GR, GR, GR, BL, WT},
{    BL, GR, GR, GR, GR, GR, WT, GR, GR, GR, GR, WT, WT, GR, GR, GR, GR, BL},
{    BL, GR, GR, GR, GR, GR, GR, GR, GR, GR, WT, WT, WT, GR, GR, GR, GR, BL},
{    BL, GR, GR, GR, GR, GR, GR, GR, GR, WT, WT, WT, GR, GR, GR, GR, GR, BL},
{    BL, GR, GR, GR, GR, GR, GR, GR, WT, WT, WT, GR, GR, GR, GR, GR, GR, BL},
{    WT, BL, GR, GR, GR, GR, GR, GR, WT, WT, GR, GR, GR, GR, GR, GR, BL, WT},
{    WT, BL, GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, BL, WT},
{    WT, BL, GR, GR, GR, GR, GR, GR, WT, WT, GR, GR, GR, GR, GR, GR, BL, WT},
{    WT, WT, BL, GR, GR, GR, GR, GR, WT, WT, GR, GR, GR, GR, GR, BL, WT, WT},
{    WT, WT, WT, BL, GR, GR, GR, GR, GR, GR, GR, GR, GR, GR, BL, WT, WT, WT},
{    WT, WT, WT, WT, BL, BL, BL, GR, GR, GR, GR, BL, BL, BL, WT, WT, WT, WT},
{    WT, WT, WT, WT, WT, WT, WT, BL, BL, BL, BL, WT, WT, WT, WT, WT, WT, WT}
};
//BITMAP MsgBoxQuestionBitmap = {(unsigned int) MsgBoxQuestion, 18, 18, 0, 0};

unsigned char MsgBoxWarning[18][18] =
{
{    WT, WT, WT, WT, WT, WT, WT, WT, BL, BL, WT, WT, WT, WT, WT, WT, WT, WT},
{    WT, WT, WT, WT, WT, WT, WT, BL, YL, YL, BL, WT, WT, WT, WT, WT, WT, WT},
{    WT, WT, WT, WT, WT, WT, WT, BL, YL, YL, BL, WT, WT, WT, WT, WT, WT, WT},
{    WT, WT, WT, WT, WT, WT, BL, YL, YL, YL, YL, BL, WT, WT, WT, WT, WT, WT},
{    WT, WT, WT, WT, WT, WT, BL, YL, BL, BL, YL, BL, WT, WT, WT, WT, WT, WT},
{    WT, WT, WT, WT, WT, BL, YL, YL, BL, BL, YL, YL, BL, WT, WT, WT, WT, WT},
{    WT, WT, WT, WT, WT, BL, YL, BL, BL, BL, BL, YL, BL, WT, WT, WT, WT, WT},
{    WT, WT, WT, WT, BL, YL, YL, BL, BL, BL, BL, YL, YL, BL, WT, WT, WT, WT},
{    WT, WT, WT, WT, BL, YL, YL, BL, BL, BL, BL, YL, YL, BL, WT, WT, WT, WT},
{    WT, WT, WT, BL, YL, YL, YL, BL, BL, BL, BL, YL, YL, YL, BL, WT, WT, WT},
{    WT, WT, WT, BL, YL, YL, YL, YL, BL, BL, YL, YL, YL, YL, BL, WT, WT, WT},
{    WT, WT, BL, YL, YL, YL, YL, YL, BL, BL, YL, YL, YL, YL, YL, BL, WT, WT},
{    WT, WT, BL, YL, YL, YL, YL, YL, BL, BL, YL, YL, YL, YL, YL, BL, WT, WT},
{    WT, BL, YL, YL, YL, YL, YL, YL, YL, YL, YL, YL, YL, YL, YL, YL, BL, WT},
{    WT, BL, YL, YL, YL, YL, YL, YL, BL, BL, YL, YL, YL, YL, YL, YL, BL, WT},
{    BL, YL, YL, YL, YL, YL, YL, YL, BL, BL, YL, YL, YL, YL, YL, YL, YL, BL},
{    BL, YL, YL, YL, YL, YL, YL, YL, YL, YL, YL, YL, YL, YL, YL, YL, YL, BL},
{    WT, BL, BL, BL, BL, BL, BL, BL, BL, BL, BL, BL, BL, BL, BL, BL, BL, WT}
};
//BITMAP MsgBoxWarningBitmap = {(unsigned int) MsgBoxWarning, 18, 18, 0, 0};

unsigned char MsgBoxInformation[18][18] =
{
{    WT, WT, WT, WT, WT, WT, BL, BL, BL, BL, BL, BL, WT, WT, WT, WT, WT, WT},
{    WT, WT, WT, WT, BL, BL, WT, WT, WT, WT, WT, WT, BL, BL, WT, WT, WT, WT},
{    WT, WT, WT, BL, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, BL, WT, WT, WT},
{    WT, WT, BL, WT, WT, WT, WT, WT, 0x09, 0x09, WT, WT, WT, WT, WT, BL, WT, WT},
{    WT, BL, WT, WT, WT, WT, WT, WT, 0x09, 0x09, WT, WT, WT, WT, WT, WT, BL, WT},
{    WT, BL, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, BL, WT},
{    BL, WT, WT, WT, WT, WT, WT, 0x09, 0x09, 0x09, WT, WT, WT, WT, WT, WT, WT, BL},
{    BL, WT, WT, WT, WT, WT, WT, WT, 0x09, 0x09, WT, WT, WT, WT, WT, WT, WT, BL},
{    BL, WT, WT, WT, WT, WT, WT, WT, 0x09, 0x09, WT, WT, WT, WT, WT, WT, WT, BL},
{    WT, BL, WT, WT, WT, WT, WT, WT, 0x09, 0x09, WT, WT, WT, WT, WT, WT, BL, WT},
{    WT, BL, WT, WT, WT, WT, WT, WT, 0x09, 0x09, WT, WT, WT, WT, WT, WT, BL, WT},
{    WT, WT, BL, WT, WT, WT, WT, 0x09, 0x09, 0x09, 0x09, WT, WT, WT, WT, BL, WT, WT},
{    WT, WT, WT, BL, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, BL, WT, WT, WT},
{    WT, WT, WT, WT, BL, BL, WT, WT, WT, WT, WT, WT, WT, WT, BL, WT, WT, WT},
{    WT, WT, WT, WT, WT, WT, BL, BL, BL, BL, BL, BL, BL, WT, WT, BL, WT, WT},
{    WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, BL, WT, BL, WT, WT},
{    WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, BL, WT, BL, WT},
{    WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, BL, BL, WT}
};

unsigned char MsgBoxError[18][18] =
{
{    WT, WT, WT, WT, WT, WT, WT, BL, BL, BL, BL, WT, WT, WT, WT, WT, WT, WT},
{    WT, WT, WT, WT, BL, BL, BL, 12, 12, 12, 12, BL, BL, BL, WT, WT, WT, WT},
{    WT, WT, WT, BL, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, BL, WT, WT, WT},
{    WT, WT, BL, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, BL, WT, WT},
{    WT, BL, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, BL, WT},
{    WT, BL, 12, 12, 12, 12, WT, 12, 12, 12, 12, WT, 12, 12, 12, 12, BL, WT},
{    WT, BL, 12, 12, 12, WT, WT, WT, 12, 12, WT, WT, WT, 12, 12, 12, BL, WT},
{    BL, 12, 12, 12, 12, 12, WT, WT, WT, WT, WT, WT, 12, 12, 12, 12, 12, BL},
{    BL, 12, 12, 12, 12, 12, 12, WT, WT, WT, WT, 12, 12, 12, 12, 12, 12, BL},
{    BL, 12, 12, 12, 12, 12, 12, WT, WT, WT, WT, 12, 12, 12, 12, 12, 12, BL},
{    BL, 12, 12, 12, 12, 12, WT, WT, WT, WT, WT, WT, 12, 12, 12, 12, 12, BL},
{    WT, BL, 12, 12, 12, WT, WT, WT, 12, 12, WT, WT, WT, 12, 12, 12, BL, WT},
{    WT, BL, 12, 12, 12, 12, WT, 12, 12, 12, 12, WT, 12, 12, 12, 12, BL, WT},
{    WT, BL, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, BL, WT},
{    WT, WT, BL, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, BL, WT, WT},
{    WT, WT, WT, BL, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, BL, WT, WT, WT},
{    WT, WT, WT, WT, BL, BL, BL, 12, 12, 12, 12, BL, BL, BL, WT, WT, WT, WT},
{    WT, WT, WT, WT, WT, WT, WT, BL, BL, BL, BL, WT, WT, WT, WT, WT, WT, WT}
};
//BITMAP MsgBoxErrorBitmap = {(unsigned int) MsgBoxError, 18, 18, 0, 0};

/////////////////////////////////////////////////////////////////


#endif
