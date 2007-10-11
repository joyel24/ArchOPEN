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



void bmp_read1BitRow(unsigned int * buffer,char * data,int width)
{
    int i,j,k;;
    bmpColor * tmp,*tmp2;
    int hasMore=width%8;
    char dataVal;

    printk("width=%d hasMore=%d w=%d\n",width,hasMore,width/8);

    width/=8;
    j=0;

    for(i=0;i<width;i++)
    {
        dataVal=data[i];
        for(k=0;k<4;k++)
        {
            tmp=&bmpPalette[dataVal&0x80];
            dataVal<<=1;
            tmp2=&bmpPalette[dataVal&0x80];
            dataVal<<=1;
            buffer[j]=(unsigned int)((tmp2->Y&0xFF)<<24 | (((tmp->Cr+tmp2->Cr)>>1)&0xFF) <<16 |
                    (tmp->Y&0xFF)<<8 | (((tmp->Cb+tmp2->Cb)>>1)&0xFF));
            j++;
        }
    }

    if(hasMore)
    {
        //printk("hasmore: %d\n",hasMore);
        dataVal=data[i];
        while(hasMore>1)
        {
            tmp=&bmpPalette[dataVal&0x80];
            dataVal<<=1;
            tmp2=&bmpPalette[dataVal&0x80];
            dataVal<<=1;
            buffer[j]=(unsigned int)((tmp2->Y&0xFF)<<24 | (((tmp->Cr+tmp2->Cr)>>1)&0xFF) <<16 |
                    (tmp->Y&0xFF)<<8 | (((tmp->Cb+tmp2->Cb)>>1)&0xFF));
            j++;
            hasMore-=2;
        }

        if(hasMore==1)
        {
            //printk("still to put\n");
            tmp=&bmpPalette[dataVal&0x80];
            buffer[j]=(unsigned int)(  (tmp->Cr&0xFF)<<16 |
                    (tmp->Y&0xFF)<<8 |(tmp->Cb&0xFF));
            j++;
        }
    }
}

