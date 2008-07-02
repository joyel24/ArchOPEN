/*
*   kernel/core/bkpt_dummy.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

/**************************************
 * dummy bkpt when bkpt.S is not build
 * do_bkpt is not doing anything
 * see bkpt.S for the real function: it is used by aomulator
 * if Makefile define HAVE_DBUG is set to 1
 * bkpt.S is build, otherwise bkpt_dummy.c is
 * build.
 * Device priori to AV400 were not caching the arm bkpt
 * instruction, so it was safe to leave them in medios. For Av400
 * and later this dummy function is needed in order to not call
 * arm bkpt instruction
 **************************************/

void do_bkpt(void)
{

}
