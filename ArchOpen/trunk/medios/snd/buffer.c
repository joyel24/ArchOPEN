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
#include <kernel/thread.h>
#include <fs/stdfs.h>

#include <snd/playlist.h>
#include <snd/buffer.h>

#define POS2OFF(x) ((x)%buffer_size)
#define POS2PTR(x) (&buffer_data[POS2OFF(x)])

static char * buffer_data = NULL;

static int buffer_size = 0;

static int buffer_startPos = 0;
static int buffer_endPos = 0;

static int buffer_curPos = 0;

static THREAD_INFO * buffer_thread = NULL;

static PLAYLIST_ITEM * buffer_activeItem = NULL;
static PLAYLIST_ITEM * buffer_bufferedItem = NULL;

bool buffer_bufferedItemChanged=false;

static void buffer_updateBounds(int delta){
    buffer_endPos+=delta;
    
    if((buffer_endPos-buffer_startPos)>buffer_size){
        buffer_startPos=buffer_endPos-buffer_size;
        printk("buffer_startPos %d\n",buffer_startPos);
    }
}

static int buffer_loadData(int f,int pos,int count){
    printk("[buffer] loading data, pos=%d count=%d\n",pos,count);

    // will we cross the buffer end?
    if(count>(buffer_size-POS2OFF(pos))){
        int red;

        // we'll have to read the file in 2 parts

        red=read(f,POS2PTR(pos),buffer_size-POS2OFF(pos));

        if(red<count){
            red+=read(f,buffer_data,count-red);
        }

        return red;
    }else{

        return read(f,POS2PTR(pos),count);
    }
}

static bool buffer_bufferItem(PLAYLIST_ITEM * item){
    int free;
    int f;
    bool success;

    printk("[buffer] buffering %s\n",item->name);

    //open the file
    f=open(item->name,O_RDONLY);

    if(f<0) return false;

    //calculate free buffer space
    free=buffer_size-(buffer_endPos-buffer_curPos);

    item->bufferedSize=0;
    item->startPos=buffer_endPos;

    success=false;
    if(item->fileSize<=free){

        // buffer whole file
        item->bufferedSize=buffer_loadData(f,buffer_endPos,item->fileSize);

        buffer_updateBounds(item->bufferedSize);

        success=true;
    }else{
        for(;;){
            int red;

            // buffer what we can
            red=buffer_loadData(f,buffer_endPos,free);

            buffer_updateBounds(red);
            item->bufferedSize+=red;

            if((buffer_endPos-item->startPos)>=item->fileSize){
                success=true;
                break;
            }

            // wait for curPos to reach critical zone
            while(!buffer_bufferedItemChanged && buffer_curPos<(buffer_endPos-BUFFER_CRITICAL_ZONE)){
                yield();
            }

            // we must stop buffering current item if another item is to be buffered
            if(buffer_bufferedItemChanged){

                // discard what was buffered
                buffer_curPos=buffer_endPos=item->startPos;

                item->bufferedSize=0;
                item->startPos=-1;

                success=false;
                break;
            }

            //TODO: seek if curpos is farther than 1 buffer size from endpos (optimisation)

            // recalc free space
            free=buffer_size-(buffer_endPos-buffer_curPos);
        }
    }

    close(f);

    printk("[buffer] end buffering\n");

    return success;
}

static void buffer_threadFunction(){
    printk("\n[buffer] thread start\n");

    for(;;){
        // wait for a playlist item to buffer
        while(!buffer_bufferedItemChanged){
            yield();
        }

        buffer_bufferedItemChanged=false;

        while(buffer_bufferedItem!=NULL){

            buffer_bufferedItemChanged=false;

            buffer_bufferItem(buffer_bufferedItem);

            if(!buffer_bufferedItemChanged){
                buffer_bufferedItem=buffer_bufferedItem->next;
            }
        };
    }
}

