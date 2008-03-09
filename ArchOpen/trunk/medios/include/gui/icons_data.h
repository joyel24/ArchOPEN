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

#include <sys_def/colordef.h>

/* color def */
#define BR COLOR_BROWN
#define BL COLOR_BLACK
#define WT COLOR_WHITE
#define BG COLOR_TRSP
#define YL COLOR_YELLOW
#define GR COLOR_GREEN
#define GA COLOR_GRAY
#define LB COLOR_LIGHT_BLUE
#define DB COLOR_DARK_BLUE
#define RD COLOR_RED
#define BE COLOR_BLUE
#define LG COLOR_LIGHT_GRAY

unsigned char medios_logo[10][51] = {
{LB,DB,DB,DB,DB,LB,LB,LB,LB,DB,DB,DB,DB,LB,LB,LB,DB,DB,DB,DB,LB,LB,DB,DB,DB,DB,DB,DB,LB,LB,LB,DB,DB,DB,LB,LB,LB,DB,DB,DB,DB,DB,LB,LB,LB,LB,LB,DB,DB,DB,DB},
{DB,DB,DB,DB,DB,DB,LB,LB,DB,DB,DB,DB,DB,DB,LB,DB,DB,DB,DB,DB,DB,LB,DB,DB,DB,DB,DB,DB,DB,LB,LB,DB,DB,DB,LB,LB,DB,DB,DB,DB,DB,DB,DB,LB,LB,LB,DB,DB,DB,DB,DB},
{DB,DB,DB,DB,DB,DB,DB,DB,DB,DB,DB,DB,DB,DB,LB,DB,DB,DB,DB,DB,DB,LB,DB,DB,DB,LB,LB,DB,DB,DB,LB,DB,DB,DB,LB,DB,DB,DB,DB,DB,DB,DB,DB,DB,LB,DB,DB,DB,LB,LB,LB},
{DB,DB,DB,LB,DB,DB,DB,DB,DB,DB,LB,DB,DB,DB,LB,DB,DB,DB,LB,LB,LB,LB,DB,DB,DB,LB,LB,LB,DB,DB,LB,DB,DB,DB,LB,DB,DB,DB,LB,LB,LB,DB,DB,DB,LB,DB,DB,DB,LB,LB,LB},
{DB,DB,DB,LB,LB,DB,DB,DB,DB,LB,LB,DB,DB,DB,LB,DB,DB,DB,DB,DB,DB,LB,DB,DB,DB,LB,LB,LB,DB,DB,LB,DB,DB,DB,LB,DB,DB,DB,LB,LB,LB,DB,DB,DB,LB,LB,DB,DB,DB,LB,LB},
{DB,DB,DB,LB,LB,LB,DB,DB,LB,LB,LB,DB,DB,DB,LB,DB,DB,DB,DB,DB,DB,LB,DB,DB,DB,LB,LB,LB,DB,DB,LB,DB,DB,DB,LB,DB,DB,DB,LB,LB,LB,DB,DB,DB,LB,LB,LB,DB,DB,DB,LB},
{DB,DB,DB,LB,LB,LB,LB,LB,LB,LB,LB,DB,DB,DB,LB,DB,DB,DB,LB,LB,LB,LB,DB,DB,DB,LB,LB,LB,DB,DB,LB,DB,DB,DB,LB,DB,DB,DB,LB,LB,LB,DB,DB,DB,LB,LB,LB,LB,DB,DB,DB},
{DB,DB,DB,LB,LB,LB,LB,LB,LB,LB,LB,DB,DB,DB,LB,DB,DB,DB,DB,DB,DB,LB,DB,DB,DB,LB,LB,DB,DB,DB,LB,DB,DB,DB,LB,DB,DB,DB,DB,DB,DB,DB,DB,DB,LB,LB,LB,LB,DB,DB,DB},
{DB,DB,DB,LB,LB,LB,LB,LB,LB,LB,LB,DB,DB,DB,LB,DB,DB,DB,DB,DB,DB,LB,DB,DB,DB,DB,DB,DB,DB,LB,LB,DB,DB,DB,LB,LB,DB,DB,DB,DB,DB,DB,DB,LB,LB,DB,DB,DB,DB,DB,LB},
{DB,DB,DB,LB,LB,LB,LB,LB,LB,LB,LB,DB,DB,DB,LB,LB,DB,DB,DB,DB,LB,LB,DB,DB,DB,DB,DB,DB,LB,LB,LB,DB,DB,DB,LB,LB,LB,DB,DB,DB,DB,DB,LB,LB,LB,DB,DB,DB,DB,LB,LB} };

/************************
 * 15x6 USB status icon */
unsigned char usb_icon[6][15] = {
{LB,LB,LB,LB,LB,DB,DB,DB,DB,DB,DB,LB,LB,LB,LB},
{LB,LB,LB,LB,DB,DB,DB,DB,DB,DB,DB,DB,DB,DB,DB},
{DB,DB,DB,DB,DB,DB,DB,DB,DB,DB,DB,LB,DB,LB,DB},
{DB,DB,DB,DB,DB,DB,DB,DB,DB,DB,DB,LB,DB,LB,DB},
{LB,LB,LB,LB,DB,DB,DB,DB,DB,DB,DB,DB,DB,DB,DB},
{LB,LB,LB,LB,LB,DB,DB,DB,DB,DB,DB,LB,LB,LB,LB} };
//BITMAP usbIcon = {(unsigned int) usb_icon, 15, 6, 0, 0};

