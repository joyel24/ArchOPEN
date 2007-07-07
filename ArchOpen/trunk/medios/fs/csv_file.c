/*
* medios/fs/csv_file.c
*
*   MediOS project
*   Copyright (c) 2007 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#include <lib/string.h>

#include <sys_def/stddef.h>
#include <sys_def/errors.h>

#include <kernel/kernel.h>
#include <kernel/malloc.h>

#include <fs/stdfs.h>

#define DEBUG_CSV
#ifdef DEBUG_CSV
#define PRINTK_CSV(s...)   printk(s)
#else
#define PRINTK_CSV(s...)
#endif

int fileIsOpen=0;
char * buffer;
int size=0;
int lineStart=0;


/* RQ: we are loading the whole file in one big buffer
if we need to load big csv file will have to modify this */
MED_RET_T csv_newFile(char * filename)
{
    int fd;;
    fd=open(filename,O_RDONLY);

    if (fd<0)
    {
        printk("[csv|newFile] can't open file %s!\n",filename);
        fileIsOpen=0;
        return -MED_ENOENT;
    }
    
    fileIsOpen=1;
    size=filesize(fd);
    buffer=(char*)malloc(size);
    if(!buffer)
    {
        printk("[csv|newFile] not enough mem to allocate buffer of %d bytes\n",size);
        return -MED_ENOMEM;
    }
    
    if(read(fd,buffer,size)!=size)
    {
        printk("[csv|newFile] Error redaing file\n");
        kfree(buffer);
        return -MED_EIO;
    }
    
    lineStart=0;
    close(fd);
    
    return MED_OK;
}

MED_RET_T csv_end(void)
{
    if(fileIsOpen)
    {
        kfree(buffer);
    }
    else
    {
        printk("[csv|end] file not open yet\n"); 
        return -MED_ENOTOPEN;
    }
    return MED_OK;

}

MED_RET_T csv_line2Array(int nbItem,char ** itemArray,char sepChar)
{
    int deb,cur;
    char * item;
    int itemSize;
    int itemNum=0;

    if(fileIsOpen)
    {
        cur=deb=lineStart;
        
        while(cur<size)
        {
            /* empty line or \n\r, \r\n */
            if(cur==lineStart && (buffer[cur] == '\n' || buffer[cur] == '\r'))
            {
                cur++;
                lineStart=deb=cur;
                PRINTK_CSV("[csv|line2Array] empty line\n");
                continue;
            }
            
            /* we have an item */
            if(buffer[cur]==sepChar || buffer[cur] == '\n' || buffer[cur] == '\r')
            {
                /* creating new item */                               
                itemSize=cur-deb;
                item=(char*)malloc(itemSize+1);
                if(itemSize>0)
                    memcpy(item,&buffer[deb],itemSize);
                /* adding leading char 0 */
                item[itemSize]='\0';
                itemArray[itemNum]=item;
                /* next item */
                itemNum++;
                PRINTK_CSV("[csv|line2Array] item %d: %s (size=%d)\n",itemNum,item,itemSize);
                if(itemNum==nbItem && buffer[cur]==sepChar)
                {
                    /* we have read all item, searching for end of line or file */
                    printk("[csv|line2Array] All item read before end of line=> error\n");
                    while( cur<size && buffer[cur] != '\n' && buffer[cur] != '\r')
                        cur++;                  
                    lineStart=cur;
                    return -MED_EINVAL;
                }
                
                if(buffer[cur] == '\n' || buffer[cur] == '\r')
                {
                    /* we are at the end of a line */
                    PRINTK_CSV("[csv|line2Array] End of line\n");
                    cur++;
                    lineStart=cur;
                    /* have we read all asked items ? */
                    if(itemNum<nbItem)
                    {
                        printk("[csv|line2Array] Not all item read, missing: %d => error\n",nbItem-itemNum);
                        for(;itemNum<nbItem;itemNum++)
                            itemArray[itemNum]=NULL;
                        return -MED_EMOBJ;
                    }
                    /* everything is fine*/
                    return MED_OK;
                }
                /* move to next item*/
                cur++;
                deb=cur;
            }
            else
                /* move to next char */
                cur++;
        } /* end of loop*/
        PRINTK_CSV("[csv|line2Array] End of file\n");
        /* we are at the end of file: all other case of exit treated directly in the loop */
        if(deb!=cur)
        {
            PRINTK_CSV("[csv|line2Array] adding one last item\n");
            /* one last item to add */
            itemSize=cur-deb;
            item=(char*)malloc(itemSize+1);
            if(itemSize>1)
                memcpy(item,&buffer[deb],itemSize);
            /* adding leading char 0 */
            item[itemSize]='\0';
            itemArray[itemNum]=item;
            /* next item */
            itemNum++;
            lineStart=deb=cur;
            /* have we read all expected items ? */
            if(itemNum<nbItem)
            {
                /*adding blank item */
                PRINTK_CSV("[csv|line2Array] not enough item in this last line (missing %d)=> error\n",nbItem-itemNum);
                for(;itemNum<nbItem;itemNum++)
                    itemArray[itemNum]=NULL;
                return -MED_EMOBJ;
            }
            /*all is fine with this last line*/
            return MED_OK;
        }
        else
        {
            PRINTK_CSV("[csv|line2Array] Sending EoF\n");
            /* we have finally read all file */
            return -MED_EOF;
        }
    }
    else
    {
        printk("[csv|line2Array] File not open yet\n");
        return -MED_ENOTOPEN;
    }
    PRINTK_CSV("[csv|line2Array] No reason to be here !!!\n");
    return -MED_ERROR;
}

MED_RET_T csv_readLine(void * data,char * formatStr, char sepChar)
{
    int nbElem;
    MED_RET_T ret_val;
    char ** strArray;
    int i,j;
    /* computing number of element */
    /* in formatStr 1 char for 1 data type => strlen*/
    nbElem=strlen(formatStr);
    strArray=(char**)malloc(sizeof(char*)*nbElem);
    memset(strArray,0,sizeof(char*)*nbElem);
    /*calling main parser function*/
    ret_val=csv_line2Array(nbElem,strArray,sepChar);
    if(ret_val != MED_OK)
    {
        /* we get an error, freeing everything*/
        for(i=0;i<nbElem;i++)
            if(strArray[i])
                kfree(strArray[i]);
        if(ret_val!=-MED_EOF)
            printk("[csv|readLine] Exiting after error %d in line2Array\n",-ret_val);
        return ret_val;
    }
    
    /* converting to struct */
    for(i=0;i<nbElem;i++)
    {
       switch(formatStr[i])
       {
           case 'd':
               *((unsigned int*)data)=atoi(strArray[i]);
               data+=4;
               /*need to free string*/
               kfree(strArray[i]);
               break;
           case 's':
               *((char **)data)=strArray[i];
               data+=4;
               break;
           default:
               printk("[csv|readLine] UKN format: %c - Exiting\n",formatStr[i]);
               /* freeing all strings*/
               for(j=0;j<i;i++)
                   if(formatStr[i]=='s')
                       kfree(strArray[j]);
               for(;j<nbElem;j++)
                   free(strArray[j]);
               kfree(strArray);
               return -MED_EINVAL;
        }
   }
   /* we can free the srting array */
   kfree(strArray);
   return MED_OK;    
}

