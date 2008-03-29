/*
*   medios/gfx/bmp.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/kernel.h>
#include <kernel/malloc.h>
#include <kernel/timer.h>

#include <gfx/graphics.h>
#include <gfx/bmp.h>

#include <driver/lcd.h>
#include <driver/osd.h>

#include <fs/stdfs.h>

#include <sys_def/errors.h>
#include <sys_def/stddef.h>


#include <math.h>

extern struct graphicsBuffer VIDEO_1;
static bmpColor * bmpPalette;

static bmp_info_header infoHeader;
static bmp_file_header fileHeader;

unsigned int bmp_numberColor(int bitcount)
{
    unsigned int res;
    switch(bitcount)
    {
        case 1 : res=2; break;
        case 4 : res=16; break;
        case 8 : res=256; break;
        case 16: res=65536; break;
        case 24: res=16777216; break;
        case 32: res=16777216; break;
        default: res=0;
    }
    return res;
}



void bmp_read1BitRow(unsigned int * buffer,char * data,int width,int output_mode)
{
    int i,j,k;;
    bmpColor * tmp,*tmp2;
    int hasMore=width%8;
    char dataVal;
    char * ptrBmap=(char*)buffer;
    
    if(output_mode==BMP_MODE_VIDEO)
    {
        width/=8;
        j=0;
    
        for(i=0;i<width;i++)
        {
            dataVal=data[i];
            for(k=0;k<4;k++)
            {
                tmp=&bmpPalette[(dataVal&0x80)>>7];
                dataVal<<=1;
                tmp2=&bmpPalette[(dataVal&0x80)>>7];
                dataVal<<=1;
                buffer[j]=(unsigned int)((tmp2->Y&0xFF)<<24 | (((tmp->Cr+tmp2->Cr)>>1)&0xFF) <<16 |
                        (tmp->Y&0xFF)<<8 | (((tmp->Cb+tmp2->Cb)>>1)&0xFF));
                j++;
            }
        }
    
        if(hasMore)
        {
            dataVal=data[i];
            while(hasMore>1)
            {
                tmp=&bmpPalette[(dataVal&0x80)>>7];
                dataVal<<=1;
                tmp2=&bmpPalette[(dataVal&0x80)>>7];
                dataVal<<=1;
                buffer[j]=(unsigned int)((tmp2->Y&0xFF)<<24 | (((tmp->Cr+tmp2->Cr)>>1)&0xFF) <<16 |
                        (tmp->Y&0xFF)<<8 | (((tmp->Cb+tmp2->Cb)>>1)&0xFF));
                j++;
                hasMore-=2;
            }
    
            if(hasMore==1)
            {
                tmp=&bmpPalette[(dataVal&0x80)>>7];
                buffer[j]=(unsigned int)(  (tmp->Cr&0xFF)<<16 |
                        (tmp->Y&0xFF)<<8 |(tmp->Cb&0xFF));
                j++;
            }
        }
    }
    else
    {
        dataVal=0;
        for(i=0;i<width;i++)
        {
            if((i%8)==0)
                dataVal=data[i/8];
            ptrBmap[i]=(bmpPalette[(dataVal&0x80)>>7]).index;
            dataVal<<=1;
        }
    }
}

void bmp_read4BitRow(unsigned int * buffer,char * data,int width,int output_mode)
{
    int i;
    bmpColor * tmp,*tmp2;
    int hasMore=width%2;
    char dataVal;
    char * ptrBmap=(char*)buffer;
    
    if(output_mode==BMP_MODE_VIDEO)
    {    
        width/=2;    
        for(i=0;i<width;i++)
        {
            dataVal=data[i];
            tmp2=&bmpPalette[dataVal&0xF];
            tmp=&bmpPalette[(dataVal>>4)&0xF];
            buffer[i]=(unsigned int)((tmp2->Y&0xFF)<<24 | (((tmp->Cr+tmp2->Cr)>>1)&0xFF)<<16 |
                    (tmp->Y&0xFF)<<8 | (((tmp->Cb+tmp2->Cb)>>1)&0xFF));
        }    
        if(hasMore)
        {
            tmp=&bmpPalette[data[i]&0xF];
            buffer[i]=(unsigned int)( (tmp->Cr&0xFF) <<16 |
                    (tmp->Y&0xFF)<<8 |(tmp->Cb&0xFF));
        }
    }
    else
    {
        dataVal=0;
        for(i=0;i<width;i++)
        {
            if((i%2)==0)
                dataVal=data[i/2];
            ptrBmap[i]=(bmpPalette[(dataVal&0xF0)>>4]).index;
            dataVal<<=4;
        }
    }
}

void bmp_read8BitRow(unsigned int * buffer,char * data,int width,int output_mode)
{
    int i;
    bmpColor * tmp,*tmp2;
    int hasMore=width%2;
    char * ptrBmap=(char*)buffer;
    
    if(output_mode==BMP_MODE_VIDEO)
    {    
        width/=2;
    
        for(i=0;i<width;i++)
        {
            tmp2=&bmpPalette[data[i*2]&0xFF];
            tmp=&bmpPalette[data[i*2+1]&0xFF];
            buffer[i]=(unsigned int)((tmp->Y&0xFF)<<24 | (((tmp->Cr+tmp2->Cr)>>1)&0xFF) <<16 |
                    (tmp2->Y&0xFF)<<8 | (((tmp->Cb+tmp2->Cb)>>1)&0xFF));
        }
    
        if(hasMore)
        {
            tmp=&bmpPalette[(int)data[i*2]&0xFF]; /* NOTE: should we read the i*2+1 ?*/
            buffer[i]=(unsigned int)( (tmp->Cr&0xFF) <<16 |
                    (tmp->Y&0xFF)<<8 |(tmp->Cb&0xFF));
        }
    }
    else
    {
        for(i=0;i<width;i++)
            ptrBmap[i]=(bmpPalette[data[i]&0xFF]).index;
    }
}

