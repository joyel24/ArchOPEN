/*
* test.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#include <api.h>
#include <sys_def/arch.h>
#include <sys_def/lang.h>

void app_main(int argc,char* argv)
{
    int ver,w,h,i;
    char res[256];

    printf("In test: %s\n",lang_getStr(STRLNG_OTHER));
    
    ver=getArch();
    getResolution(&w,&h);
    printf("We are on %s\n resolution is: (%d,%d)\n",ver==AV3XX_ARCH?"AV3XX":ver==GMINI4XX_ARCH?"Gmini4xx":
            ver==AV4XX_ARCH?"Av4xx":"UKN ARCH",
        w,h
    );
    
    
    
    for(i=0;i<2;i++)
    {
        browser_simpleBrowse("/",res);
        printf("res: %s\n",res);
    }
}
