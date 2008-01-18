/*
*   init/boot_error_msg.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

 /*table of error messages */
 
char * boot_error_msg[] = {
 	/*0*/	"/medios folder is missing",
	/*1*/	"/medios/codec folder is missing",
	/*2*/	"DSP file /medios/codec/snd_dsp.out can't be loaded",
	/*3*/	"/medios/lang missing",
	/*4*/	"config lang file missing, using default",
	/*5*/	"/medios/icons is missing",
	/*6*/	"old icon structure upgrade to new",
	/*7*/	"/medios/handlers.cfg is missing",
	/*8*/	"/medios/menu.cfg is missing",
	/*9*/	"/medios/bg.img is missing",
   /*10*/   "Wrong Lang file version, get newest pack",
};
