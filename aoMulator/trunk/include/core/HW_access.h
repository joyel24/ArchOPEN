/*
*   hw_access.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __HW_ACCESS_H
#define __HW_ACCESS_H

#include <emu.h>

class HW_access {

protected:
    bool exit_on_not_match;
    bool data_abt_on_not_match;

public:
    HW_access(uint32_t start,uint32_t end, char * name);

    uint32_t        start;
    uint32_t        end;
    char *          name;
    bool full_size;

    virtual uint32_t read(uint32_t addr,int size){return 0;};
    virtual void write(uint32_t addr,uint32_t val,int size){};
    virtual void printString(uint32_t addr){};
    virtual void printString(uint32_t addr,int size){};
};

#endif

