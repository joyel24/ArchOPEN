/*
*   kernel/driver/ata_featuresList.h
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

struct ata_features featTab82_85[16]= {
{ NULL                   ,0,0},
{ "NOP"                  ,1,0},
{ "READ BUFFER"          ,1,0},
{ "WRITE BUFFER"         ,1,0},
{ NULL                   ,0,0},
{ "Host protected area"  ,1,0},
{ "DEVICE RESET"         ,1,0},
{ "SERVICE Int"          ,1,0},
{ "Release Int"          ,1,0},
{ "Look-ahead"           ,1,0},
{ "Write cache"          ,1,0},
{ "PACKET cmd"           ,1,1},
{ "Mandat. Power Mgt"    ,1,0},
{ "Removable media feat.",1,0},
{ "Security mode"        ,1,0},
{ "SMART"                ,1,0}
};
struct ata_features featTab83_86[16]= {
    { NULL                   ,0,0},
    { NULL                   ,0,0},
    { "FLUSH CACHE EXT"      ,1,0},
    { "Mandat. FLUSH CACHE"  ,1,0},
    { "Device config overlay",1,0},
    { "48-bit Address"       ,1,0},
    { "Auto accoustic mgt"   ,1,0},
    { "SET MAX security"     ,1,0},
    { NULL                   ,0,0},
    { "SET FEATURES required to spinup after power up",1,0},
    { "Power up in Stdby mode",1,0},
    { "Removable media Status notification",1,0},
    { "Adv. Power Mgt."      ,1,0},
    { "CFA feature set"      ,1,0},
    { "READ/WRITE DMA QUEUED",1,0},
    { "DOWNLOAD MICROCODE"   ,1,0}
};

struct ata_features featTab84_87[16]= {
    { NULL                   ,0,0},
    { NULL                   ,0,0},
    { "IDLE IMMEDIATE with UNLOAD FEATURE",1,0},
    { NULL                   ,0,0},
    { NULL                   ,0,0},
    { "URG bit for WRITE STREAM DMA EXT and WRITE STREAM EXT",1,0},
    { "URG bit for READ STREAM DMA EXT and READ STREAM EXT",1,0},
    { "64bit World Wide name",1,0},
    { "WRITE DMA QUEUED FUA EXT",1,0},
    { "WRITE DMA FUA EXT and WRITE MULTIPLE FUA EXT",1,0},
    { "General purpose logging",1,0},
    { "Streaming feature"    ,1,0},
    { "Media card pass through",1,0},
    { "Media serial number"  ,1,0},
    { "SMART self-test"      ,1,0},
    { "SMART error logging"  ,1,0},
};
