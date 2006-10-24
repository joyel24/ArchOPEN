/*
*   i2c_RTC.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __I2C_RTC_H
#define __I2C_RTC_H

#include <emu.h>
#include <HW/i2c_device.h>

class i2c_RTC:i2c_device {
    public:
        i2c_RTC(void);

        int read(void);
        void write(int val);

        void start(int direction);
        void stop(void);

        i2c_device * nxt;

        int address;
        char * name;
    private:
        bool has_address;
        int index;

        void readTime(void);

        int int2bcd(int v);
        int bcd2int(int v);

        int rtc_reg[0x42];
};

#endif /* __I2C_RTC_H */
