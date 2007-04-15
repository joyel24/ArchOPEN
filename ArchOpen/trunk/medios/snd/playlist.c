/*
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

#include <kernel/kernel.h>
#include <kernel/malloc.h>
#include <fs/stdfs.h>

#include <snd/codec.h>

#include <snd/playlist.h>

PLAYLIST_ITEM * playlist_first = NULL;
PLAYLIST_ITEM * playlist_last = NULL;

int playlist_count=0;

PLAYLIST_ITEM * playlist_new(){
    PLAYLIST_ITEM * item;
    PLAYLIST_ITEM * prevItem;

    playlist_count++;

    // alloc data
    item=malloc(sizeof(PLAYLIST_ITEM));

    // add to linked list
    prevItem=playlist_last;

    if(prevItem==NULL){
        playlist_first=item;
    }else{
        prevItem->next=item;
    }

    item->name=NULL;
    item->startPos=-1;
    item->fileSize=0;
    item->bufferedSize=0;

    item->prev=prevItem;
    item->next=NULL;

    playlist_last=item;

    return item;
}

bool playlist_remove(PLAYLIST_ITEM * item){

    if(item==NULL) return false;

    playlist_count--;

    // link previous and next items
    if(item->prev!=NULL){
        item->prev->next=item->next;
    }

    if(item->next!=NULL){
        item->next->prev=item->prev;
    }

    // reajust first or last if necessary
    if(item==playlist_first){
        playlist_first=item->next;
    }

    if(item==playlist_last){
        playlist_last=item->prev;
    }

    // free data
    if(item->name!=NULL) free(item->name);
    free(item);
    
    return true;
}

void playlist_clear(){
    while(playlist_last!=NULL){
        playlist_remove(playlist_last);
    }
}

bool playlist_addFile(char * name){
    int f;
    PLAYLIST_ITEM * item;

    f=open(name,O_RDONLY);

    if (f<0) return false;

    item=playlist_new();

    item->name=strdup(name);
    item->fileSize=filesize(f);

    //TODO: tags stuff

    close(f);

    return true;
}

bool playlist_addFolder(char * name,bool recurse){
    DIR * dir;
    struct dirent * ent;
    char * fname;

    dir=opendir(name);

    if (dir==NULL){
        return false;
    }

    fname=malloc(MAX_PATH);

    for(;;){
        ent=readdir(dir);

        if(ent==NULL){
            break;
        }

        if(!strcmp(ent->d_name,".") || !strcmp(ent->d_name,"..")){
            continue;
        }

        sprintf(fname,"%s/%s",name,ent->d_name);

        if(ent->type==VFS_TYPE_DIR && recurse){

            playlist_addFolder(fname,true);
        }else if (ent->type==VFS_TYPE_FILE){

            // do we have a codec for the file?
            if(codec_findCodecFor(fname)!=NULL){

                playlist_addFile(fname);
            }
        }
    }

    closedir(dir);
    free(fname);

    return true;
}

void playlist_printItems(){
    PLAYLIST_ITEM * item;

    item=playlist_first;

    while(item!=NULL){
        printk("* name=%s, startPos=%d, fileSize=%d, bufferedSize=%d\n",item->name,item->startPos,item->fileSize,item->bufferedSize);
        item=item->next;
    }
}

void playlist_init(){

    playlist_first=NULL;
    playlist_last=NULL;
    playlist_count=0;
}
