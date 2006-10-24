/*
*   gio_lcd.cpp
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

#include <HW/gio_connection.h>
#include <HW/HW_connection.h>

GIO_CONN::GIO_CONN(HW_connection * hwConn,int num,char * name):gpio_port(num,name)
{
    state = 0;  /* default => btn not pressed */
    this->hwConn=hwConn;
}

void GIO_CONN::set_gpio(void)
{

}

void GIO_CONN::clear_gpio(void)
{

}

bool GIO_CONN::is_set(void)
{
    return this->hwConn->isConnected(gpio_num);
}
