/*
*   tools/lang/script/mkLng.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <stdio.h>
#include <stdlib.h>

#define MK_LNG_C

#include "../../../medios/include/kernel/lang.h"

int main(int argc, char* argv[])
{
    int fSize,nbXfer,numLng;
    FILE* infile;
    FILE* outfile;
    char * inBuff;

    if (argc != 4)
    {
        fprintf(stderr,"Info: %s preprocessed lng destination \n",argv[0]);
        exit(1);
    }

    infile = fopen(argv[1], "rb");

    if (!infile)
    {
        printf("Can't open file '%s' for reading!\n", argv[1]);
        exit(1);
    }
    
    numLng=atoi(argv[3]);
    
    fseek(infile,0,SEEK_END);
    fSize=ftell(infile);
    fseek(infile,0,SEEK_SET);
    inBuff=(char*)malloc(fSize);
    
    if(!inBuff)
    {
        printf("Error allocating %d bytes\n",fSize);
        fclose(infile);
        exit(1);
    }
    
    nbXfer=fread(inBuff,1,fSize,infile);
    if(nbXfer!=fSize)
    {
        printf("Error read from %s (read %d/%d)\n",argv[1],nbXfer,fSize);
        free(inBuff);
        fclose(infile);
        exit(1);   
    }
    
    fclose(infile);
    outfile = fopen(argv[2], "wb");

    if (!outfile)
    {
        printf("Can't open file '%s' for writting!\n", argv[2]);
        free(inBuff);
        fclose(infile);
        exit(1);
    }
    
    /* writting header*/
    fputs(LNG_HEADER,outfile);
    /* writing version */
    fputc(LNG_VERSION,outfile);
    
    fputc(numLng,outfile);
    
    nbXfer=fwrite(inBuff,1,fSize,infile);
    if(nbXfer!=fSize)
    {
        printf("Error read from %s (read %d/%d)\n",argv[1],nbXfer,fSize);
        free(inBuff);        
        fclose(outfile);
        exit(1);   
    }
    /*writting last entry */
    fputc((LNG_LAST_ENTRY>>8)&0xFF,outfile);
    fputc(LNG_LAST_ENTRY&0xFF,outfile);
    
    free(inBuff);  
    fclose(outfile);
}
    
