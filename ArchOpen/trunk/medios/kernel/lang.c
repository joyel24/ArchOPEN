/*
*   medios/kernel/lang.c
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

#include <kernel/lang.h>
#include <kernel/kernel.h>
#include <kernel/malloc.h>

#include <fs/stdfs.h>

char * customLang;

char * langString[STRLNG_LAST_ENTRY];

extern char * buildin_lng;
extern char * buildin_lng_end;

char * ptr_buildin;
char * ptr_buildin_end;

MED_RET_T lang_loadLng(char * ptr,int size)
{
    int i,id;
    char * end=ptr+size;
    
    if(strncmp(ptr,"LNG",3))
    {
        printk("Not a lang buffer\n");
        return -MED_ERROR;
    }
    
    //print_data(ptr,10);
    
    ptr+=4; // skipping LNG header + 1 char of invisible header 
    
    for(i=0;i<STRLNG_LAST_ENTRY;i++)
    {
        id=(*ptr)<<8 | (*(ptr+1));
        ptr+=2;
        if(id<STRLNG_LAST_ENTRY)
        {
//            printk("[lang_loadLng] Found %d: %s\n",id,ptr);
            langString[id]=ptr;
        }
        else
        {
            if(id==0xFFFF)
            {
                printk("[lang_loadLng] End of reached: found end magic id (%d str found, max=%d)\n",
                       i,STRLNG_LAST_ENTRY);
                break;
            }
            else
            {
                printk("[lang_loadLng] bad id: %d for %s\n",id,ptr);
            }
        }
        while(ptr<end && *ptr) ptr++;
        if(ptr==end)
        {
            printk("[lang_loadLng] found %d str (max is %d)\n",i,STRLNG_LAST_ENTRY);
            break;
        }
        ptr++;
    }
    return MED_OK;
}

void lang_loadDefault(void)
{
    int size;
    if(customLang==NULL)
        return;
    ptr_buildin=(char*)&buildin_lng;
    ptr_buildin_end=(char*)&buildin_lng_end;
    size=ptr_buildin_end-ptr_buildin;
    customLang=NULL;
    lang_loadLng(ptr_buildin,size);
}

char * lang_getStr(int id)
{
    printk("id=%d - %x |%s|\n",id,langString[id],langString[id]);
    
    return getLangStr(id);
}

void lang_init(void)
{
    customLang=(char*)1;
    lang_loadDefault();
    //printk("lang at 0x%x\n",langString);
}

MED_RET_T lang_loadFile(char * fName)
{
    int fd;
    int size;    
    char * buffer;
    printk("[lang_loadFile] loading lng from %s\n",fName);
    fd=open(fName,O_RDONLY);
    if(fd<0)
    {
        printk("[lang_loadFile] Error loading file %s\n",fName);
        return -MED_ENOTFOUND;
    }
    size=filesize(fd);
    buffer=(char*)malloc(size);
    if(!buffer)
    {
        printk("[lang_loadFile] Error getting mem for buffer\n");
    }
    size=read(fd,buffer,size);
    close(fd);
if(lang_loadLng(buffer,size)!=MED_OK)
        return -MED_ERROR;
/* NOTE: on error should reload previous lang */
    if(customLang)        
        free(customLang);
    customLang=buffer;
    return MED_OK;
#if 0
lang_error:
    /*restoring initial data*/
    lang_loadLng(buildin_lng,buildin_lng_end-buildin_lng);
    return -MED_EBADDATA;
#endif
}
