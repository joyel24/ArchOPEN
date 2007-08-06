
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{    
    FILE * infile;
    int c,count,count2; 
    
    infile = fopen(argv[1], "rb");
    count=count2=0;
    
    if (!infile)
    {
        printf("Can't open file '%s' for reading!\n", argv[1]);
        exit(1);
    }
    
    while(!feof(infile))
    {
        c=fgetc(infile);
        if(c==0)
            count++;
        
        if(c==0x3)
            count2++;
    }
    
    printf("Nb 00 : %04d 03: %04d - %s\n",count,count2,argv[1]);
    
    fclose(infile);
    
    return 0;
}
