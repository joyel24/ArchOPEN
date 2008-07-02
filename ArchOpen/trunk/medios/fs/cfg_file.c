/*
* kernel/gfx/gui/cfg_file.c
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

#include <kernel/kernel.h>
#include <kernel/malloc.h>

#include <fs/stdfs.h>
#include <fs/cfg_file.h>

typedef struct {
    bool dummy;
    bool deleted;
    char * name;
    char * value;
} CFG_ITEM;


struct CFG_DATA_STRUCT {
    CFG_ITEM * items;
    int count;
    int current;
};

static void cfg_escapeLF(char * input,char * output,int length){
    char * ip;
    char * op;

    op=output;
    ip=input;

    while(*ip!='\0' && op-output<length){
        switch(*ip){
            case '\\':
                *op++='\\';
                if(op-output<length){
                    *op++='\\';
                }
                break;
            case '\n':
                *op++='\\';
                if(op-output<length){
                    *op++='n';
                }
                break;
            default:
                *op++=*ip;
                break;
        }

        ++ip;
    }

    *op='\0';
}

static void cfg_unescapeLF(char * input,char * output,int length){
    char * ip;
    char * op;
    bool wasSlash;

    op=output;
    ip=input;
    wasSlash=false;

    while(*ip!='\0' /*&& op-output<length*/){
        switch(*ip){
            case '\\':
                if(wasSlash){
                    *op++='\\';
                    wasSlash=false;
                }else{
                    wasSlash=true;
                }
                break;
            case 'n':
                if(wasSlash){
                    *op++='\n';
                }else{
                    *op++='n';
                }
                wasSlash=false;
                break;
            default:
                *op++=*ip;
                wasSlash=false;
                break;
        }

        ++ip;
    }

    *op='\0';
}

static CFG_ITEM * cfg_getItem(CFG_DATA * data, char * name){
    int i;

    for(i=0;i<data->count;++i){

        if(!data->items[i].dummy && !data->items[i].deleted &&!strcmp(data->items[i].name,name)){

            return &data->items[i];
        }
    }

    // item not found
    return NULL;
}

static CFG_ITEM * cfg_addItem(CFG_DATA * data){

    data->count++;

    data->items=realloc(data->items,data->count*sizeof(CFG_ITEM));

    return &data->items[data->count-1];
}

void cfg_clear(CFG_DATA * data){
    int i;

    if(!data) return;
    
    if (data->items!=NULL){

        // clear items data
        for(i=0;i<data->count;++i){
            kfree(data->items[i].name);
            if (!data->items[i].dummy) free(data->items[i].value);
        }

        kfree(data->items);
    }

    kfree(data);
}

CFG_DATA * cfg_newFile(){
    CFG_DATA * data;

    data=malloc(sizeof(CFG_DATA));

    data->items=malloc(0);
    data->count=0;
    data->current=-1;

    return data;
}

CFG_DATA * cfg_readFile(char * filename){
    int f;
    char * filedata;
    char prev;
    char save;
    int size;
    int namebeg,nameend,valbeg,valend;
    int i;
    int ii;
    CFG_ITEM * item;
    CFG_DATA * data;

    // open the file and read its content into a temp buffer

    f=open(filename,O_RDONLY);

    if (f<0){
        printk("[cfg] can't open file!\n");
        return NULL;
    }

    size=filesize(f);

    filedata=malloc(size);

    if (read(f,filedata,size)!=size){
        printk("[cfg] read error!\n");
        kfree(filedata);
        close(f);
        return NULL;
    }

    close(f);

    data=cfg_newFile();

    // read the data once to count how many items there is and alloc cfg_items

    prev=0;
    for(i=0;i<size;++i){
        if (prev=='\n' || prev==0) data->count++;
        prev=filedata[i];
    }
    
    data->items=malloc(data->count*sizeof(CFG_ITEM));

    // read again and fill cfg_items

    if(data->count>0){
        i=0;
        ii=0;
        do{
            item=&data->items[ii];
            item->dummy=true;
            item->deleted=false;
            item->name=NULL;
            item->value=NULL;
    
            namebeg=nameend=valbeg=valend=i;
    
            // parse a line
            while(i<size && filedata[i]!='\r' && filedata[i]!='\n'){
    
                if(filedata[i]=='=' && valbeg==nameend){ // first '=' on a line?
                    item->dummy=filedata[namebeg]=='#'; // dummy item if no '=' or line is a comment
    
                    nameend=i;
                    valbeg=valend=i+1;
                }
    
                i++;
            };
    
            if (item->dummy){ // dummy item -> the whole line goes into the 'name' field
                nameend=i;
            }else{
                valend=i;
            }
    
            // copy name
            item->name=malloc(nameend-namebeg+1);
            memset(item->name,0,nameend-namebeg+1);
            strncpy(item->name,&filedata[namebeg],nameend-namebeg);
    
            // copy value
            if(!item->dummy){
                item->value=malloc(valend-valbeg+1);
                memset(item->value,0,valend-valbeg+1);
                save=filedata[valend];
                filedata[valend]='\0';
                cfg_unescapeLF(&filedata[valbeg],item->value,valend-valbeg);
                filedata[valend]=save;
            }
    
            // handle CR+LF
            if (i<size-1 && filedata[i]=='\r' && filedata[i+1]=='\n'){
                i++;
            }
    
            i++;
            ii++;
        }while(i<size);
    }

    kfree(filedata);

    return data;
}

