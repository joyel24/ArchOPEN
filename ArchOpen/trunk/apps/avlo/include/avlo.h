/*
*   apps/avlo/include/avlo.h
*
*   Avlo a loader for Jbmm, av1xx and av3xx
*   ArchOpen Project
*   Copyright (c) 2007 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __AVLO_H_
#define __AVLO_H_

#define VERSION "V5.1"

/*
Common functions
*/

void chkOFF(int key);
void drawMenu(int nb);
void drawBox(int txt_width,int txt_height,int * start_x,int * start_y);
void waitKey(void);
void waitKeyReleased(int has_time_out);
void affUSB();
void avlo_drawBat();
void moveCursor(int direction,int nbCfg);
int  processDefault(int key,int nbCfg);
void printErr(int key);
int fastLoadCJBM(char * filename);
int loadFile(char * fileN,char* buffer,int prog);
void init_colors(void);
void doFault(int faultNum);

void resetMem(unsigned int ptr,unsigned int cfg);

/* arch sp�cifique */
void printIniLevel(int level);
void HD_init_string(void);
void readingConfString(void);
void USBEnableString(void);
void printFault(int faultNum);
void shutDownString(void);
void drawProgress(int offset,int length,int mode);
void USB_connDisp(void);
void USB_connHide(void);

#endif