/************************
 * 15x6 FW status icon */
unsigned char fw_ext_icon[6][15] = {
{LB,LB,LB,DB,DB,LB,LB,LB,LB,LB,DB,DB,LB,LB,LB},
{LB,LB,LB,LB,DB,DB,LB,LB,LB,DB,DB,LB,LB,LB,LB},
{LB,LB,LB,LB,LB,DB,DB,LB,DB,DB,LB,LB,LB,LB,LB},
{LB,LB,LB,LB,LB,LB,DB,DB,DB,LB,LB,LB,LB,LB,LB},
{LB,LB,LB,LB,LB,LB,DB,DB,DB,LB,LB,LB,LB,LB,LB},
{LB,LB,LB,LB,LB,LB,DB,DB,DB,LB,LB,LB,LB,LB,LB} };
//BITMAP fwExtIcon = {(unsigned int) fw_ext_icon, 15, 6, 0, 0};

/************************
 * 15x6 internal HP status icon */
unsigned char intHP_icon[10][8] = {
{LB,LB,LB,LB,LB,LB,LB,BL},
{LB,LB,LB,LB,LB,LB,BL,BL},
{BL,BL,BL,LB,LB,BL,GR,BL},
{BL,GA,BL,LB,BL,GR,GR,BL},
{BL,GA,BL,BL,GR,GR,GR,BL},
{BL,GA,BL,BL,GR,GR,GR,BL},
{BL,GA,BL,LB,BL,GR,GR,BL},
{BL,BL,BL,LB,LB,BL,GR,BL},
{LB,LB,LB,LB,LB,LB,BL,BL},
{LB,LB,LB,LB,LB,LB,LB,BL}};    

/************************
 * 15x6 CF status icon */
unsigned char cf_icon[6][15] = {
{LB,LB,DB,DB,DB,LB,LB,LB,DB,DB,DB,DB,LB,LB,LB},
{LB,DB,DB,DB,DB,LB,LB,LB,DB,DB,DB,LB,LB,LB,LB},
{DB,DB,LB,LB,LB,LB,LB,DB,DB,LB,LB,LB,LB,LB,LB},
{DB,DB,LB,LB,LB,LB,LB,DB,DB,DB,DB,LB,LB,LB,LB},
{LB,DB,DB,DB,DB,LB,DB,DB,DB,DB,LB,LB,LB,LB,LB},
{LB,LB,DB,DB,DB,LB,DB,DB,LB,LB,LB,LB,LB,LB,LB} };
//BITMAP cfIcon = {(unsigned int) cf_icon, 15, 6, 0, 0};


/**************************
 * 13x6 Power status icon */
unsigned char power_icon[6][13] = {
{LB,LB,LB,LB,LB,DB,DB,DB,DB,LB,LB,LB,LB},
{LB,LB,LB,LB,DB,DB,DB,DB,DB,DB,DB,DB,DB},
{DB,DB,DB,DB,DB,DB,DB,DB,DB,LB,LB,LB,LB},
{DB,DB,DB,DB,DB,DB,DB,DB,DB,LB,LB,LB,LB},
{LB,LB,LB,LB,DB,DB,DB,DB,DB,DB,DB,DB,DB},
{LB,LB,LB,LB,LB,DB,DB,DB,DB,LB,LB,LB,LB} };
//BITMAP powerIcon = {(unsigned int) power_icon, 13, 6, 0, 0};

/*********************
 * 9x9 Up Arrow icon */
unsigned char upArrow[9][9] =
{   {GA,GA,GA,GA,BL,GA,GA,GA,GA},
    {GA,GA,GA,BL,BL,BL,GA,GA,GA},
    {GA,GA,BL,GA,BL,GA,BL,GA,GA},
    {GA,BL,GA,GA,BL,GA,GA,BL,GA},
    {GA,GA,GA,GA,BL,GA,GA,GA,GA},
    {GA,GA,GA,GA,BL,GA,GA,GA,GA},
    {GA,GA,GA,GA,BL,GA,GA,GA,GA},
    {GA,GA,GA,GA,BL,GA,GA,GA,GA},
    {GA,GA,GA,GA,BL,GA,GA,GA,GA},
};
//BITMAP upBitmap = {(unsigned int) upArrow, 9, 9, 0, 0};

/***********************
 * 9x9 Down Arrow icon */
