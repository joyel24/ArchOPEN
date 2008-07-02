/*
* include/gui/arch_widget.h
*
* MediOS project
* Copyright (c) 2005 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#ifndef __ARCH_WIDGET_H
#define __ARCH_WIDGET_H

#ifdef GMINI4XX
    #define WIDGET_ACTION_BTN BTN_1
    #define WIDGET_BACK_BTN BTN_2
    #define WIDGET_DEFAULT_FONT STD6X9
    #define WIDGET_CONFIG_FONT STD6X9    
#endif

#ifdef GMINI402
    #define WIDGET_ACTION_BTN BTN_1
    #define WIDGET_BACK_BTN BTN_2
    #define WIDGET_DEFAULT_FONT STD6X9
    #define WIDGET_CONFIG_FONT STD6X9
#endif

#ifdef AV4XX
    #define WIDGET_ACTION_BTN BTN_ON
    #define WIDGET_BACK_BTN BTN_OFF
    #define WIDGET_DEFAULT_FONT STD6X9
    #define WIDGET_CONFIG_FONT STD8X13
#endif

#ifdef PMA
    #define WIDGET_ACTION_BTN BTN_ON
    #define WIDGET_BACK_BTN BTN_OFF
    #define WIDGET_DEFAULT_FONT STD6X9
    #define WIDGET_CONFIG_FONT STD6X9
#endif

#ifdef AV3XX
    #define WIDGET_ACTION_BTN BTN_ON
    #define WIDGET_BACK_BTN BTN_OFF
    #define WIDGET_DEFAULT_FONT STD6X9
    #define WIDGET_CONFIG_FONT STD8X13
#endif

#ifdef AV1XX
    #define WIDGET_ACTION_BTN BTN_1
    #define WIDGET_BACK_BTN BTN_OFF
    #define WIDGET_DEFAULT_FONT STD6X9
    #define WIDGET_CONFIG_FONT STD6X9
#endif

#ifdef JBMM
    #define WIDGET_ACTION_BTN BTN_1
    #define WIDGET_BACK_BTN BTN_OFF
    #define WIDGET_DEFAULT_FONT STD6X9
    #define WIDGET_CONFIG_FONT STD6X9
#endif

#ifdef AV5XX
    #define WIDGET_ACTION_BTN BTN_ON
    #define WIDGET_BACK_BTN BTN_OFF
    #define WIDGET_DEFAULT_FONT STD6X9
    #define WIDGET_CONFIG_FONT STD8X13
#endif


#ifndef WIDGET_ACTION_BTN
    #error Unknown arch!
#endif

#endif