void bmp_read16BitRow(unsigned int * buffer,char * data,int width,int output_mode)
{
}

void bmp_read24BitRow(unsigned int * buffer,char * data,int width,int output_mode)
{
    int i,k;
    int r,g,b,Y1,Y2,Cr,Cb;
    int hasMore=width%2;
    char * ptrBmap=(char*)buffer;
    if(output_mode==BMP_MODE_VIDEO)
    {
        width/=2;    
        k=0;
        for(i=0;i<width;i++)
        {
    
            b=data[k++];
            g=data[k++];
            r=data[k++];
    
            Y1=RGB2Y(r,g,b);
            Cr=RGB2Cr(r,g,b);
            Cb=RGB2Cb(r,g,b);
    
            b=data[k++];
            g=data[k++];
            r=data[k++];
    
            Y2=RGB2Y(r,g,b);
            Cr=(Cr+(RGB2Cr(r,g,b)))>>1;
            Cb=(Cb+RGB2Cb(r,g,b))>>1;
    
            buffer[i]=(unsigned int)((Y2&0xFF)<<24 | (Cr&0xFF) <<16 | (Y1&0xFF)<<8 | (Cb&0xFF));
        }
    
        if(hasMore)
        {
            b=data[k++];
            g=data[k++];
            r=data[k++];
    
            Y1=RGB2Y(r,g,b);
            Cr=RGB2Cr(r,g,b);
            Cb=RGB2Cb(r,g,b);
            buffer[i>>1]=(unsigned int)( (Cr&0xFF) <<16  | (Y1&0xFF)<<8 |(Cb&0xFF));
        }
    }
    else
    {
        k=0;
        for(i=0;i<width;i++)
        {
    
            b=data[k++];
            g=data[k++];
            r=data[k++];
            ptrBmap[i]=get_nearest(r,g,b);
        }
    }
}

void bmp_read32BitRow(unsigned int * buffer,char * data,int width,int output_mode)
{
    /* TODO: not supported atm */
}

#define BMP_RLE_DISCOVER 0
#define BMP_RLE_ENCODED  1
#define BMP_RLE_ABSOLUTE 2
#define BMP_RLE_END_BMP  3

