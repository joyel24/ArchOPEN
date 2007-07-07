/* 
*   include/kernel/pipes.h
*
*   MediOS project
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __PIPES_H
#define __PIPES_H

#include <kernel/thread.h>

#define PIPE_SIZE 16384
#define PIPE_SIZE_MASK 16383

struct pipe
{
    unsigned long nIN;
    unsigned long nOUT;
    THREAD_INFO * myThread;
    unsigned char buffer [PIPE_SIZE];    
};


void pipeWrite (struct pipe* pPipe, void* _pData, unsigned long nBytes);
void pipeRead  (struct pipe* pPipe, void* _pData, unsigned long nBytes);
int  pipe_hasBytes(struct pipe* pPipe);
void pipeInit(struct pipe* pPipe);
void pipeInitWithThread(struct pipe* pPipe,THREAD_INFO * ptr);

#endif
