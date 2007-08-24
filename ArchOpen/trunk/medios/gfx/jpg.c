/*
*   medios/gfx/jpg.c
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
#include <kernel/timer.h>

#include <gfx/graphics.h>
#include <gfx/jpg.h>

#include <driver/lcd.h>

#include <fs/stdfs.h>

#include <sys_def/errors.h>

extern struct graphicsBuffer VIDEO_1;

MED_RET_T gfx_loadJpg(char * filename,JMETHOD(void, progress_draw, (j_common_ptr)))
{
   
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    struct jpeg_progress_mgr prog_mgr;
    int img_file;
    unsigned char * offset;
    JSAMPROW rowptr[1];
    int scale[]={2,4,8};
    int i,j,x,y,w,h;
    unsigned int * screenDirect;
    unsigned int tick_start;
    int ret_val;
    
    tick_start=tmr_getTick();
    
    gfx_planeGetSize(VID1,&w,&h,NULL);
        
    /* Initialize the JPEG decompression object with default error handling. */
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    
    if ((img_file = open(filename, O_RDONLY)) < 0 )
    {
        printk("[gfx_loadJpg]: can't open %s\n", filename);
        return -MED_ERROR;
    }
    
    jpeg_stdio_src(&cinfo, img_file);
    
    if(progress_draw)
    {
        prog_mgr.progress_monitor=progress_draw;
        cinfo.progress=&prog_mgr;
    }
    
    ret_val=jpeg_read_header(&cinfo,TRUE);
    
    printk("[jpeg_read_header] get %d\n",ret_val);
    
    if(ret_val!=1)
    {
        jpeg_destroy_decompress(&cinfo);
        close(img_file);
        return -MED_ERROR;
    }
    
    cinfo.out_color_space=JCS_CUST;
    
    printk("[gfx_loadJpg]: input image w=%d h=%d\n",cinfo.image_width,cinfo.image_height);
    
    if(cinfo.image_width > SCREEN_WIDTH || cinfo.image_height > SCREEN_HEIGHT)
    {
        for(i=0;i<3;i++)
            if((cinfo.image_width/scale[i])<SCREEN_WIDTH && (cinfo.image_height/scale[i])<SCREEN_HEIGHT)
                break;
        if(i==3)
        {
            printk("[gfx_loadJpg]: image too big %s\n", filename);
            close(img_file);
            return -MED_ERROR;
        }
        else
            cinfo.scale_denom=scale[i];
    }
 
    /* changes proposed by tgb */       
    cinfo.two_pass_quantize = FALSE;
    cinfo.dither_mode = JDITHER_NONE;
    cinfo.desired_number_of_colors = 256;
    cinfo.dct_method = JDCT_FASTEST;
    cinfo.do_fancy_upsampling = FALSE;
    /**********************************/
    
    
    
    jpeg_start_decompress(&cinfo);
      
    screenDirect=(unsigned int *)gfx_planeGetBufferOffset(VID1);
    
    for (j=0;j<SCREEN_HEIGHT;j++)
        for (i=0;i<SCREEN_WIDTH;i++)
            screenDirect[j*SCREEN_WIDTH + i] = 0x00800080;
    
    x=(SCREEN_WIDTH-cinfo.output_width)/2;
    y=(SCREEN_HEIGHT-cinfo.output_height)/2;
    
    printk("[gfx_loadJpg]: output x=%d, y=%d, w=%d, h=%d\n",x,y,cinfo.output_width,cinfo.output_height),
    
    offset=(char*)(screenDirect+x/2+y*w);
    
    
    i=0;
    while(cinfo.output_scanline < cinfo.output_height)
    {
        rowptr[0] = (JSAMPROW)offset;
        if(jpeg_read_scanlines(&cinfo, rowptr,1))
            offset+=w*4;
    }
    
    printk("[gfx_loadJpg]: took %d tick\n",tmr_getTick()-tick_start);
    /*gfx_setPlane(BMAP1);*/
    
    jpeg_destroy_decompress(&cinfo);
    close(img_file);
    return MED_OK;

}
