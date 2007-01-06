
#include <stdio.h>
#include <stdlib.h>

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

    if(argc != 5)
    {
        fprintf(stderr,"Info: %s source destination w h\n",argv[0]);
        exit(1);
    }   

    w=atoi(argv[3]);
    h=atoi(argv[4]);
    
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
    
        
    printf("Expecting img size: (w=%d,h=%d)\n",w,h);
    
    for(i=0;i<h && !stop;i++)
    {
        count=0;
        for(j=0;j<w;)
        {
            if (feof(infile)) {stop=1;break;}
            c = fread(rgb, 1, 3, infile);
            if (c == 0) {stop=1;break;}
            if (c != 3) {fprintf(stderr,"\nMismatch!\n\n");stop=1;break;}
            
            j++;
            
            calcYCrCb(&y1,&cr1,&cb1,rgb);
            
            if (feof(infile)) {stop=1;break;}
            c = fread(rgb, 1, 3, infile);
            if (c == 0) {stop=1;break;}
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
    
    fclose(infile);
    fclose(outfile);
    
    return 0;
}
