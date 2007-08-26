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
#include <snd/tag.h>

#include <snd/playlist.h>

PLAYLIST_ITEM * playlist_first = NULL;
PLAYLIST_ITEM * playlist_last = NULL;

int playlist_count=0;

PLAYLIST_ITEM * playlist_new(PLAYLIST_ITEM * prevItem){
    PLAYLIST_ITEM * item;

    playlist_count++;

    // alloc data
    item=malloc(sizeof(PLAYLIST_ITEM));

    // add to linked list
    item->prev=prevItem;

    if(prevItem!=NULL){

        item->next=prevItem->next;

        if(prevItem->next!=NULL){
            prevItem->next->prev=item;
        }
        prevItem->next=item;
    }else{

        item->next=playlist_first;
        if(playlist_first!=NULL){
            playlist_first->prev=item;
        }

        playlist_first=item;
    }

    if(prevItem==playlist_last){
        playlist_last=item;
    }

    // init members
    item->name=NULL;
    item->startPos=-1;
    item->fileSize=0;
    item->bufferedSize=0;

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
    if(item->name!=NULL) kfree(item->name);

    tag_free(&item->tag);

    kfree(item);
    
    return true;
}

void playlist_clear(){
    while(playlist_last!=NULL){
        playlist_remove(playlist_last);
    }
}

PLAYLIST_ITEM * playlist_addFile(char * name, PLAYLIST_ITEM * prevItem){
    int f;
    PLAYLIST_ITEM * item;

    f=open(name,O_RDONLY);

    if (f<0) return NULL;

    item=playlist_new(prevItem);

    item->name=strdup(name);
    item->fileSize=filesize(f);

    close(f);

    //tags stuff
    tag_clear(&item->tag);

    codec_tagRequest(name,&item->tag);

    if (item->tag.badFile){

        printk("[playlist] bad file: %s\n",item->name);

        playlist_remove(item);

        return NULL;
    }else{

        return item;
    }
}

bool playlist_addFolder(char * name,bool recurse){
    DIR * dir;
    struct dirent * ent;
    char * fname;
    PLAYLIST_ITEM * startItem;
    PLAYLIST_ITEM * prevItem;
    PLAYLIST_ITEM * item;
    bool nullItem;

    dir=opendir(name);

    if (dir==NULL){
        return false;
    }

    startItem=playlist_last;
    prevItem=playlist_last;

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

        prevItem=startItem;

        nullItem=(prevItem==NULL);

        for(;;){

            if(nullItem){
                item=playlist_first;
                nullItem=false;
            }else{
                item=prevItem->next;
            }

            if(item==NULL || strcasecmp(item->name,fname)>0){
                break;
            }

            prevItem=item;
        };

        if(ent->type==VFS_TYPE_DIR && recurse){

            //add a temporary fake item
            item=playlist_new(prevItem);

            item->name=strdup(fname);
            item->fileSize=-1;

        }else if (ent->type==VFS_TYPE_FILE){

            // do we have a codec for the file?
            if(codec_findCodecFor(fname)!=NULL){

                item=playlist_addFile(fname,prevItem);
            }
        }
    }

    closedir(dir);
    kfree(fname);

    //find fake dir items, call addFolder on them then delete them

    if(startItem==NULL){
        item=playlist_first;
    }else{
        item=startItem->next;
    }

    do{
        if(item->fileSize==-1){
            
            playlist_addFolder(item->name,true);
            
            prevItem=item;
            item=item->next;
            
            playlist_remove(prevItem);
        }else{

            item=item->next;
        }

    }while(item!=NULL);


    return true;
}

bool playlist_addM3UPlaylist(char * name){
    char * data;
    char * path;
    char * item;
    char * line;
    char * pos;
    char * context;
    char save;
    int f;
    int dataSize;

    //read playlist file
    f=open(name,O_RDONLY);

    if(f<0){
        return false;
    }

    dataSize=filesize(f);
    data=malloc(dataSize+1);

    read(f,data,dataSize);
    data[dataSize]='\0';

    close(f);

    //extract path
    path=malloc(MAX_PATH+1);
    pos=strrchr(name,'/');
    if(pos!=NULL){
        ++pos;
        save=*pos;
        *pos='\0';
        strcpy(path,name);
        *pos=save;
    }else{
        *path='\0';
    }

    //convert \ to /
    do{
        pos=strchr(data,'\\');
        if(pos!=NULL){
            *pos='/';
        }
    }while(pos!=NULL);


    item=malloc(MAX_PATH+1);

    //scan lines
    pos=strtok_r(data,"\r\n",&context);
    while(pos!=NULL){

        //trim spaces
        line=pos;
        while(*line==' '){
            line++;
        }

        if(strstr(line,"#EXTINF")!=line && strstr(line,"#EXTM3U")!=line){ // skip extended m3u lines
            //detect & remove windows drives letters
            if(line[1]==':'){
                line+=2;
            }

            if(line[0]=='/'){ // absolute path?
                strncpy(item,line,MAX_PATH);
            }else{
                snprintf(item,MAX_PATH,"%s%s",path,line);
            }

            //add item to playlist
            playlist_addFile(item,playlist_last);
        }

        pos=strtok_r(NULL,"\r\n",&context);
    }

    kfree(item);
    kfree(path);
    kfree(data);

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