unsigned char dwArrow[9][9] =
{   {GA,GA,GA,GA,BL,GA,GA,GA,GA},
    {GA,GA,GA,GA,BL,GA,GA,GA,GA},
    {GA,GA,GA,GA,BL,GA,GA,GA,GA},
    {GA,GA,GA,GA,BL,GA,GA,GA,GA},
    {GA,GA,GA,GA,BL,GA,GA,GA,GA},
    {GA,BL,GA,GA,BL,GA,GA,BL,GA},
    {GA,GA,BL,GA,BL,GA,BL,GA,GA},
    {GA,GA,GA,BL,BL,BL,GA,GA,GA},
    {GA,GA,GA,GA,BL,GA,GA,GA,GA},
};
//BITMAP dwBitmap = {(unsigned int) dwArrow, 9, 9, 0, 0};

/**********************
 * 8x8 Directory icon */
unsigned char dir[8][8] =
{   {BL,BL,BL,WT,WT,WT,WT,WT},
    {BL,BR,BL,BL,BL,WT,WT,WT},
    {BL,BR,BR,BR,BL,WT,WT,WT},
    {BL,BR,BR,BR,BL,WT,WT,WT},
    {BL,BR,BR,BR,BL,BR,BR,BL},
    {BL,BR,BR,BR,BL,BR,BL,WT},
    {BL,BR,BR,BR,BL,BL,WT,WT},
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
    {WT,BL,BL,WT,WT,BL,BR,BL},
    {BL,BR,BL,WT,WT,WT,BL,WT},
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
    {BL,LG,BL,BR,BR,BL,LG,BL},
    {BL,LG,BL,BR,BR,BL,LG,BL},
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
{    WT, WT, BL, WT, WT, WT, WT, WT, BE, BE, WT, WT, WT, WT, WT, BL, WT, WT},
{    WT, BL, WT, WT, WT, WT, WT, WT, BE, BE, WT, WT, WT, WT, WT, WT, BL, WT},
{    WT, BL, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, WT, BL, WT},
{    BL, WT, WT, WT, WT, WT, WT, BE, BE, BE, WT, WT, WT, WT, WT, WT, WT, BL},
{    BL, WT, WT, WT, WT, WT, WT, WT, BE, BE, WT, WT, WT, WT, WT, WT, WT, BL},
{    BL, WT, WT, WT, WT, WT, WT, WT, BE, BE, WT, WT, WT, WT, WT, WT, WT, BL},
{    WT, BL, WT, WT, WT, WT, WT, WT, BE, BE, WT, WT, WT, WT, WT, WT, BL, WT},
{    WT, BL, WT, WT, WT, WT, WT, WT, BE, BE, WT, WT, WT, WT, WT, WT, BL, WT},
{    WT, WT, BL, WT, WT, WT, WT, BE, BE, BE, BE, WT, WT, WT, WT, BL, WT, WT},
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
{    WT, WT, WT, WT, BL, BL, BL, RD, RD, RD, RD, BL, BL, BL, WT, WT, WT, WT},
{    WT, WT, WT, BL, RD, RD, RD, RD, RD, RD, RD, RD, RD, RD, BL, WT, WT, WT},
{    WT, WT, BL, RD, RD, RD, RD, RD, RD, RD, RD, RD, RD, RD, RD, BL, WT, WT},
{    WT, BL, RD, RD, RD, RD, RD, RD, RD, RD, RD, RD, RD, RD, RD, RD, BL, WT},
{    WT, BL, RD, RD, RD, RD, WT, RD, RD, RD, RD, WT, RD, RD, RD, RD, BL, WT},
{    WT, BL, RD, RD, RD, WT, WT, WT, RD, RD, WT, WT, WT, RD, RD, RD, BL, WT},
{    BL, RD, RD, RD, RD, RD, WT, WT, WT, WT, WT, WT, RD, RD, RD, RD, RD, BL},
{    BL, RD, RD, RD, RD, RD, RD, WT, WT, WT, WT, RD, RD, RD, RD, RD, RD, BL},
{    BL, RD, RD, RD, RD, RD, RD, WT, WT, WT, WT, RD, RD, RD, RD, RD, RD, BL},
{    BL, RD, RD, RD, RD, RD, WT, WT, WT, WT, WT, WT, RD, RD, RD, RD, RD, BL},
{    WT, BL, RD, RD, RD, WT, WT, WT, RD, RD, WT, WT, WT, RD, RD, RD, BL, WT},
{    WT, BL, RD, RD, RD, RD, WT, RD, RD, RD, RD, WT, RD, RD, RD, RD, BL, WT},
{    WT, BL, RD, RD, RD, RD, RD, RD, RD, RD, RD, RD, RD, RD, RD, RD, BL, WT},
{    WT, WT, BL, RD, RD, RD, RD, RD, RD, RD, RD, RD, RD, RD, RD, BL, WT, WT},
{    WT, WT, WT, BL, RD, RD, RD, RD, RD, RD, RD, RD, RD, RD, BL, WT, WT, WT},
{    WT, WT, WT, WT, BL, BL, BL, RD, RD, RD, RD, BL, BL, BL, WT, WT, WT, WT},
{    WT, WT, WT, WT, WT, WT, WT, BL, BL, BL, BL, WT, WT, WT, WT, WT, WT, WT}
};
//BITMAP MsgBoxErrorBitmap = {(unsigned int) MsgBoxError, 18, 18, 0, 0};

/////////////////////////////////////////////////////////////////


#endif