void bmp_read4BitRow(unsigned int * buffer,char * data,int width)
{
    int i;
    bmpColor * tmp,*tmp2;

    int hasMore=width%2;
    char dataVal;

    i=0;

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

void bmp_read8BitRow(unsigned int * buffer,char * data,int width)
{
    int i;
    bmpColor * tmp,*tmp2;
    int hasMore=width%2;

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

void bmp_read16BitRow(unsigned int * buffer,char * data,int width)
{
}

void bmp_read24BitRow(unsigned int * buffer,char * data,int width)
{
    int i,k;
    int r,g,b,Y1,Y2,Cr,Cb;
    int hasMore=width%2;
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

void bmp_read32BitRow(unsigned int * buffer,char * data,int width)
{
    /* TODO: not supported atm */
}

#define BMP_RLE_DISCOVER 0
#define BMP_RLE_ENCODED 1
#define BMP_RLE_ABSOLUTE 2

static int rle_mode;
static int uses_rle;
static int nbToRead;
static char bmp_tmpBuff[0x100];
static int bmpIsRead;
static int posInfo;

void bmp_initFileReader(int hasRle)
{
    if(hasRle)
    {
        uses_rle=1;
        rle_mode=BMP_RLE_DISCOVER;
    }
    else
    {
        uses_rle=0;
    }
}

int bmp_readFile(int fd,char* buffer,int nbBytes)
{
    char tag[2];
    int bmp_rle_done=0;
    int nbRead;
    char charToDup;
    int i;
    char buff[0x100];

    if(uses_rle)
    {
        while(!bmp_rle_done)
        {
            switch(rle_mode)
            {
                case BMP_RLE_DISCOVER:
                    if(read(fd,tag,2)!=2)
                    {
                        printk("[BMP_RLE] error reading tag\n");
                        return 0;
                    }
                    if(tag[0]==0)
                    {
                        switch(tag[1])
                        {
                            case 0x0: /*endOfLine*/
                                printk("End of line\n");
                                break;
                            case 0x1: /*endOfBmp*/
                                printk("End of bmp\n");
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

                case BMP_RLE_ENCODED: /* attention ne marche pas car il faut diminuer nbBytes*/
                    for(i=0;i<nbBytes && nbToRead>0;i++,nbToRead--)
                        buffer[i]=charToDup;
                    if(nbToRead==0)
                        rle_mode=BMP_RLE_DISCOVER;
                    break;

                case BMP_RLE_ABSOLUTE:
                    if(!bmpIsRead)
                    {
                        if(read(fd,bmp_tmpBuff,nbToRead)!=nbToRead)
                        {
                            printk("Error reading from file\n");
                            return 0;
                        }
                        bmpIsRead=1;
                    }
                    for(i;nbBytes>0 && nbToRead>0;nbBytes--,nbToRead--)
                    {
                        
                    }
                    break;
            }
        }
    }
    else
    {
        return read(fd,buffer,nbBytes);
    }
}

MED_RET_T gfx_loadBmp(char * filename)
{
    int fd;
    bmp_info_header infoHeader;
    bmp_file_header fileHeader;
    char tmp[fileHeader_size];
    int * tmp_int=(int*)tmp;
    int tmp_data;
    int i,j;
    int img_x,img_y;
    unsigned int * screenDirect;
    int rowSize;
    char * tmpRow;
    unsigned int * buff_offset;
    int scr_w,scr_h;

    bmpPalette = NULL;

    /*reading VID1 plane info*/
    gfx_planeGetSize(VID1,&scr_w,&scr_h,NULL);

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

    /* Compression */
    if(infoHeader.compression == 1 || infoHeader.compression == 2)
    {
        printk("[BMP ERROR] %s is a RLE file => not supported\n",filename);
        close(fd);
        return -MED_ERROR;
    }

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
       || infoHeader.width > scr_w || infoHeader.height > scr_h)
    {
        printk("[BMP ERROR] %s uses bad size : w=%d,h=%d (screen: w=%d,h=%d)\n",
               filename,infoHeader.width,infoHeader.height,
              scr_w,scr_h);
        close(fd);
        return -MED_ERROR;
    }

    /* dealing with palette*/
    if(infoHeader.bitcount<16)
    {
        /*palette size determined using space btw header and img data*/
        int paletteSize=fileHeader.offsetbits-54;
        char * palette_buf;
        int r,g,b,res;
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
            res=palette_buf[i+3];
            bmpPalette[j].Y=RGB2Y(r,g,b);
            bmpPalette[j].Cr=RGB2Cr(r,g,b);
            bmpPalette[j].Cb=RGB2Cb(r,g,b);
            printk("%d: (%02x,%02x,%02x-%02x) -> (%02x,%02x,%02x)\n",j,r,g,b,res,
                   bmpPalette[i].Y,bmpPalette[i].Cr,bmpPalette[i].Cb);
        }
        /*init rest of palette with white*/
        for(i=j;i<bmp_numberColor(infoHeader.bitcount);i++)
            bmpPalette[i].Y=bmpPalette[i].Cr=bmpPalette[i].Cb=0;

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
        return -MED_ERROR;
    }

    /* reading data to screen */
    screenDirect=(unsigned int *)gfx_planeGetBufferOffset(VID1);

    for (j=0;j<scr_h;j++)
        for (i=0;i<scr_w;i++)
            screenDirect[j*scr_w + i] = 0x00800080;

    img_x=(SCREEN_WIDTH-infoHeader.width)/2;
    img_y=(scr_h-infoHeader.height)/2;

    printk("[BMP info]: output x=%d, y=%d, w=%d, h=%d (screen: %d-%d,%d)\n",img_x,img_y,
           infoHeader.width,infoHeader.height,scr_w,SCREEN_WIDTH,scr_h),

    //buff_offset=&screenDirect[img_x/2+(img_y+infoHeader.height-1)*scr_w];
    buff_offset=screenDirect+infoHeader.height*scr_w;

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
            if(read(fd,tmpRow,rowSize)!=rowSize)
            {
                printk("[BMP ERROR] error reading row %d of %s\n",i,filename);
                close(fd);
                if(bmpPalette) free(bmpPalette);
                free(tmpRow);
                return -MED_ERROR;
            }

            /* converting 1 row */
            switch(bitDepth)
            {
                case 1 : bmp_read1BitRow(buff_offset,tmpRow,width); break;
                case 4 : bmp_read4BitRow(buff_offset,tmpRow,width); break;
                case 8 : bmp_read8BitRow(buff_offset,tmpRow,width); break;
                case 24: bmp_read24BitRow(buff_offset,tmpRow,width); break;
                case 32: bmp_read32BitRow(buff_offset,tmpRow,width); break;
            }
            /* moving to next row in img */
            buff_offset-=scr_w;
        }
    }

    close(fd);
    printk("bf free1\n");
    if(bmpPalette) free(bmpPalette);
    printk("bf free2\n");
    free(tmpRow);
    return MED_OK;
}
