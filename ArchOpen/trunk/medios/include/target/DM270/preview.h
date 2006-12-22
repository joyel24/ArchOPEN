/*
*   include/kernel/preview_DM270.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __PREVIEW_DM270_H
#define __PREVIEW_DM270_H

#define PREVIEW_BASE                   0x00030780

#define PREVIEW_ENABLE                 (PREVIEW_BASE + 0x0000)
#define PREVIEW_SETUP                  (PREVIEW_BASE + 0x0002)
#define PREVIEW_SDRAM_READ_HI          (PREVIEW_BASE + 0x0004)
#define PREVIEW_SDRAM_READ_LO          (PREVIEW_BASE + 0x0006)
#define PREVIEW_SDRAM_WRITE_HI         (PREVIEW_BASE + 0x0008)
#define PREVIEW_SDRAM_WRITE_LO         (PREVIEW_BASE + 0x000a)
#define PREVIEW_HORZ_START             (PREVIEW_BASE + 0x000c)
#define PREVIEW_HORZ_SIZE              (PREVIEW_BASE + 0x000e)
#define PREVIEW_VERT_START             (PREVIEW_BASE + 0x0010)
#define PREVIEW_VERT_SIZE              (PREVIEW_BASE + 0x0012)
#define PREVIEW_HORZ_RESIZE            (PREVIEW_BASE + 0x0020)
#define PREVIEW_VERT_RESIZE            (PREVIEW_BASE + 0x0022)
#define PREVIEW_LUMA_RANGE             (PREVIEW_BASE + 0x004e)
#define PREVIEW_CHROMA_RANGE           (PREVIEW_BASE + 0x0050)

#define PREVIEW_ENB_ENABLED            0x0001
#define PREVIEW_ENB_DISABLED           0x0000

#define PREVIEW_STP_WRITE_IGNORE       0x0000
#define PREVIEW_STP_WRITE_VALID        0x0001
#define PREVIEW_STP_BURST_NORMAL       0x0000
#define PREVIEW_STP_BURST_ALIGNED      0x0002
#define PREVIEW_STP_INPUT_CCD          0x0000
#define PREVIEW_STP_INPUT_SDRAM        0x0004
#define PREVIEW_STP_MODE_NORMAL        0x0000
#define PREVIEW_STP_MODE_RESIZEONLY    0x0008
#define PREVIEW_STP_CONTINUOUS         0x0000
#define PREVIEW_STP_ONESHOT            0x0010
#define PREVIEW_STP_ALAW_NORMAL        0x0000
#define PREVIEW_STP_ALAW_INVERSE       0x0020
#define PREVIEW_STP_10BIT              0x0000
#define PREVIEW_STP_8BIT               0x0040

#define PREVIEW_YCR_MIN_SHIFT          0
#define PREVIEW_YCR_MAX_SHIFT          8

#define PREVIEW_RESIZE_RATIO           16

#endif
