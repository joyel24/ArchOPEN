/*
* kernel/gui/gfx/icons.c
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

#include <kernel/kernel.h>
#include <kernel/malloc.h>

#include <fs/stdfs.h>
#include <fs/cfg_file.h>

#include <gfx/graphics.h>

#include <gui/icons.h>
#include <gui/icons_data.h>
#include <gui/shellmenu.h>

#include <init/boot_error.h>

int arch_icon_type[]=
{
	1, /* AV3XX_ARCH */
	0, /* GMINI4XX_ARCH */
	0, /* AV1XX_ARCH */
	0, /* JBMM_ARCH */
	0, /* GMINI402_ARCH */
	1, /* AV4XX_ARCH */
	1, /* PMA_ARCH */
	2, /* AV5XX_ARCH */
};

char * arch_icon_string[]=
{
	ICON_DIR_SMALL,ICON_DIR_STD,ICON_DIR_BIG
};

char ident_str[]="AVICO";

char * iconPath=NULL;
int iconPathLen=0;
int folderType=0;

ICON icon_list_head=NULL;
int icon_initDone=0;

void icon_initFolderName(void)
{
	DIR * fd;
	/* we should read here cfg file*/
	/* ==> using default value */

	folderType=arch_icon_type[CURRENT_ARCH];
	if(iconPathLen!=0)
	{
		free(iconPath);
		iconPathLen=0;
	}

	while(folderType>=0)
	{
		iconPathLen=strlen(ICON_DIR)+strlen(arch_icon_string[folderType]);
		iconPath=(char*)malloc(iconPathLen);
		sprintf(iconPath,"%s%s",ICON_DIR,arch_icon_string[folderType]);
		if((fd=opendir(iconPath))!=NULL)
		{
			closedir(fd);
			break;
		}
		free(iconPath);
		iconPathLen=0;
		folderType--;
	}

	if(!iconPathLen)
	{
        printk("No new icon folder found\n");
        if((fd=opendir(ICON_DIR))!=NULL)
        {
            closedir(fd);
            gui_bootError(OLD_ICON_FOLDER_ERROR,BOOT_WARN);
            iconPath=ICON_DIR"/";
            iconPathLen=strlen(iconPath);
            folderType=1;
        }
        else
        {
            gui_bootError(MISSING_ICON_FOLDER_ERROR,BOOT_WARN);
            iconPath="";
            folderType=0;
        }
		
	}
    
    if(folderType==0)
        shellHasCaption=0;
    else
        shellHasCaption=1;
    
    
}

void icon_kernelInit(void)
{
	/* adding icon included during build*/
    icon_add("mediosLogo",(unsigned char*)medios_logo, 51, 10);
    icon_add("usbIcon",(unsigned char*)usb_icon, 15, 6);
    icon_add("fwExtIcon",(unsigned char*)fw_ext_icon, 15, 6);
    icon_add("cfIcon",(unsigned char*)cf_icon, 15, 6);
    icon_add("powerIcon",(unsigned char*)power_icon, 13, 6);
    icon_add("upBitmap",(unsigned char*)upArrow, 9, 9);
    icon_add("dwBitmap",(unsigned char*)dwArrow, 9, 9);
    icon_add("dirBitmap",(unsigned char*)dir, 8, 8);
    icon_add("mp3Bitmap",(unsigned char*)mp3, 8, 8);
    icon_add("textBitmap",(unsigned char*)text, 8, 8);
    icon_add("imageBitmap",(unsigned char*)image, 8, 8);
    icon_add("MsgBoxExclamationBitmap",(unsigned char*)MsgBoxExclamation, 18, 18);
    icon_add("MsgBoxQuestionBitmap",(unsigned char*)MsgBoxQuestion, 18, 18);
    icon_add("MsgBoxWarningBitmap",(unsigned char*)MsgBoxWarning, 18, 18);
    icon_add("MsgBoxInformationBitmap",(unsigned char*)MsgBoxInformation, 18, 18);
    icon_add("MsgBoxErrorBitmap",(unsigned char*)MsgBoxError, 18, 18);
    icon_initDone=0;
}

