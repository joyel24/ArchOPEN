/*
*   apps/avlo/include/avlo_cfg.c
*
*   Avlo a loader for Jbmm, av1xx and av3xx
*   ArchOpen Project
*   Copyright (c) 2007 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __AVLO_CFG_H_
#define __AVLO_CFG_H_

#define VAR_COLOR(NAME,NAME2)  rgb_color NAME;unsigned int NAME2;
#define INIT_COLOR(NAME,NAME2,COLOR) NAME : {-1,-1,-1}, NAME2 : COLOR,

typedef struct rgb_color_t {
    int r;
    int g;
    int b;
} rgb_color;

struct avlo_cfg {
    char magic[8] ;
    /* color */
    VAR_COLOR(color_txt,color_txt_index)
    VAR_COLOR(color_box,color_box_index)
    VAR_COLOR(color_sel,color_sel_index)
    VAR_COLOR(color_load,color_load_index)
    VAR_COLOR(color_wait,color_wait_index)
    VAR_COLOR(color_msg_box_0,color_msg_box_0_index)
    VAR_COLOR(color_msg_box_1,color_msg_box_1_index)
    VAR_COLOR(color_pwr_charge,color_pwr_charge_index)
    VAR_COLOR(color_pwr_l0,color_pwr_l0_index)
    VAR_COLOR(color_pwr_l1,color_pwr_l1_index)
    VAR_COLOR(color_pwr_l2,color_pwr_l2_index)
    VAR_COLOR(color_pwr_l3,color_pwr_l3_index)
    VAR_COLOR(color_pwr_frame_dc_on,color_pwr_frame_dc_on_index)
    VAR_COLOR(color_pwr_frame_dc_off,color_pwr_frame_dc_off_index)
        
    /*position / size */
    int menu_x,menu_y;
    int status_x,status_y;
    int bar_x,bar_y,bar_w,bar_h;
    int usb_x,usb_y;
    int bat_x,bat_y;
};

#endif