bool cfg_writeFile(CFG_DATA * data, char * filename){
    int f;
    int i;
    CFG_ITEM * item;
    char * line;

    // open the file and write back items

    f=open(filename,O_RDWR | O_CREAT);

    if (f<0){
        printk("[cfg] can't open file!\n");
        return false;
    }

    line=malloc(256);

    for(i=0;i<data->count;++i){
        item=&data->items[i];

        if(!item->deleted){

            // build line
            strcpy(line,item->name);
            if(!item->dummy){ // dummy items don't have value
                strcat(line,"=");
                cfg_escapeLF(item->value,line+strlen(line),255-strlen(line));
            }
            strcat(line,"\r\n");

            // write line
            if (write(f,line,strlen(line))!=strlen(line)){
                printk("[cfg] write error!\n");
                close(f);
                kfree(line);
                return false;
            }
        }
    }

    // truncate file
    truncate(f,lseek(f,0,SEEK_CUR));

    close(f);
    kfree(line);

    return true;
}

void cfg_rewindItems(CFG_DATA * data){
    data->current=-1;
}

bool cfg_nextItem(CFG_DATA * data, char * * name,char * * value){

    if(data->count==0) return false;

    // find next valid item
    do{
        data->current++;
    }while(data->items[data->current].dummy || data->items[data->current].deleted);

    if(data->current<data->count){
        *name=data->items[data->current].name;
        *value=data->items[data->current].value;

        return true;
    };

    return false;
}

bool cfg_itemExists(CFG_DATA * data, char * name){

    return cfg_getItem(data, name)!=NULL;
}

char * cfg_readString(CFG_DATA * data, char * name){
    CFG_ITEM * item;

    item=cfg_getItem(data,name);

    if(item!=NULL){

        return item->value;
    };

    return "";
}

int cfg_readInt(CFG_DATA * data, char * name){

    return atoi(cfg_readString(data,name));
}

bool cfg_readBool(CFG_DATA * data, char * name){

    return (cfg_readInt(data,name))?true:false;
}

void cfg_writeString(CFG_DATA * data, char * name,char * value){
    CFG_ITEM * item;

    item=cfg_getItem(data,name);

    if(item!=NULL){

        item->value=realloc(item->value,strlen(value)+1);
        strcpy(item->value,value);
    }else{

        // item not found -> create new item
        item=cfg_addItem(data);

        item->dummy=false;
        item->deleted=false;
        item->name=malloc(strlen(name)+1);
        strcpy(item->name,name);
        item->value=malloc(strlen(value)+1);
        strcpy(item->value,value);
    }
}

void cfg_writeInt(CFG_DATA * data, char * name,int value){
    char sv[12];

    sprintf(sv,"%d",value);

    cfg_writeString(data,name,sv);
}

void cfg_writeBool(CFG_DATA * data, char * name,bool value){

    if(value){

        cfg_writeString(data,name,"1");
    }else{

        cfg_writeString(data,name,"0");
    }
}

void cfg_addDummyLine(CFG_DATA * data, char * text){
    CFG_ITEM * item;

    item=cfg_addItem(data);

    item->dummy=true;
    item->deleted=false;
    item->value=NULL;
    item->name=malloc(strlen(text)+1);
    strcpy(item->name,text);
}

bool cfg_deleteItem(CFG_DATA * data, char * name){
    CFG_ITEM * item;

    item=cfg_getItem(data,name);

    if(item!=NULL){

        item->deleted=true;

        return true;
    }

    return false;
}

void cfg_printItems(CFG_DATA * data){
    int i;

    for(i=0;i<data->count;++i){
        if (data->items[i].deleted){
            printk("i=%d deleted!\n",i);
        }else{
            if (data->items[i].dummy){
                printk("i=%d dummy! text='%s'\n",i,data->items[i].name);
            }else{
                printk("i=%d name='%s' value='%s'\n",i,data->items[i].name,data->items[i].value);
            }
        }
    }
}
