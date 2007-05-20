#ifndef __GUI_H
#define __GUI_H

extern bool gui_browserNeedInit;
extern struct browser_data * browser;

void gui_init();
bool gui_browse();
void gui_showEmuPlane();
void gui_showGuiPlane();

#endif