int buffer_seek(int offset,int whence){

    switch(whence){

        case SEEK_SET:
            if(offset>=0 &&
               offset<buffer_activeItem->fileSize){
                buffer_curPos=buffer_activeItem->startPos+offset;
                break;
            }else{
                return -1;
            }

        case SEEK_CUR:
            if(offset>=-(buffer_curPos-buffer_activeItem->startPos) &&
               offset<=(buffer_activeItem->fileSize-(buffer_curPos-buffer_activeItem->startPos))){
                buffer_curPos+=offset;
                break;
            }else{
                return -1;
            }

        case SEEK_END:
            if(offset<=0 &&
               offset>=-buffer_activeItem->fileSize){
                buffer_curPos=buffer_activeItem->startPos+buffer_activeItem->fileSize+offset;
                break;
            }else{
                return -1;
            }
    }

    return buffer_curPos-buffer_activeItem->startPos;
}

int buffer_read(void * buf,int count){
    int remaining;

//    printk("read %d %d\n",buffer_curPos,buffer_endPos);

    //handle buffering on rewind before buffer start
    if(buffer_curPos<buffer_startPos){
        int offset;

        // save wanted file offset
        offset=buffer_curPos-buffer_activeItem->startPos;

        buffer_curPos=buffer_endPos;

        printk("rewind %d\n",offset);

        // ask to rebuffer the item
        buffer_setActiveItem(buffer_activeItem);

        // restore offset
        buffer_seek(offset,SEEK_SET);

    }


    // make sure we don't read past file end
    remaining=buffer_activeItem->fileSize-(buffer_curPos-buffer_activeItem->startPos+count);
    if(remaining<0){
        count+=remaining;
    }

    // wait for the buffer thread to buffer data
    while((buffer_curPos+count)>buffer_endPos){
        yield();
    }

    // will we cross the buffer end?
    if(POS2OFF(buffer_curPos+count)<POS2OFF(buffer_curPos)){
        int end;

        // we'll have to read the buffer in 2 parts

        end=buffer_size-POS2OFF(buffer_curPos);

        memcpy(buf,POS2PTR(buffer_curPos),end);
        memcpy(buf+end,buffer_data,count-end);
    }else{

        memcpy(buf,POS2PTR(buffer_curPos),count);
    }

    buffer_curPos+=count;

    return count;
}

void buffer_setActiveItem(PLAYLIST_ITEM * item){

/*
    printk("### sp=%d ep=%d isp=%d\n",buffer_startPos,buffer_endPos,item->startPos);
    playlist_printItems();
*/

    buffer_activeItem=item;

    //do we have to prebuffer the item?
    if(item->startPos<buffer_startPos){
        buffer_bufferedItem=buffer_activeItem;
        buffer_bufferedItemChanged=true;

        // wait for the item buffering to start
        while(buffer_activeItem->startPos<buffer_startPos){
            yield();
        }
    }

    buffer_curPos=buffer_activeItem->startPos;
}

PLAYLIST_ITEM * buffer_getActiveItem(){
    return buffer_activeItem;
}

void buffer_start(void)
{
    printk("[Buffer] starting\n");
    buffer_startPos=0;
    buffer_endPos=0;
    buffer_curPos=0;
    
    buffer_bufferedItem=NULL;
    buffer_activeItem=NULL;
    buffer_bufferedItemChanged=false;

    // alloc audio buffer
    buffer_size=BUFFER_SIZE;

    buffer_data=malloc(BUFFER_SIZE);

    if(buffer_data==NULL){
        printk("[buffer] error: couldn't alloc audio buffer\n");
        return;
    }
    thread_enable(buffer_thread->pid);
}

void buffer_stop(void)
{
    // stop buffering any file
    buffer_bufferedItem=NULL;
    buffer_bufferedItemChanged=true;
    while(buffer_bufferedItemChanged){
        yield();
    }

    free(buffer_data);
    thread_disable(buffer_thread->pid);
}

void buffer_init()
{
    buffer_thread=NULL;
    //create buffer thread
    thread_startFct(&buffer_thread,buffer_threadFunction,"Audio buffer",THREAD_STATE_DISABLE,PRIO_MED);
}
