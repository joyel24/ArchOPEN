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
#include <gfx/bmp.h>

#include <gui/icons.h>
#include <gui/icons_data.h>
#include <gui/shellmenu.h>

#include <init/boot_error.h>

#include <sys_def/colordef.h>

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
    icon_add("intHPIcon",(unsigned char*)intHP_icon, 8, 10);
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
    return icon_loadFlag(filename,0,0);
}

ICON icon_loadForce(char * filename)
{
    return icon_loadFlag(filename,1,0);
}

ICON icon_loadOther(char * filename)
{
    return icon_loadFlag(filename,0,1);
}     

#define ICO_READ2(BUFF,POS) ((BUFF[(POS)+1]<<8)|BUFF[(POS)])
#define ICO_READ4(BUFF,POS) ((BUFF[(POS)+3]<<24)|(BUFF[(POS)+2]<<16)|(BUFF[(POS)+1]<<8)|BUFF[(POS)])
        
struct ico_head {
    short header;
    short type;
    short img_count;
};

struct ico_struct {
    char width;
    char height;
    char colour_count;
    char reserved;
    short color_planes;
    short bpp;
    unsigned int size;
    unsigned int offset;
};
    
struct ico_bmap_head {
      unsigned int  biSize;
      unsigned int  biWidth;
      unsigned int  biHeight;
      short   biPlanes;
      short   biBitCount;
      unsigned int  biCompression;
      unsigned int  biSizeImage;
      unsigned int  biXPelsPerMeter;
      unsigned int  biYPelsPerMeter;
      unsigned int  biClrUsed;
      unsigned int  biClrImportant;

} ;
        
#define GET_COLOR_NB(BPP) ((BPP)==1?2:((BPP)==2?4:((BPP)==4?16:((BPP)==8?256:0))))
        
#define WR_ICON_HEAD(DATA,BUFF) {         \
        DATA.header=ICO_READ2(BUFF,0);    \
        DATA.type=ICO_READ2(BUFF,2);      \
        DATA.img_count=ICO_READ2(BUFF,4); \
}
        
#define WR_ICON_DATA(DATA,BUFF) {       \
        DATA.width=BUFF[0];             \
        DATA.height=BUFF[1];            \
        DATA.colour_count=BUFF[2];      \
        DATA.reserved=BUFF[3];          \
        DATA.color_planes=ICO_READ2(BUFF,4); \
        DATA.bpp=ICO_READ2(BUFF,6);     \
        DATA.size=ICO_READ4(BUFF,8);    \
        DATA.offset=ICO_READ4(BUFF,12); \
}

#define WR_ICON_BMAP_HEAD(DATA,BUFF)  { \
        DATA.biSize=ICO_READ4(BUFF,0);           \
        DATA.biWidth=ICO_READ4(BUFF,4);          \
        DATA.biHeight=ICO_READ4(BUFF,8);         \
        DATA.biPlanes=ICO_READ2(BUFF,12);        \
        DATA.biBitCount=ICO_READ2(BUFF,14);      \
        DATA.biCompression=ICO_READ4(BUFF,16);   \
        DATA.biSizeImage=ICO_READ4(BUFF,20);     \
        DATA.biXPelsPerMeter=ICO_READ4(BUFF,24); \
        DATA.biYPelsPerMeter=ICO_READ4(BUFF,28); \
        DATA.biClrUsed=ICO_READ4(BUFF,32);       \
        DATA.biClrImportant=ICO_READ4(BUFF,36);  \
}
        
#define READ_FROM_FILE(FILE,BUFF,SIZE,ERROR) { \
    if(read(FILE,BUFF,SIZE)!=(SIZE))  \
    {                               \
        printk(ERROR);              \
        goto err1;                  \
    }                               \
}

extern int gui_pal[256][3];
       
