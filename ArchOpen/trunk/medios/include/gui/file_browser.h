/*
* include/gui/file_browser.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __GUI_FILE_BROWSER_H
#define __GUI_FILE_BROWSER_H

#include <gui/scrollbar.h>
#include <sys_def/stdfs.h>

#define TYPE_STD      0
#define TYPE_BACK     1
#define TYPE_DIR      2
#define TYPE_FILE     3

#define TYPE_INTERNAL 4

#define LEFT_SCROLL  0
#define RIGHT_SCROLL 1

#define PATHLEN    MAX_PATH

#define MODE_SELECT   0
#define MODE_NOSELECT 1
#define MODE_STRING   2

extern int browser_has_back_entry;
extern int browser_scroll_only_selected;
extern int browser_has_statusbar;
extern int txt_scroll_speed;

struct dir_entry {
    char * name;
    int name_size;
    char * cur_name;
    int type;
    int size;
    int selected;
    int inc;
};

struct browser_data {

    char path[PATHLEN];

    int pos;
    int nselect;

    int show_dot_files;

    int scroll_pos;

    int nbFile;
    int nbDir;
    int totSize;

    int nb_disp_entry;
    int max_entry_length;
    int x_start;
    int y_start;

    int width;
    int height;

    int entry_height;
    
    int mode;
    int font;

    struct dir_entry * list;
    int                listused;
    int                listsize;

    char * retPath;
    
    int is_dual;
    int dual_mode;
    struct browser_data * dual;
    
    struct scroll_bar browser_scroll;
    
    
};

#define    BROWSER_STATUS_HEIGHT     20
#define    BROWSER_ICON_WIDTH        11
#define    BROWSER_SCROLLBAR_WIDTH   10

int browser_simpleBrowse(char * path,char * res);
int browser_browse(struct browser_data *bdata,char * path,char * res);
void browser_setPath(char * path,struct browser_data *bdata);

void browser_disposeBrowse(struct browser_data * bdata);
struct browser_data * browser_NewBrowse(void);
struct browser_data * browser_NewDualBrowse(void);

/*****    ls_fct    *****/
int  qSortEntry  (const void * a1,const void * a2);
void cleanList   (struct browser_data * bdata);
int  ini_lists   (struct browser_data * bdata);
int  addEntry    (struct browser_data * bdata,char * name,int type,int size);
int  doLs        (struct browser_data * bdata);
void chgSelect   (struct browser_data *bdata,int num);
int  nbSelected  (struct browser_data * bdata);
struct dir_entry * nxtSelect(struct browser_data * bdata,int * pos);

int upDir(struct browser_data * bdata);
int inDir(struct browser_data * bdata,char * name);
char * currentDir(struct browser_data * bdata);
int isRoot(struct browser_data * bdata);
int findName(struct browser_data * bdata,char * name);

/*****    evt_handle_fct    *****/
int  browserEvt         (struct browser_data * bdata);

/*****    gui_fct    *****/
void iniBrowser         (void);
int  viewNewDir         (struct browser_data *bdata,char *name,char * oldName);
void browser_doDraw     (struct browser_data *bdata);
void printName          (struct dir_entry * dEntry,int pos,int clear,int selected,struct browser_data *bdata);
void printAllName       (struct browser_data *bdata);
void printAName         (struct browser_data *bdata,int pos, int nselect, int clear, int selected);
void printLongName      (int pos,int relPos,int selected,struct browser_data *bdata);
void createSizeString   (char * str,int Isize);
void clearBrowser       (struct browser_data *bdata);
void redrawBrowser      (struct browser_data *bdata);
void browser_screenSize (struct browser_data *bdata);
void browser_computeSize(struct browser_data *bdata);
int  browser_loadFoler  (struct browser_data *bdata,char * path);
/* main fct */
void  bwseventHandler       (int evt);

/**** drawings        ******/
void draw_bottom_status  (struct browser_data *bdata);
void draw_file_size      (struct browser_data *bdata, struct dir_entry * entry);
void clear_status(struct browser_data *bdata);

//char *  browse     (char * path,int mode);
//void ini_file_browser  (void);

/*** Conctextual Menu ****/
void browser_mkMenu(struct browser_data * bdata);
void browser_contMenu(struct browser_data * bdata);


#define BROWSE_IS_DUAL      (bdata->is_dual!=0 && bdata->dual_mode!=0 && bdata->dual != NULL)
#define BROWSER_IS_FOCUSED  ((BROWSE_IS_DUAL && (bdata->is_dual==bdata->dual_mode))||!BROWSE_IS_DUAL)
#endif
