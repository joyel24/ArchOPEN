/*
* kernel/gui/gfx/shell/shell.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <lib/string.h>
#include <sys_def/stddef.h>
#include <sys_def/colordef.h>

#include <kernel/kernel.h>
#include <kernel/malloc.h>
#include <kernel/evt.h>
#include <kernel/thread.h>

#include <fs/med.h>
#include <fs/stdfs.h>
#include <fs/cfg_file.h>

#include <gfx/graphics.h>
#include <gfx/screens.h>

#include <gui/shell.h>
#include <gui/icons.h>
#include <gui/msgBox.h>
#include <gui/file_browser.h>
#include <gui/status_line.h>
#include <gui/shellmenu.h>
#include <gui/internal_commands.h>
#include <gui/settings_energy.h>
#include <gui/player.h>
#include <gui/playlistmenu.h>
#include <gui/settings_bgImg.h>

#include <snd/codec.h>
#include <snd/sound.h>

#include <driver/lcd.h>

#include <init/boot_error.h>

typedef struct SHELL_HANDLER_STRUCT * SHELL_HANDLER;

struct SHELL_HANDLER_STRUCT {
    char * ext;
    char * handler;

    SHELL_HANDLER next;
};

SHELL_HANDLER shell_firstHandler = NULL;
SHELL_HANDLER shell_lastHandler = NULL;

int shell_eventHandler = -1;

bool shell_isInitialized = false;

static void shell_loop(){
    int event;

    for(;;){
        event=evt_getStatusBlocking(shell_eventHandler);
        if (event==NO_EVENT){ // no new events
            yield();
            continue;
        }

        // event handlers
        shellMenu_handleEvent(event);
        statusLine_handleEvent(event);
    };
}

static SHELL_HANDLER shell_addHandler(){
    SHELL_HANDLER handler;

    // alloc memory
    handler=malloc(sizeof(*handler));

    // init members
    handler->ext=NULL;
    handler->handler=NULL;
    handler->next=NULL;

    // handle linked list
    if(shell_firstHandler==NULL) shell_firstHandler=handler;
    if(shell_lastHandler!=NULL) shell_lastHandler->next=handler;
    shell_lastHandler=handler;

    return handler;
}

static bool shell_parseHandlers(char * filename){
    char * name;
    char * value;
    char * ext = NULL;
    SHELL_HANDLER handler;
    CFG_DATA * data;

    data=cfg_readFile(filename);
    if(data==NULL)
    {
        gui_bootError(MISSING_HANDLER_FILE_ERROR,BOOT_ERROR);
        return false;
    }

    printk("[shell] parsing '%s'\n",filename);

    cfg_rewindItems(data);

    while(cfg_nextItem(data,&name,&value)){
        if(!strcmp(name,"ext")){
            ext=strdup(value);
            strlwr(ext);

        }else if(!strcmp(name,"handler")){

            if (ext==NULL){
                printk("[shell] parse error: 'ext' required before 'handler'\n",name);
            }else{

                // make sure ext is not already registered
                handler=shell_firstHandler;
                while(handler){
                    if(!strcmp(ext,handler->ext)){
                        printk("[shell] parse error: handler for '%s' is already registered\n",ext);
                        break;
                    }

                    handler=handler->next;
                }

                // register if not already registered
                if(handler==NULL){
                    handler=shell_addHandler();

                    handler->ext=ext;
                    handler->handler=strdup(value);
                }

                ext=NULL;
            }
        }else{
            printk("[shell] parse error: unknown parameter '%s'\n",name);
        }
    }

    cfg_clear(data);

    return true;
}

static bool shell_executeMed(char * medfile,char * param){
    char * argv[2];
    int argc;
    bool retval=true;

    printk("[shell] running MED file '%s', param='%s'\n",medfile,(param)?param:"");

    //build arguments
    argc=1;
    argv[0]=medfile;
    if(param!=NULL){
        argc++;
        argv[1]=param;
    }

    //launch med
    if(med_loadParam(argc,argv)<0){
        retval=false;
    }

    //prepare return to shell
    gfx_openGraphics();
    bgImg_loadPref();

    return retval;
}


void shell_printHandlers(){
    SHELL_HANDLER h;

    h=shell_firstHandler;
    while(h){
        printk("* ext='%s' handler='%s'\n",h->ext,h->handler);
        h=h->next;
    }
}

bool shell_execute(char * command,char * param){
    bool retval=true;
    char * s = malloc(256);

    printk("[shell] executing '%s', param='%s'\n",command,(param)?param:"");

    //feedback
    sprintf(s,"Opening %s",command);
    msgBox_show("MediOS",s,MSGBOX_TYPE_INFO,MSGBOX_ICON_INFORMATION,shell_eventHandler);

    // internal command ?
    if(strstr(command,"internal:")==command){
        char * intcmd;

        intcmd=command+9; // "internal:" length

        // execute internal command
        if(!intCmd_execute(intcmd,param)){
            retval=false;
        }

    }else{
        char * ext;

        // get extension
        ext=command+strlen(command)-1;
        while(ext>=command && *(ext-1)!='.'){
            --ext;
        }

        if (ext<command){
            printk("[shell] error: can't execute '%s', no extension\n",command);
            retval=false;
        }else{

            ext=strdup(ext);
            strlwr(ext);

            // med file?
            if(!strcmp(ext,"med")){
                // execute the med file
                retval=shell_executeMed(command,param);

            }else{
                
                if(codec_findCodecFor(command)!=NULL){

                    intCmd_execute("player",command);

                }else{

                    SHELL_HANDLER handler;
    
                    // find the extension handler
                    handler=shell_firstHandler;
                    while(handler!=NULL){
                        if(!strcmp(ext,handler->ext)){
                            break;
                        }
    
                        handler=handler->next;
                    }
    
                    //found a handler?
                    if(handler!=NULL){
                        //execute handler with the file as param
                        retval=shell_execute(handler->handler,command);

                    }else{
                        printk("[shell] error: can't execute '%s', no handler for extension '%s'\n",command,ext);
                        retval=false;
                    }
                }
            }

            kfree(ext);

        }
    }

    //restore shell screen
    shell_restore();

    if(!retval){
        //feedback
        sprintf(s,"Couldn't open %s",command);
        msgBox_show("MediOS - Error",s,MSGBOX_TYPE_OK,MSGBOX_ICON_ERROR,shell_eventHandler);
        shell_restore();
    }

    kfree(s);

    return retval;
}

void shell_restore(){
    struct evt_t evt;

    //purge events
    evt_purgeHandler(shell_eventHandler);

    //redraw shell
    evt.evt=EVT_REDRAW;
    evt.evt_class=GUI_CLASS;
    evt.data=NULL;
    evt_send(&evt);
}

void shell_main(){

    gfx_initGraphics();    
    gfx_clearScreen(COLOR_TRSP);

    icon_init();
    lcd_brightIni();
    msgBox_init();
    iniBrowser();

    shell_eventHandler=evt_getHandler(ALL_CLASS);

    //parse handlers file
    if(!shell_parseHandlers(SHELL_HANDLERS_FILE)){
        printk("[shell] error reading handlers file\n");
    }
    // init shell menu
    if (shellMenu_init()){

        // init status line
        printk("[shell] Status line init\n");
        statusLine_init();

        // init player & playlist
        printk("[shell] Player init\n");
        player_init();
        printk("[shell] Playlist init\n");
        playlistMenu_init();

        shell_isInitialized=true;

        printk("[shell] Initial paint\n");
        
        // initial paint
        shell_restore();

        screens_show(SCREEN_GFX);
        bgImg_loadPref();
        printk("nb Tick needed for boot %d\n",tick);
        // main loop (never returns)
        shell_loop();
    }
}


void shell_close(){
    SHELL_HANDLER h;
    SHELL_HANDLER prev;

    printk("[shell] closing\n");

    if(shell_isInitialized){

        player_close();
        playlistMenu_close();
    
        shellMenu_close();
    
        gfx_closeGraphics();
        evt_freeHandler(shell_eventHandler);
    
        // free handlers
        h=shell_firstHandler;
        while(h!=NULL){
            // free strings
            if(h->ext!=NULL) kfree(h->ext);
            if(h->handler!=NULL) kfree(h->handler);
            // get next item pointer before freeing item
            prev=h;
    
            h=h->next;
    
            kfree(prev);
        }
    }
}