MED_RET_T icon_convIco(char * fname,char * filename)
{
    int i,j;
    int infile;
    
    char * pal_conv=NULL;
    char * tmp_pal=NULL;
    char * bmp_data;
    char * mask_data;
    char * ptr;
    char buff[50];
    
    int nbColor=0;
    
    int bmp_scanline,mask_scanline;
    
    struct ico_head icon_head;
    struct ico_struct icon_data;
    struct ico_bmap_head icon_bitmap_head;
    
    printk("[icon|convIco] %s,%s\n",fname,filename);
    
    infile = open(fname, O_RDONLY);
    if(infile<0)
    {
        printk("[icon_load] can't open file '%s' for reading!\n", fname);
        return -MED_ERROR;
    }
    
    READ_FROM_FILE(infile,buff,6,"[icon_load] can't load icon header!\n");        
    WR_ICON_HEAD(icon_head,buff);
    
    /* reading only first image */
    READ_FROM_FILE(infile,buff,16,"[icon_load] can't load icon data structure!\n");        
    WR_ICON_DATA(icon_data,buff);
            
    /*jumping to first img data*/
    lseek(infile,icon_data.offset,SEEK_SET);
    READ_FROM_FILE(infile,buff,45,"[icon_load] can't load icon data structure!\n");        
    WR_ICON_BMAP_HEAD(icon_bitmap_head,buff);
                    
    /* processing palette */        
    nbColor=GET_COLOR_NB(icon_data.bpp);
            
    pal_conv=(char*)malloc(nbColor);        
    if(!pal_conv) {printk("[icon_load] Can't get mem for pal_conv\n"); goto err1;}
    
    tmp_pal=(char*)malloc(nbColor*4);
    if(!tmp_pal) {printk("[icon_load] Can't get mem for pal_conv\n"); goto err1;}
    
    lseek(infile,icon_data.offset+icon_bitmap_head.biSize,SEEK_SET);
            
    READ_FROM_FILE(infile,tmp_pal,nbColor*4,"[icon_load] can't load palette data!\n");
                
    for(j=0;j<nbColor;j++)
    {
        if(gui_pal[j][0] != tmp_pal[j*4+2] || gui_pal[j][1] != tmp_pal[j*4+1] ||
            gui_pal[j][2] != tmp_pal[j*4])
        {
            
            if(tmp_pal[j*4]==0 && tmp_pal[j*4+1]==0 && tmp_pal[j*4+2]==0)
            {
                pal_conv[j]=COLOR_BLACK;
            }
            else
            {  
                pal_conv[j]=get_nearest(tmp_pal[j*4+2],tmp_pal[j*4+1],tmp_pal[j*4]);
                if(pal_conv[j]==0)
                    pal_conv[j]=COLOR_WHITE;
            }
        }
        else
            pal_conv[j]=j;
    }
    
    free(tmp_pal); tmp_pal=NULL;
    
    bmp_scanline=((icon_data.width*8+31)/32)*4;
    mask_scanline=((icon_data.width+31)/32)*4;
    
    bmp_data=(char*)malloc(icon_data.width*icon_data.height);
    if(!bmp_data) { printk("Can't malloc for bmp data\n"); goto err1; }
        
    
    mask_data=(char*)malloc(mask_scanline);
    if(!mask_data) { printk("Can't malloc for mask data\n"); goto err2; }
    
    /* reading bitmap data */
    lseek(infile,icon_data.offset+icon_bitmap_head.biSize+nbColor*4,SEEK_SET);
        
    for(i=icon_data.height-1;i>=0;i--)
    {
        if(read(infile, bmp_data+icon_data.width*i,icon_data.width)<icon_data.width)
        {
            printk("[icon_load] end of file reached - reading bmp data\n");
            goto err3;
        }
        
        if(bmp_scanline>icon_data.width)
        {
            read(infile,buff,bmp_scanline-icon_data.width);            
        }
        
    }
    
    /* processing transparency + palette conversion */
    for(i=icon_data.height-1;i>=0;i--)
    {
        if(read(infile,mask_data,mask_scanline)<mask_scanline)
        {
            printk("[icon_load] end of file reached - reading mask\n");
            goto err3;
        }
        for(j=0;j<icon_data.width;j++)
        {
            if((mask_data[j/8]>>(7-(j%8)))&0x1)
                *(bmp_data+icon_data.width*i+j)=COLOR_TRSP;
            else
                *(bmp_data+icon_data.width*i+j)=pal_conv[(int)*(bmp_data+icon_data.width*i+j)];
        }
    }
    
    /* some clean up */
    close(infile);
    free(mask_data);
    free(pal_conv);

    /* writting new ico file */
    infile = open(fname, O_WRONLY|O_TRUNC);
    if(infile<0)
    {
        printk("[icon_load] can't open file '%s' for writting!\n", fname);
        free(bmp_data);
        return -MED_ERROR;
    }    
    
    write(infile,"AVICO",5);
    
    ptr=strrchr(filename,'.');
        
    /* name size */
    buff[0]=(ptr-filename)&0xFF;
    write(infile,buff,1);
    
    /*name*/
    write(infile,filename,buff[0]);
    
    /* ico size */
    buff[0]=icon_data.width;
    buff[1]=icon_data.height;
    write(infile,buff,2);
    
    /*bitmap data*/
    write(infile,bmp_data,icon_data.height*icon_data.width);
    free(bmp_data);
    close(infile);
    return MED_OK;
err3:
    free(mask_data);    
err2:
    free(bmp_data);
err1:
    if(tmp_pal) free(tmp_pal);
    if(pal_conv) free(pal_conv);
    close(infile);
    return -MED_ERROR;
}
       