void icon_init(void)
{
    CFG_DATA * cfg;
    int needWrite=0;
           
    if(!icon_initDone)
    {
        icon_initDone=1;
        cfg=cfg_readFile("/medios/medios.cfg");
                
        if(!cfg)
        {
            printk("Can't open cfg file\n");
            /* creating default */
            cfg=cfg_newFile();
            icon_initFolderName();
            if(!cfg)
            {
                printk("Can't create new cfg file\n");
                return;
            }           
            
            cfg_writeInt(cfg,"shellHasCaption",shellHasCaption);
            cfg_writeInt(cfg,"iconSize",folderType);            
            needWrite=1;   
        }
        else
        {
            if(cfg_itemExists(cfg,"shellHasCaption"))
            {
                shellHasCaption=cfg_readInt(cfg,"shellHasCaption");
            }
            else
            {
                int old_data=folderType;
                icon_initFolderName();
                folderType=old_data;
                cfg_writeInt(cfg,"shellHasCaption",shellHasCaption);
                needWrite=1;
            }   
                         
            if(cfg_itemExists(cfg,"iconSize"))
            {
                folderType=cfg_readInt(cfg,"iconSize");
            }
            else
            {
                int old_data=shellHasCaption;
                icon_initFolderName();
                shellHasCaption=old_data;
                cfg_writeInt(cfg,"iconSize",folderType);
                needWrite=1;
            }  
        }
        if(needWrite) cfg_writeFile(cfg,"/medios/medios.cfg");
        cfg_clear(cfg);
        
        icon_setPath();        
    }
}

void icon_setPath(void)
{
    iconPathLen=strlen(ICON_DIR)+strlen(arch_icon_string[folderType]);
    iconPath=(char*)malloc(iconPathLen);
    sprintf(iconPath,"%s%s",ICON_DIR,arch_icon_string[folderType]);
    printk("[icon] init folder: using %s (len=%d)(type=%d)(caption=%d)\n",
           iconPath,iconPathLen,folderType,shellHasCaption);
}
    

ICON icon_load(char * filename)
{
    return icon_loadFlag(filename,0);
}
        
