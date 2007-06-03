/*
* tools/createBin.c
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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc,char ** argv)
{
    FILE * fd;    
    char buff[10];
    struct stat fd_info;
    int size,i;
    
    //int fstatfs(int fd, struct statfs *buf);
    
    if(argc != 2)
        return 0;
    
    fd=fopen(argv[1], "r+");
    if (!fd)
    {
        printf("Can't open file '%s' for reading!\n", argv[1]);
        exit(1);
    }
    
    fseek(fd,4,SEEK_SET);
    fread(&buff,1,6,fd);
    
    if(strncmp(buff,"MEDIOS",6))
    {
        printf("Bad magic!\n");
        fclose(fd);
        exit(1);
    }
    printf("Magic OK\n");
    
    /* reading size */
    if(stat(argv[1],&fd_info)!=0)
    {
        printf("Can't read size\n");
        fclose(fd);
        exit(0);
    }
    size=fd_info.st_size;
    printf("File size: %d-%x\n",size,size);
    
    fseek(fd,12,SEEK_SET);
    for(i=0;i<4;i++)
    {    
        fputc(size&0xFF,fd);
        size>>=8;
    }
    
    printf("\nSize written in %s\n",argv[1]);        
    
    fclose(fd);
    return 0;
}