ICON icon_loadFlag(char * filename,int force,int other)        
{
    int infile;
    int i;
    int len;
    ICON ptr;
    char * tmpF;
    char * name;
    char buff[10];
    int found=0;
    
    /* create the filename+path */
    if(other)
    {
        tmpF=(char*)malloc(sizeof(char)*(strlen(ICON_OTHER_DIR)+1+strlen(filename)+1));
        if(!tmpF)
        {
            printk("[icon_load (other)] can't create filename string\n");
            return NULL;
        }
        sprintf(tmpF,"%s/%s",ICON_OTHER_DIR,filename);
    }
    else        
    {
        tmpF=(char*)malloc(sizeof(char)*(iconPathLen+1+strlen(filename)+1));
        if(!tmpF)
        {
            printk("[icon_load (menu)] can't create filename string\n");
            return NULL;
        }
        sprintf(tmpF,"%s/%s",iconPath,filename);
    }

    /* open the icon file */
    infile = open(tmpF, O_RDWR);
    if(infile<0)
    {
        printk("[icon_load] can't open file '%s' (%s) for reading!\n", tmpF,filename);
        return NULL;
    }

    /* check if it has the correct ident. string */
    READ_FROM_FILE(infile,buff,5,"[icon_load] bad ident (length)\n");

    if(buff[0]==0 && buff[1]==0 && buff[2]==1 && buff[3]==0)
    {    
        close(infile);    
        if(icon_convIco(tmpF,filename)!=MED_OK)
        {
            printk("[icon_load] Error converting ico to medios icon\n");
            goto err1;
        }
        
        infile = open(tmpF, O_RDWR);
        if(infile<0)
        {
            printk("[icon_load] can't open file '%s' (%s) for reading!\n", tmpF,filename);
            return NULL;
        } 
        READ_FROM_FILE(infile,buff,5,"[icon_load] bad ident (length)\n");
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
                printk("[icon_load] force loading icon %s\n",name);
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
#if 0
    for(i=0;i<ptr->bmap_data.height;i++)
    {
        if(read(infile,ptr->data+ptr->bmap_data.width*i,ptr->bmap_data.width)<ptr->bmap_data.width)
        {
            printk("[icon_load] end of file reached - Step6\n");
            goto err4;
        }
    }
#else
    if(read(infile,ptr->data,len)<len)
    {
        printk("[icon_load] end of file reached - Step6\n");
        goto err4;
    }              
#endif    
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
