
#include <stdio.h>
#include <stdlib.h>

#define AVLO_C

#include "../../include/sys_def/colordef_DSC25.h"
#include "../../apps/avlo/include/avlo_cfg.h"
#include "../../apps/avlo/include/AV3XX_default_cfg.h"

#include "parse_cfg.h"

#define SCREEN_REAL_WIDTH 320
#define SCREEN_HEIGHT     240

void do_parse(struct avlo_cfg * cfg);
void parse_long(rgb_color * color,char * value,char * name);

#define ITEM_TEST(NAME,VAR) if(!strcmp(item,NAME)) {cfg->VAR=atoi(value); nb++;}
#define ITEM_LONG_TEST(NAME,VAR) if(!strcmp(item,NAME)) {parse_long(&cfg->VAR,value,NAME); nb++;}

extern char item_buff[MAX_TOKEN+1];
extern char value_buff[MAX_TOKEN+1];

void do_parse(struct avlo_cfg * cfg)
{
    char *item=item_buff;
    char *value=value_buff;
    int pos;
    int nb=0;
    while (1) 
    {
        if (!nxt_cfg(item,value)) break;
        
        ITEM_TEST("menu_x",menu_x)
        else ITEM_TEST("menu_y",menu_y)
        else ITEM_TEST("status_x",status_x)
        else ITEM_TEST("status_y",status_y)
        else ITEM_TEST("bar_x",bar_x)
        else ITEM_TEST("bar_y",bar_y)
        else ITEM_TEST("bar_w",bar_w)
        else ITEM_TEST("bar_h",bar_h)
        else ITEM_TEST("usb_x",usb_x)
        else ITEM_TEST("usb_y",usb_y)
        else ITEM_TEST("bat_x",bat_x)
        else ITEM_TEST("bat_y",bat_y)
        else ITEM_LONG_TEST("color_txt",color_txt)
        else ITEM_LONG_TEST("color_box",color_box)
        else ITEM_LONG_TEST("color_sel",color_sel)
        else ITEM_LONG_TEST("color_load",color_load)
        else ITEM_LONG_TEST("color_wait",color_wait)
        else ITEM_LONG_TEST("color_msg_box_0",color_msg_box_0)
        else ITEM_LONG_TEST("color_msg_box_1",color_msg_box_1)
        else ITEM_LONG_TEST("color_pwr_charge",color_pwr_charge)
        else ITEM_LONG_TEST("color_pwr_l0",color_pwr_l0)
        else ITEM_LONG_TEST("color_pwr_l1",color_pwr_l1)
        else ITEM_LONG_TEST("color_pwr_l2",color_pwr_l2)
        else ITEM_LONG_TEST("color_pwr_l3",color_pwr_l3)
        else ITEM_LONG_TEST("color_pwr_frame_dc_on",color_pwr_frame_dc_on)
        else ITEM_LONG_TEST("color_pwr_frame_dc_off",color_pwr_frame_dc_off)
        else
            printf("unknown item type: %s on line %d \n",item,line_num);  
    }
    printf("found %d param\n",nb); 
}

#define SEARCH_NXT(DEST,INIT,CHAR) {  \
    char * __ptr; \
    for(__ptr = INIT;*__ptr!='\0' && *__ptr!=CHAR;__ptr++) /*nothing*/; \
    if(*__ptr==CHAR) {DEST=INIT;*__ptr='\0';INIT=__ptr+1;} \
}

void parse_long(rgb_color * color,char * value,char * name)
{
    char * ptr_r=NULL;
    char * ptr_g=NULL;
    char * ptr_b=NULL;
    SEARCH_NXT(ptr_r,value,',');
    if(*value)
    {
        SEARCH_NXT(ptr_g,value,',');
        if(*value)
        {
            ptr_b=value;           
            if(ptr_r && ptr_g && ptr_b)
            {
                color->r = atoi(ptr_r);
                color->g = atoi(ptr_g);
                color->b = atoi(ptr_b);
                printf("found: (%d,%d,%d) for '%s'\n",color->r,color->g,color->b,name);
            }
        }
    }
    
    
    
}

