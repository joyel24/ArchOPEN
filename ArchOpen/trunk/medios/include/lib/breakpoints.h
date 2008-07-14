/*
*   include/lib/breakpoints.h
*
*   MediOS project
*   Copyright (c) 2008 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
* original code from Ben's Nintendo DS hacking gdb stub:
* http://masscat.afraid.org/ninds/debug_stub.php
*
*/

#ifndef __BREAKPOINTS_H
#define __BREAKPOINTS_H

/**
 * The desciption of a breakpoint.
 */
struct breakpoint_descr {
  /** list former */
  struct breakpoint_descr *next;

  /** The address of the breakpoint */
  uint32_t address;

  /** The replaced instruction */
  union {
    uint32_t arm;
    uint16_t thumb;
  } instruction;

  /** The ARM/Thumb flag */
  int thumb;
};



/*
 * The breakpoint handling functions
 */
/** Remove the breakpoints in a list from memory */
void
removeAll_breakpoint( const struct breakpoint_descr *list_head);


/** Insert the breakpoints in a list into memory */
void
insertAll_breakpoint( struct breakpoint_descr *list_head);


/*
 * Breakpoint list functions
 */
/** Concatenate two lists */
void
catLists_breakpoint( struct breakpoint_descr **list1_ptr,
		     struct breakpoint_descr *list2);


/** Remove a breakpoint from a list based on the breakpoint address */
struct breakpoint_descr *
removeFromList_breakpoint( struct breakpoint_descr **list_ptr, uint32_t bkpt_addr);


/** Add a breakpoint to the head of a breakpoint list */
void
addHead_breakpoint( struct breakpoint_descr **list_ptr,
		    struct breakpoint_descr *bkpt_descr);


/** Remove a breakpoint descriptor from the head of a list */
struct breakpoint_descr *
removeHead_breakpoint( struct breakpoint_descr **list_ptr);


/** Initialise a breakpoint descriptor */
void
initDescr_breakpoint( struct breakpoint_descr *bkpt_descr,
		      uint32_t address, int thumb_flag);


#endif