ICON icon_loadFlag(char * filename,int force)        
{
    int infile;
    int i;
    int len;
    ICON ptr;
    char * tmpF;
    char * name;
    char buff[6];
    int found=0;

    /* create the filename+path */
    tmpF=(char*)malloc(sizeof(char)*(iconPathLen+1+strlen(filename)+1));
    if(!tmpF)
    {
        printk("[icon_load] can't create filename string\n");
        return NULL;
    }
    sprintf(tmpF,"%s/%s",iconPath,filename);

    /* open the icon file */
    infile = open(tmpF, O_RDONLY);
    if(infile<0)
    {
        printk("[icon_load] can't open file '%s' (%s) for reading!\n", tmpF,filename);
        return NULL;
    }

    /* check if it has the correct ident. string */
    if(read(infile,buff,5)!=5)
    {
        printk("[icon_load] bad ident (length)\n");
        goto err1;
    }

    for(i=0;i<5;i++)
    {
        if(buff[i]!=ident_str[i])
        {
            printk("[icon_load] bad ident (string)\n");
            goto err1;
        }
    }

    /* read strlen & create string*/
    if(read(infile,buff,1)<=0)
    {
        printk("[icon_load] end of file reached - Step2\n");
        goto err1;
    }
    len=(int)buff[0];
    name=(char*)kmalloc(sizeof(char)*(len+1));
    if(!name)
    {
        printk("[icon_load] not enough memory for name in icon structure\n");
        goto err1;
    }

    /* read icon name */

    if(read(infile,name,len)<len)
    {
        printk("[icon_load] end of file reached - reading name\n");
        goto err2;
    }

    name[len]='\0';

    /* check if an icon with the same name is not already present */

    for(ptr=icon_list_head;ptr!=NULL;ptr=ptr->nxt)
    {
        if(!strcmp(ptr->name,name))
        {
            if(force)
            {
                kfree(ptr->data);
                kfree(name);
                found=1;
                break;
            }
            else
            {
                printk("[icon_load] there is already an icon with that name\n");
                kfree(name);
                goto out;
            }
        }
    }

    /* create new icon struct*/
    if(!found)
    {
        ptr=(ICON)kmalloc(sizeof(struct icon_elem));
        if(!ptr)
        {
            printk("[icon_load] not enough memory for icon structure\n");
            goto err2;
        }
        ptr->name=name;
    }

    /* read width and height */
    if(read(infile,buff,1)<=0)
    {
        printk("[icon_load] end of file reached - Step4\n");
        goto err3;
    }
    ptr->bmap_data.width=(int)buff[0];

    if(read(infile,buff,1)<=0)
    {
        printk("[icon_load] end of file reached - Step5\n");
        goto err3;
    }
    ptr->bmap_data.height=(int)buff[0];
    
    /* create space for bmap data */
    ptr->data=(unsigned char *)kmalloc(sizeof(unsigned char)*ptr->bmap_data.width*ptr->bmap_data.height);
    if(!ptr->data)
    {
        printk("[icon_load] not enough memory for data in icon structure (w=%d,h=%d)\n",
               ptr->bmap_data.width,ptr->bmap_data.height);
        goto err3;
    }
    
    len=ptr->bmap_data.width*ptr->bmap_data.height;

   /* printk("[icon_load] loading icon %s from %s size (%d,%d)\n",ptr->name,
        tmpF,ptr->bmap_data.width,ptr->bmap_data.height);*/

    /* read icon data */
    /*for(i=0;i<ptr->bmap_data.width*ptr->bmap_data.height;i++)
    {
        if(read(infile,buff,1)<=0)
        {
            printk("[icon_load] end of file reached - Step6\n");
            //goto err4;
        }
        ptr->data[i]=buff[0];
    }*/

    for(i=0;i<ptr->bmap_data.height;i++)
    {
        if(read(infile,ptr->data+ptr->bmap_data.width*i,ptr->bmap_data.width)<ptr->bmap_data.width)
        {
            printk("[icon_load] end of file reached - Step6\n");
            goto err4;
        }
    }
    
    /*if(read(infile,ptr->data,len)<len)
    {
        printk("[icon_load] end of file reached - reading icon data\n");
        goto err4;
    }*/

    /* fill BITMAP structure */
    ptr->bmap_data.data=(unsigned int)ptr->data;
    ptr->bmap_data.type=0;
    ptr->bmap_data.bpline=0;
    
    /* insert new icon in list */
    if(!found)
    {
        ptr->nxt=icon_list_head;
        icon_list_head=ptr;
    }
    
out:
    /* close file */
    close(infile);
    kfree(tmpF);

    return ptr;

err4:
    kfree(ptr->data);
err3:
    kfree(ptr);
err2:
    kfree(name);
err1:
    close(infile);
    kfree(tmpF);
    return NULL;
}

ICON icon_add(char * name,unsigned char * data,int w,int h)
{
    ICON ptr;
    //printk("addIcon %s ",name);
    /* check if an icon with the same name is not already present */

    for(ptr=icon_list_head;ptr!=NULL;ptr=ptr->nxt)
    {
        if(!strcmp(ptr->name,name))
        {
            //printk("[icon_load] there is already an icon with that name\n");
            goto out;
        }
    }
    //printk("- not defined ");
    /* create new icon struct*/
    ptr=(ICON)kmalloc(sizeof(struct icon_elem));
    if(!ptr)
    {
        printk("[icon_load] not enough memory for icon structure\n");
        goto err1;
    }
    //printk("- allocated ");
    ptr->name=name;
    ptr->data=data;
    ptr->bmap_data.data=(unsigned int)ptr->data;
    ptr->bmap_data.width=w;
    ptr->bmap_data.height=h;
    ptr->bmap_data.type=0;
    ptr->bmap_data.bpline=0;

    /* insert new icon in list */
    ptr->nxt=icon_list_head;
    icon_list_head=ptr;
    //printk("- init\n");
out:
    return ptr;

err1:
    return NULL;
}

ICON icon_get(char * name)
{
    ICON ptr;
    for(ptr=icon_list_head;ptr!=NULL;ptr=ptr->nxt)
    {
        if(!strcmp(ptr->name,name))
        {
            return ptr;
        }
    }
    printk("Icon %s not found\n",name);
    return NULL;
}
