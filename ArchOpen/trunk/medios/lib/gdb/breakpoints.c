/*
*   lib/breakpoints.c
*
*   MediOS project
*   Copyright (c) 2008 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*
* original code from Ben's Nintendo DS hacking gdb stub:
* http://masscat.afraid.org/ninds/debug_stub.php
*
*/

#include <kernel/kernel.h>

#include <lib/gdb.h>
#include <lib/breakpoints.h>

#include <lib/string.h>

/** The ARM BKPT instruction opcode */
#define ARM_BKPT_OPCODE ((uint32_t)0xEF000006)

/** The Thumb BKPT instruction opcode */
#define THUMB_BKPT_OPCODE ((uint16_t)0xCF06)

/** Remove the breakpoints in a list from memory */
void removeAll_breakpoint( const struct breakpoint_descr *list_head)
{
  const struct breakpoint_descr *bkpt = list_head;

  while ( bkpt != NULL) {
    DEBUG_GDB( "Removing bkpt at %08x (n %08x)\n", bkpt->address, (uint32_t)bkpt->next);
    /* place the stored instruction back into memory */
    if ( bkpt->thumb) {
      *(uint16_t *)bkpt->address = bkpt->instruction.thumb;
    }
    else {
      *(uint32_t *)bkpt->address = bkpt->instruction.arm;
    }

    bkpt = bkpt->next;
  }
}


/** Insert the breakpoint in a list into memory */
void
insertAll_breakpoint( struct breakpoint_descr *list_head) {
  struct breakpoint_descr *bkpt = list_head;

  while ( bkpt != NULL) {
    DEBUG_GDB( "Inserting bkpt at %08x thumb = %d code = (%x) old %x\n", bkpt->address,bkpt->thumb,
               bkpt->thumb?(THUMB_BKPT_OPCODE):(ARM_BKPT_OPCODE),
               bkpt->thumb?*(uint16_t *)bkpt->address:*(uint32_t *)bkpt->address,
               (uint32_t)bkpt->next);
    /* store the current contents of memory and replace it with the bkpt opcode */
    if ( bkpt->thumb) {
      bkpt->instruction.thumb = *(uint16_t *)bkpt->address;
      *(uint16_t *)bkpt->address = THUMB_BKPT_OPCODE;
    }
    else {
      bkpt->instruction.arm = *(uint32_t *)bkpt->address;
      *(uint32_t *)bkpt->address = ARM_BKPT_OPCODE;
    }
    DEBUG_GDB( "Nxt %x\n",bkpt->next);
    bkpt = bkpt->next;
  }
  DEBUG_GDB( "Finished Inserting bkpts\n");
}


/*
 * Breakpoint list functions
 */
/** Concatenate two lists */
void catLists_breakpoint( struct breakpoint_descr **list1_ptr,
		     struct breakpoint_descr *list2)
{
  if ( *list1_ptr == NULL) {
    *list1_ptr = list2;
  }
  else {
    struct breakpoint_descr *descr = *list1_ptr;

    while ( descr->next != NULL) {
      descr = descr->next;
    }
    descr->next = list2;
  }
}


/** Remove a breakpoint from a list based on the breakpoint address */
struct breakpoint_descr * removeFromList_breakpoint( struct breakpoint_descr **list_ptr, uint32_t bkpt_addr)
{
  struct breakpoint_descr *found = NULL;
  struct breakpoint_descr *prev = NULL;
  struct breakpoint_descr *current = *list_ptr;

  while ( found == NULL && current != NULL)
  {
      if ( current->address == bkpt_addr)
      {
          found = current;

            /* remove from the list */
            if ( prev == NULL)
	           *list_ptr = current->next;
            else
	           prev->next = current->next;
            current->next = NULL;
        }
        prev = current;
        current = current->next;
  }

  return found;
}


/** Add a breakpoint to the head of a breakpoint list */
void addHead_breakpoint( struct breakpoint_descr **list_ptr,
		    struct breakpoint_descr *bkpt_descr)
{
  bkpt_descr->next = *list_ptr;

  *list_ptr = bkpt_descr;
}


/** Remove a breakpoint descriptor from the head of a list */
struct breakpoint_descr * removeHead_breakpoint( struct breakpoint_descr **list_ptr)
{
  struct breakpoint_descr *head = *list_ptr;

  if ( head != NULL) {
    *list_ptr = head->next;
    head->next = NULL;
  }

  return head;
}


/** Initialise a breakpoint descriptor */
void initDescr_breakpoint( struct breakpoint_descr *bkpt_descr,
		      uint32_t address, int thumb_flag)
{
  bkpt_descr->next = NULL;
  bkpt_descr->address = address;
  bkpt_descr->thumb = thumb_flag;
}
