
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{    
    FILE * infile;
    FILE * outfile; 
    int c,count,count2; 
    
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
    
    while(!feof(infile))
    {
        c=fgetc(infile);
        if(c==0)
            fputc(0x3,outfile);
        else if(c==0x3)
            fputc(0x0,outfile);
        else
            fputc(c,outfile);
            
    }
    
    fclose(infile);
    fclose(outfile);
    
    return 0;
}
