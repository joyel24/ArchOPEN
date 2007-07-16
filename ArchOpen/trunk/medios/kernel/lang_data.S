/*
*   medios/kernel/lang_data.S
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

.section ".text"
.asciz "BUILDIN_LNG"
.align 8
.global buildin_lng
buildin_lng:
.incbin "lang_data.bin"
.global buildin_lng_end
buildin_lng_end:
