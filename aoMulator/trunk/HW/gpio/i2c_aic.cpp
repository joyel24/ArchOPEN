/*
*   i2c_aic.cpp
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <stdlib.h>
#include <stdio.h>

#include <time.h>

#include <emu.h>

#include <HW/i2c_aic.h>

i2c_aic::i2c_aic(void):i2c_device(0x36,"AIC")
{
    
}

int i2c_aic::read(void)
{
    
    return 0;
}

void i2c_aic::write(int val)
{
    
}

void i2c_aic::start(int direction)
{

}

void i2c_aic::stop(void)
{
    
}