struct avlo_cfg cfg;

void calcYCrCb(int *y,int * cr, int * cb,unsigned char *rgb)
{
    int r,g,b;
    r  = rgb[0];
    g  = rgb[1];
    b  = rgb[2];
    
    *y = (306*r + 601*g + 117*b) >> 10 ; 
    *cb = ((-173*r -339*g + 512*b) >> 10) + 128;
    *cr = ((512*r - 428*g - 84*b) >> 10) + 128;
            
    if (*y < 0)* y = 0;
    if (*y > 255) *y = 255;
    if (*cb < 0) *cb = 0;
    if (*cb > 255) *cb = 255;
    if (*cr < 0) *cr = 0;
    if (*cr > 255) *cr = 255;
}

int main(int argc, char* argv[]) {
    int w,h,i,j,c,ept;
    int stop=0;
    int count;
    int binOut;
    unsigned char rgb[3];
    int y1,cr1,cb1;
    int y2,cr2,cb2;
    int cr,cb;
    
    FILE * infile;
    FILE * outfile; 

    w=SCREEN_REAL_WIDTH;h=SCREEN_HEIGHT;
    
    char * ptr = (char*) &cfg;
    
    if(argc != 4)
    {
        fprintf(stderr,"Info: %s source destination layout\n",argv[0]);
        exit(1);
    }   

    infile = fopen(argv[1], "rb");

    if (!infile)
    {
        printf("Can't open file '%s' for reading!\n", argv[1]);
        exit(1);
    }
    
    outfile = fopen(argv[2], "wb");
    
    if (!outfile)
    {
        printf("Can't open file '%s' for writing!\n", argv[2]);
        fclose(infile);
        exit(1);
    }
    
    /* reading layout */
    if(!file_open(argv[3]))
    {
        printf("Can't open file '%s' for reading!\n", argv[3]);
        fclose(infile);
        fclose(outfile);
        exit(1);
    }
    memcpy(&cfg,&default_cfg,sizeof(struct avlo_cfg));
    do_parse(&cfg);
    file_close();
    
    printf("Expecting img size: (w=%d,h=%d) struct size=%d => file size=%d\n",w,h,
        sizeof(struct avlo_cfg),w*h*2+sizeof(struct avlo_cfg));
    
    for(i=0;i<h && !stop;i++)
    {
        count=0;
        for(j=0;j<w;)
        {
            if (feof(infile)) {fprintf(stderr,"\nEOF!\n\n");stop=1;break;}
            c = fread(rgb, 1, 3, infile);
            if (c == 0) {fprintf(stderr,"\nNo data!\n\n");stop=1;break;}
            if (c != 3) {fprintf(stderr,"\nMismatch!\n\n");stop=1;break;}
            
            j++;
            
            calcYCrCb(&y1,&cr1,&cb1,rgb);
            
            if (feof(infile)) {fprintf(stderr,"\nEOF!\n\n");stop=1;break;}
            c = fread(rgb, 1, 3, infile);
            if (c == 0) {fprintf(stderr,"\nNo data!\n\n");stop=1;break;}
            if (c != 3) {fprintf(stderr,"\nMismatch!\n\n");stop=1;break;}
            
            j++;
            
            calcYCrCb(&y2,&cr2,&cb2,rgb);
            
            cb=(cb1+cb2)/2;
            cr=(cr1+cr2)/2;
            
            fputc(cb,outfile);
            fputc(y1,outfile);
            fputc(cr,outfile);
            fputc(y2,outfile); 

            count+=2;
        }
        
        ept=32-w%32;
        if(w%32)
        {
            for(ept=(32-w%32)/2;ept>0;ept--)
            {
                fputc(0x80,outfile);
                fputc(0xFF,outfile);
                fputc(0x80,outfile);
                fputc(0x80,outfile);
                printf(". ");
            }
        }
            
    }
       
    /* adding cfg struct */
    fwrite(ptr,1,sizeof(struct avlo_cfg),outfile);
    fclose(infile);
    fclose(outfile);
    
    return 0;
}
