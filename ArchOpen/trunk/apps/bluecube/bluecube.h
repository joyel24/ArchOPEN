/*
 * Bluecube for MediOs
 * 
 * bluecube.h
 * 
 *   MediOS project
 *   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
 *
 * All files in this archive are subject to the GNU General Public License.
 * See the file COPYING in the source tree root for full license agreement.
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express of implied.
 */
 
 #ifndef __BLUECUBE
 #define __BLUECUBE
 
 struct blueCube_archDef {
	int action_stop;
	int action_pause;
	int action_turn_right_1;
	int action_turn_right_2;
	int action_move_left;
	int action_move_right;
	int action_move_down;
	int action_new_game;
	int action_drop;
 };
 
 /* conf list */
 
 struct blueCube_archDef default_conf = {
	action_stop:BTN_OFF,
	action_pause:BTN_ON,
	action_turn_right_1:BTN_1,
	action_turn_right_2:BTN_UP,
	action_move_left:BTN_LEFT,
	action_move_right:BTN_RIGHT,
	action_move_down:BTN_DOWN,
	action_new_game:BTN_ON,
	action_drop:BTN_F1
 };
 
 struct blueCube_archDef av4_conf = {
	action_stop:BTN_OFF,
	action_pause:BTN_F2,
	action_turn_right_1:BTN_ON,
	action_turn_right_2:BTN_UP,
	action_move_left:BTN_LEFT,
	action_move_right:BTN_RIGHT,
	action_move_down:BTN_DOWN,
	action_new_game:BTN_ON,
	action_drop:BTN_F1
 };
 
 
 /* conf table */
 struct blueCube_entry {
    int arch;
    struct blueCube_archDef * conf;
};


struct blueCube_entry conf_table [] = {
	{AV3XX_ARCH,    &default_conf},
    {GMINI4XX_ARCH, &default_conf},
    {AV1XX_ARCH,    &default_conf},
    {JBMM_ARCH,     &default_conf},
    {GMINI402_ARCH, &default_conf},
    {AV4XX_ARCH,    &av4_conf},
    {PMA_ARCH,      &default_conf},
    {AV5XX_ARCH,    &default_conf},
	{-1, NULL }
};
 
#endif