#define BMP_RLE_4BIT     2
#define BMP_RLE_8BIT     1
#define BMP_BITFIELD     3

static int rle_mode;
static int uses_rle;
static int nbToRead;
static char * bmp_tmpRLEBuff;
static int bmpIsRead;
static int posInfo;
static char charToDup;
//static int partialByte_buff;

static int (*bmp_rle_decode_fct)(int fd,char* buffer,int nbBytes);

int bmp_rle_4bit_fct(int fd,char* buffer,int nbBytes)
{
    char tag[2];
    int pos=0;
    int inputPos=0;
    int partialByte=0;
    int nbBytesRead;
    
    while(pos<nbBytes)
    {
        switch(rle_mode)
        {
            case BMP_RLE_DISCOVER:
                if(read(fd,tag,2)!=2)
                {
                    printk("[BMP_RLE] error reading tag\n");
                    return pos;
                }
                inputPos+=2;
                if(tag[0]==0)
                {
                    switch(tag[1])
                    {
                        case 0x0: /*endOfLine*/
                            //printk("End of line (pos=%d nbBytes=%d)\n",pos,nbBytes);
                            if(pos!=0)
                            {
                                if(partialByte)
                                    pos++;
                                for(;pos<nbBytes;pos++)
                                    buffer[pos]=0;
                            }
                            break;
                        case 0x1: /*endOfBmp*/
                            //printk("End of bmp\n");
                            if(partialByte)
                                    pos++;
                            for(;pos<nbBytes;pos++)
                                buffer[pos]=0;
                            rle_mode=BMP_RLE_END_BMP;
                            break;
                        case 0x2: /*Delta*/
                            printk("Delta in img\n");
                            break;
                        default: /*absolute mode*/
                            nbToRead=tag[1];
                            rle_mode=BMP_RLE_ABSOLUTE;
                            bmpIsRead=0;
                            posInfo=0;                            
                            break;
                    }
                }
                else
                {
                    rle_mode=BMP_RLE_ENCODED;
                    nbToRead=tag[0];
                    charToDup=tag[1];
                }
                break;

            case BMP_RLE_ENCODED:                
                if(partialByte)
                {
                    buffer[pos]= buffer[pos] | ((charToDup >> 4) & 0xF);
                    pos++;
                    nbToRead--;
                    charToDup = ((charToDup & 0xF)<< 4) | ((charToDup >> 4) & 0xF);
                    partialByte=0;
                }
                
                for(;pos<nbBytes && nbToRead>1;pos++,nbToRead-=2)
                    buffer[pos]=charToDup;
                                                                            
                if(nbToRead==1 && pos<nbBytes)
                {
                    partialByte=1;
                    buffer[pos]=charToDup&0xF0;
                    nbToRead=0;
                }
                
                if(nbToRead==0)
                    rle_mode=BMP_RLE_DISCOVER;
                break;

            case BMP_RLE_ABSOLUTE:
                if(!bmpIsRead)
                {
                    nbBytesRead=(nbToRead+1)/2;
                    nbBytesRead=((inputPos+nbBytesRead+1)/2)*2-inputPos;
                    //printk("RLE 4bit: %d nibbles => %d bytes\n",nbToRead,nbBytesRead);
                    if(read(fd,bmp_tmpRLEBuff,nbBytesRead)!=nbBytesRead)
                    {
                        printk("Error reading from file\n");
                        return pos;
                    }
                    bmpIsRead=1;
                    posInfo=0;
                    inputPos+=nbBytesRead;
                }
                
                if(partialByte)
                {
                    buffer[pos]= buffer[pos] | ((bmp_tmpRLEBuff[posInfo/2]>>4) & 0xF);
                    pos++;   
                    posInfo++;                 
                    partialByte=0;
                }                
                                        
                for(;pos<nbBytes && posInfo<nbToRead;)
                {
                    if(posInfo&0x1)
                        buffer[pos]=(bmp_tmpRLEBuff[posInfo/2]&0xF)<<4;
                    else
                        buffer[pos]=bmp_tmpRLEBuff[posInfo/2]&0xF0;
                        
                    posInfo++;
                    if(posInfo==nbToRead)
                    {
                        partialByte=1;
                        break;   
                    }
                    if(posInfo&0x1)
                        buffer[pos]=buffer[pos]|(bmp_tmpRLEBuff[posInfo/2]&0xF);
                    else
                        buffer[pos]=buffer[pos]|((bmp_tmpRLEBuff[posInfo/2]>>4)&0xF);                        
                    pos++;
                    posInfo++;
                }
                
                if(posInfo==nbToRead)
                    rle_mode=BMP_RLE_DISCOVER;
                                
                break;
                
            case BMP_RLE_END_BMP:
                for(;pos<nbBytes;pos++)
                    buffer[pos]=0;
                break;                
        }
    }
    return pos;
}

