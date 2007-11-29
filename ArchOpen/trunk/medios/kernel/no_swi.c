/*
*   kernel/core/no_swi.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

unsigned int swi_handler (
    unsigned long nParam1,
    unsigned long nParam2,
    unsigned long nParam3,        
    unsigned long nCmd)
{
    return 0;
}
