#include <stdio.h>
#include <stdlib.h>

#if 1
#include "../../medios/include/gfx/gui_pal.h"
#else
#include "../../medios/include/gfx/gui_pal_DSC21.h"
#endif

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

};

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

void doPrint(int index,long val,unsigned char r,unsigned char g,unsigned char b,
                unsigned char pr,unsigned char pg,unsigned char pb)
{
    printf("i=%d dist=%d pix=[0x%02x,0x%02x,0x%02x] pal=[0x%02x,0x%02x,0x%02x]\n",index,val,
                r,g,b,
                pr,pg,pb);
}


int get_nearest(unsigned char r,unsigned char g,unsigned char b)
{
    int min_index=0;
    long min_val=0x7fffffffL;;
    long new_val;
    int i;
    
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

void outputStr(FILE * out,char * str)
{
    while(*str)
    {
        fputc(*str,out);
        str++;
    }
}

int main(int argc, char* argv[]) {
    int w,h,i,j,c,s;
    int stop=0;
    unsigned char rgb[3];
    int match_col;
    int binOut=0;
    char * str;
    
    int pal_conv[256];
    
    unsigned char * pal;
    unsigned char data;

    struct ico_head icon_head;
    struct ico_struct icon_data;
    struct ico_bmap_head icon_bitmap_head;
    
    FILE* infile;    

    infile = fopen(argv[1], "rwb");

    if (!infile)
    {
        printf("Can't open file '%s' for reading!\n", argv[1]);
        exit(1);
    }

    //parse BMP file

    s=fread(&icon_head,sizeof(struct ico_head),1,infile);

    if(s!=1)
    {
        printf("'%s' can't be read! (1)\n", argv[1]);
        exit(1);
    }

    printf("Ico head: head=%04x type=%04x count=%04x\n",
               icon_head.header,icon_head.type,icon_head.img_count);
    
    s=fread(&icon_data,sizeof(struct ico_struct),1,infile);

    if(s!=1)
    {
        printf("'%s' can't be read! (2)\n", argv[1]);
        exit(1);
    }

    printf("Icon data: size=%x,%x - c cnt=%x -res=%x- col planes=%x - bpp=%x - size=%x off=%x\n",
                icon_data.width,icon_data.height,icon_data.colour_count,icon_data.reserved,
                icon_data.color_planes,icon_data.bpp,icon_data.size,icon_data.offset);
    
    fseek(infile,icon_data.offset,SEEK_SET);
       
    s=fread(&icon_bitmap_head,sizeof(struct ico_bmap_head),1,infile);

    if(s!=1)
    {
        printf("'%s' can't be read! (3)\n", argv[1]);
        exit(1);
    }
            
    printf("biSize=%x, biWidth=%x, biHeight=%x, biPlanes=%x, biBitCount=%x, biCompression=%x, biSizeImage=%x, biXPelsPerMeter=%x, biYPelsPerMeter=%x, biClrUsed=%x, biClrImportant=%x\n",
        icon_bitmap_head.biSize,icon_bitmap_head.biWidth,icon_bitmap_head.biHeight,
        icon_bitmap_head.biPlanes,icon_bitmap_head.biBitCount,
        icon_bitmap_head.biCompression,icon_bitmap_head.biSizeImage,
        icon_bitmap_head.biXPelsPerMeter,icon_bitmap_head.biYPelsPerMeter,
        icon_bitmap_head.biClrUsed,icon_bitmap_head.biClrImportant);
    
    pal=(unsigned char*)malloc(256*4);
    
    s=fread(pal,256*4,1,infile);
    
    if(s!=1)
    {
        printf("'%s' can't be read! (4)\n", argv[1]);
        exit(1);
    }
    
    for(i=0;i<256;i++)
    {
        pal_conv[i]=get_nearest(pal[i*4],pal[i*4+1],pal[i*4+2]);
        printf("%d (%d,%d,%d) -> %d (%d,%d,%d) diff=%d\n",i,pal[i*4],pal[i*4+1],pal[i*4+2],
               pal_conv[i],gui_pal[pal_conv[i]][0],gui_pal[pal_conv[i]][1],gui_pal[pal_conv[i]][2],
               dist_3D(pal[i*4],pal[i*4+1],pal[i*4+2],
                      gui_pal[pal_conv[i]][0],gui_pal[pal_conv[i]][1],gui_pal[pal_conv[i]][2]));
    }
    
    for(i=0;i<icon_data.width*icon_data.height;i++)
    {
        data=fgetc(infile);
        fseek(infile,-1,SEEK_CUR);
        fputc(pal_conv[data],infile);
    }
    
    
    fclose(infile);
    
    return 0;
}