int bmp_rle_8bit_fct(int fd,char* buffer,int nbBytes)
{
    char tag[2];
    int pos=0;
    int inputPos=0;
    int nbBytesRead;
    while(pos<nbBytes)
    {
        switch(rle_mode)
        {
            case BMP_RLE_DISCOVER:
                if(read(fd,tag,2)!=2)
                {
                    printk("[BMP_RLE] error reading tag\n");
                    return pos;
                }
                inputPos+=2;
                if(tag[0]==0)
                {
                    switch(tag[1])
                    {
                        case 0x0: /*endOfLine*/
                            if(pos!=0)
                                for(;pos<nbBytes;pos++)
                                    buffer[pos]=0;
                            break;
                        case 0x1: /*endOfBmp*/
                            //printk("End of bmp\n");
                            for(;pos<nbBytes;pos++)
                                buffer[pos]=0;
                            rle_mode=BMP_RLE_END_BMP;
                            break;
                        case 0x2: /*Delta*/
                            printk("Delta in img\n");
                            break;
                        default: /*absolute mode*/
                            nbToRead=tag[1];
                            rle_mode=BMP_RLE_ABSOLUTE;
                            bmpIsRead=0;
                            posInfo=0;
                            break;
                    }
                }
                else
                {
                    rle_mode=BMP_RLE_ENCODED;
                    nbToRead=tag[0];
                    charToDup=tag[1];
                }
                break;

            case BMP_RLE_ENCODED: 
                for(;pos<nbBytes && nbToRead>0;pos++,nbToRead--)
                    buffer[pos]=charToDup;
                                                                            
                if(nbToRead==0)
                    rle_mode=BMP_RLE_DISCOVER;
                break;

            case BMP_RLE_ABSOLUTE:
                if(!bmpIsRead)
                {
                    nbBytesRead=((inputPos+nbToRead+1)/2)*2-inputPos;
                    printk("RLE_ABSOLUTE: %d asked => %d read to match word bounderie (pos=%d)\n",
                           nbToRead,nbBytesRead,inputPos);
                    if(read(fd,bmp_tmpRLEBuff,nbBytesRead)!=nbBytesRead)
                    {
                        printk("Error reading from file\n");
                        return pos;
                    }
                    inputPos+=nbBytesRead;
                    bmpIsRead=1;
                    posInfo=0;
                }
                for(;pos<nbBytes && posInfo<nbToRead;posInfo++,pos++)
                    buffer[pos]=bmp_tmpRLEBuff[posInfo];
                if(posInfo==nbToRead)
                    rle_mode=BMP_RLE_DISCOVER;
                break;
            case BMP_RLE_END_BMP:
                //printk("End of bmp\n");
                for(;pos<nbBytes;pos++)
                    buffer[pos]=0;
                break;
        }
    }
    return pos;
}

void bmp_initFileReader(int comp_mode)
{
    printk("[BMP] init RLE: comp_mode=%d\n",comp_mode);
    if(comp_mode)
    {
        uses_rle=1;
        rle_mode=BMP_RLE_DISCOVER;
        switch(comp_mode)
        {
            case BMP_RLE_4BIT:
                bmp_tmpRLEBuff=(char*)malloc(0x80);
                bmp_rle_decode_fct=bmp_rle_4bit_fct;
                break;
            case BMP_RLE_8BIT:
                bmp_tmpRLEBuff=(char*)malloc(0x100);
                bmp_rle_decode_fct=bmp_rle_8bit_fct;
                break;
            default:
                bmp_rle_decode_fct=NULL;
        }
    }
    else
    {
        uses_rle=0;
    }
}

int bmp_readFile(int fd,char* buffer,int nbBytes)
{
    if(uses_rle)
    {
        if(!bmp_tmpRLEBuff)
            return 0;
        if(!bmp_rle_decode_fct)
            return 0;
        return bmp_rle_decode_fct(fd,buffer,nbBytes);
    }
    else
    {
        return read(fd,buffer,nbBytes);
    }
}

MED_RET_T gfx_loadBmp_toPlane_center(char * filename,int plane)
{
    return gfx_loadBmp_toPlane(filename,plane,BMP_CENTER,0,0);
}

MED_RET_T gfx_loadBmp_toPlane_coord(char * filename,int plane,int x,int y)
{
    return gfx_loadBmp_toPlane(filename,plane,BMP_NO_CENTER,x,y);
}

MED_RET_T gfx_loadBmp_toMem_center(char * filename,char * dest,int w,int h,int output_mode)
{
    return gfx_loadBmp_toMem(filename,dest,w,h,output_mode,BMP_CENTER,0,0);
}
        
MED_RET_T gfx_loadBmp_toMem_coord(char * filename,char * dest,int w,int h,int output_mode,int x,int y)
{
    return gfx_loadBmp_toMem(filename,dest,w,h,output_mode,BMP_NO_CENTER,x,y);
}


MED_RET_T gfx_loadBmp_toPlane(char * filename,int plane,int center,int x,int y)
{
    int output_mode;
    switch(plane)
    {
        case VID1:
        case VID2:
            output_mode=BMP_MODE_VIDEO;
            break;
        case BMAP1:
        case BMAP2:
            output_mode=BMP_MODE_BITMAP;
            break;
        default:
            printk("Plane %d not supported\n");
            return -MED_ERROR;
    }
    
    printk("Calling bmp loader for %s on plane %d (mode = %s), %s, x=%d,y=%d\n",filename,plane,
           output_mode==BMP_MODE_VIDEO?"VIDEO":"BMAP",center==BMP_CENTER?"center":"no center",
           x,y);
            
    return gfx_loadBmpMain(filename,NULL,plane,BMP_DEST_OSD_BUFFER,output_mode,center,0,0,x,y);
}

MED_RET_T gfx_loadBmp_toMem(char * filename,char * dest,int w,int h,int output_mode,int center,int x,int y)
{
    printk("Calling bmp loader for %s to mem at %x (mode = %s), %s, (x=%d,y=%d), (w=%d,h=%d)\n",filename,dest,
           output_mode==BMP_MODE_VIDEO?"VIDEO":"BMAP",center==BMP_CENTER?"center":"no center",
           x,y,w,h);
            
    return gfx_loadBmpMain(filename,dest,0,BMP_DEST_MEM_BUFFER,output_mode,center,w,h,x,y);
}

/**********************************************************************************************
* main loader called by helper functions 
* Params:
* - dest is mandatory if dest_type is BMP_DEST_MEM_BUFFER, can by NULL otherwise
* - plane is used if dest_type is BMP_DEST_OSD_BUFFER, can by 0 otherwise
* - dest_type can be BMP_DEST_MEM_BUFFER or BMP_DEST_OSD_BUFFER
* - output_mode can be BMP_MODE_VIDEO or BMP_MODE_BITMAP, no check 
*        is made against plane val in BMP_DEST_OSD_BUFFER mode
*        in video mode only Y2CrY2Cb mode is supported
* - center : BMP_CENTER or BMP_NO_CENTER
* - width/height : only needed in BMP_DEST_MEM_BUFFER, plane info are used in this mode,
*        size of image in pixels
* - x/y : is used only in BMP_NO_CENTER, position of image in pixels
*********************************************************************************************/
MED_RET_T gfx_loadBmpMain(char * filename, char * dest,int plane,int dest_type,
                      int output_mode,int center,int width,int height,int x, int y)
{
    int fd;
    char tmp[fileHeader_size];
    int * tmp_int=(int*)tmp;
    int tmp_data;
    int i,j;
    int img_x,img_y;
    unsigned int * screenDirect;
    int rowSize;
    char * tmpRow;
    unsigned int * buff_offset;
    int scr_w,scr_h,scr_real_w;
    
    if(dest==NULL && dest_type==BMP_DEST_MEM_BUFFER)
        return -MED_ERROR;
        
    if(dest_type==BMP_DEST_MEM_BUFFER && (width<=0 || height<=0))
        return -MED_ERROR;
    
    if(center==BMP_NO_CENTER)
        x=y=0;
    
    if(dest_type==BMP_DEST_OSD_BUFFER && (plane<0 || plane>5))
        return -MED_ERROR;
        
    bmpPalette = NULL;
    bmp_tmpRLEBuff = NULL;

    if(dest_type==BMP_DEST_MEM_BUFFER)
    {
        scr_real_w=width;        
        scr_h=height;
        if(output_mode==BMP_MODE_VIDEO)
            scr_w=width/2;
        else
            scr_w=width/4;
    }
    else
    {
        gfx_planeGetSize(plane,&scr_w,&scr_h,NULL);
        if(output_mode==BMP_MODE_VIDEO)
            scr_real_w=scr_w*2;
        else
        {
            scr_real_w=scr_w;
            scr_w/=4;
        }
        
    }
    
    
    
    if ((fd = open(filename, O_RDONLY)) < 0 )
    {
        printk("[gfx_loadBmp]: can't open %s\n", filename);
        return -MED_ERROR;
    }

    if(read(fd,tmp,fileHeader_size)!=fileHeader_size)
    {
        printk("[gfx_loadBmp] error reading File Header\n");
        close(fd);
        return -MED_ERROR;
    }

    if(read(fd,&infoHeader,sizeof(bmp_info_header))!=sizeof(bmp_info_header))
    {
        printk("[gfx_loadBmp] error reading info Header\n");
        close(fd);
        return -MED_ERROR;
    }

    printk("File: %s\n",filename);
    fileHeader.type=(*tmp_int)&0xFFFF;
    fileHeader.size=((tmp_int[0]>>16)&0xFFFF)|(tmp_int[1]&0xFFFF);
    fileHeader.reserved1=(tmp_int[1]>>16)&0xFFFF;
    fileHeader.reserved2=tmp_int[2]&0xFFFF;
    fileHeader.offsetbits=((tmp_int[2]>>16)&0xFFFF)|(tmp_int[3]&0xFFFF);

    printk("File Header:\nType=%04x, Size=%08x, Res1=%04x, Res2=%04x, offsetbits=%08x\n",
    fileHeader.type,
    fileHeader.size,
    fileHeader.reserved1,
    fileHeader.reserved2,
    fileHeader.offsetbits);

    printk("Info Header:\nSize=%x, width=%x, height=%x, planes=%x, bitcount=%x\n"
            "SizeImage=%x, xpelspermeter=%x, ypelspermeter=%x\n"
            "colorsused=%x, colorsimportant=%x\n",
    infoHeader.size,
    infoHeader.width,
    infoHeader.height,
    infoHeader.planes,
    infoHeader.bitcount,
    infoHeader.compression,
    infoHeader.sizeimage,
    infoHeader.xpelspermeter,
    infoHeader.ypelspermeter,
    infoHeader.colorsused,
    infoHeader.colorsimportant);

    /* now testing headers value => can we handle the file ?*/

    if(infoHeader.compression > 3)
    {
        printk("[BMP ERROR] unsupported format for %s (compression=%d)\n",filename,infoHeader.compression);
        close(fd);
        return -MED_ERROR;
    }

    /*bit field and 16bit file ?*/
    if(infoHeader.compression == 3 && infoHeader.bitcount != 16)
    {
        printk("[BMP ERROR] %s uses bit field but isn't 16bit (%d bit)=> not supported\n",
               filename,infoHeader.bitcount);
        close(fd);
        return -MED_ERROR;
    }

    /* correct bit depth ? */
    tmp_data=infoHeader.bitcount;
    if(tmp_data != 1  && tmp_data != 4  &&
       tmp_data != 8  && //tmp_data != 16 &&
       tmp_data != 24 ) //&& tmp_data != 32 )
    {
        printk("[BMP ERROR] %s uses an unsupported bitcount : %d\n",
               filename,infoHeader.bitcount);
        close(fd);
        return -MED_ERROR;
    }

    /* checking width and height */
    if(infoHeader.width <= 0 || infoHeader.height <= 0
       || infoHeader.width > scr_real_w || infoHeader.height > scr_h)
    {
        printk("[BMP ERROR] %s uses bad size : w=%d,h=%d (screen: w=%d/%d,h=%d)\n",
               filename,infoHeader.width,infoHeader.height,
              scr_real_w,scr_w,scr_h);
        close(fd);
        return -MED_ERROR;
    }
   
    /* dealing with palette*/
    if(infoHeader.bitcount<16)
    {
        /*palette size determined using space btw header and img data*/
        int paletteSize=fileHeader.offsetbits-54;
        char * palette_buf;
        int r,g,b;
        palette_buf = (char*)malloc(paletteSize);
        if(!palette_buf)
        {
            printk("[BMP ERROR] error doing malloc of tmp buffer for palette (size=%d)\n",paletteSize);
            close(fd);
            return -MED_ERROR;
        }

        /*reading palette*/
        if(read(fd,palette_buf,paletteSize)!=paletteSize)
        {
            printk("[BMP ERROR] error reading palette from %s\n",filename);
            close(fd);
            return -MED_ERROR;
        }

        /*creating palette*/
        bmpPalette=(bmpColor*)malloc(bmp_numberColor(infoHeader.bitcount)*sizeof(bmpColor));
        if(!bmpPalette)
        {
            printk("[BMP ERROR] error doing malloc for palette (size=%d)\n",
                   bmp_numberColor(infoHeader.bitcount)*sizeof(bmpColor));
            close(fd);
            free(palette_buf);
            return -MED_ERROR;
        }

        printk("Reading palette: %d colors / %d\n",paletteSize,bmp_numberColor(infoHeader.bitcount));

        /* init palette with read data*/
        for(i=0,j=0;i<paletteSize;i+=4,j++)
        {
            b=palette_buf[i];
            g=palette_buf[i+1];
            r=palette_buf[i+2];
            //res=palette_buf[i+3];
            if(output_mode==BMP_MODE_VIDEO)
            {
                bmpPalette[j].Y=RGB2Y(r,g,b);
                bmpPalette[j].Cr=RGB2Cr(r,g,b);
                bmpPalette[j].Cb=RGB2Cb(r,g,b);
            }
            else
            {
                bmpPalette[j].index=get_nearest(r,g,b);
                //printk("for %d %d %d found index %d\n",r,g,b,bmpPalette[j].index);
            }
            
        }
        /*init rest of palette with white*/
        if(output_mode==BMP_MODE_VIDEO)
            for(i=j;i<bmp_numberColor(infoHeader.bitcount);i++)
                bmpPalette[i].Y=bmpPalette[i].Cr=bmpPalette[i].Cb=0;
        else
            for(i=j;i<bmp_numberColor(infoHeader.bitcount);i++)
                bmpPalette[i].index=0;

        free(palette_buf);
    }

    /* set file cursor to start of img */
    if(lseek(fd,fileHeader.offsetbits,SEEK_SET)!=fileHeader.offsetbits)
    {
        printk("[BMP ERROR] error seeking to img datafor file %s\n",filename);
        close(fd);
        if(bmpPalette) free(bmpPalette);
        return -MED_ERROR;
    }

    /*init a buffer of 1 row */
    rowSize=infoHeader.width*infoHeader.bitcount/8;
    while(8*rowSize<infoHeader.width*infoHeader.bitcount) rowSize++;
    while(rowSize%4) rowSize++;


    tmpRow=(char *)malloc(rowSize);
    if(!tmpRow)
    {
        printk("[BMP ERROR] error seeking to img datafor file %s\n",filename);
        close(fd);
        if(bmpPalette) free(bmpPalette);
        if(bmp_tmpRLEBuff) free(bmp_tmpRLEBuff);
        return -MED_ERROR;
    }

    /* reading data to screen */
    
    if(dest_type==BMP_DEST_MEM_BUFFER)
        screenDirect=(unsigned int *)dest;
    else
        screenDirect=(unsigned int *)gfx_planeGetBufferOffset(plane);

     for (j=0;j<scr_h;j++)
            for (i=0;i<scr_w;i++)
                screenDirect[j*scr_w + i] = output_mode==BMP_MODE_VIDEO?0x00800080:0;
    
    
    if(center==BMP_CENTER)
    {
        img_x=(scr_real_w-infoHeader.width)/2;
        img_y=(scr_h-infoHeader.height)/2;
    }
    else
    {
        img_x=x;
        img_y=y;
    }

    printk("[BMP info]: output x=%d, y=%d, w=%d, h=%d (screen: %d-%d,%d)\n",img_x,img_y,
           infoHeader.width,infoHeader.height,scr_w,scr_real_w,scr_h),

    buff_offset=screenDirect+((output_mode==BMP_MODE_VIDEO?img_x/2:img_x/4)+(img_y+infoHeader.height)*scr_w);
    
    /* init Compression */
    bmp_initFileReader(infoHeader.compression);   
    
        
    if(infoHeader.bitcount==16)
    {
        /*NOTE: need code for 16bit bmp */
    }
    else
    {
        int bitDepth=infoHeader.bitcount;
        int width=infoHeader.width;
        for(i=infoHeader.height-1;i>=0;i--)
        {
            /*read one row*/            
            if(bmp_readFile(fd,tmpRow,rowSize)!=rowSize)
            {
                printk("[BMP ERROR] error reading row %d of %s\n",i,filename);
                close(fd);
                if(bmpPalette) free(bmpPalette);
                if(bmp_tmpRLEBuff) free(bmp_tmpRLEBuff);
                free(tmpRow);
                return -MED_ERROR;
            }
            switch(bitDepth)
            {
                case 1 : bmp_read1BitRow(buff_offset,tmpRow,width,output_mode); break;
                case 4 : bmp_read4BitRow(buff_offset,tmpRow,width,output_mode); break;
                case 8 : bmp_read8BitRow(buff_offset,tmpRow,width,output_mode); break;
                case 24: bmp_read24BitRow(buff_offset,tmpRow,width,output_mode); break;
                case 32: bmp_read32BitRow(buff_offset,tmpRow,width,output_mode); break;
            }
            /* moving to next row in img */
            buff_offset-=scr_w;
        }
    }

    close(fd);
    if(bmpPalette) free(bmpPalette);
    if(bmp_tmpRLEBuff) free(bmp_tmpRLEBuff);
    free(tmpRow);
    return MED_OK;
}

long dist_3D(unsigned char r,unsigned char g,unsigned char b,
                unsigned char pr,unsigned char pg,unsigned char pb)
{
    long R,G,B;
    long res;

    R=abs(r-pr);
    G=abs(g-pg);
    B=abs(b-pb);
    
    res=R*R+G*G+B*B;
    return res;
}

extern int gui_pal[256][3];

int get_nearest(unsigned char r,unsigned char g,unsigned char b)
{
    int min_index=0;
    long min_val=0x7fffffffL;;
    long new_val;
    int i;
    
    /* DB0073 (219,000,115) */
    
    /*if(r==0xfe && g==0xfe && b==0xfe)
        return 0;*/
    
    for(i=1;i<256;i++)
    {
        new_val=dist_3D(r,g,b,gui_pal[i][0],gui_pal[i][1],gui_pal[i][2]);
        if(new_val==0)
        {
            min_index=i;
            min_val=new_val;
            break;
        }
        if(new_val<min_val)
        {
            min_val=new_val;
            min_index=i;
        }
    }
    
    return min_index;
}
