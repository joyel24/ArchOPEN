/*
* kernel/gfx/gui/widget/virtKbd_inc.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

unsigned char leftArrow_DA[13][8] =
{ {0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 1, 1, 0, 0},
{0, 0, 0, 1, 1, 0, 0, 0},
{0, 0, 1, 1, 0, 0, 0, 0},
{0, 1, 1, 0, 0, 0, 0, 0},
{1, 1, 1, 1, 1, 1, 1, 1},
{1, 1, 1, 1, 1, 1, 1, 1},
{0, 1, 1, 0, 0, 0, 0, 0},
{0, 0, 1, 1, 0, 0, 0, 0},
{0, 0, 0, 1, 1, 0, 0, 0},
{0, 0, 0, 0, 1, 1, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0}
};

SPRITE leftArrow_SP = {(unsigned int) leftArrow_DA, 8, 13, 1, 1};

unsigned char rightArrow_DA[13][8] =
{ {0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 1, 1, 0, 0, 0, 0},
{0, 0, 0, 1, 1, 0, 0, 0},
{0, 0, 0, 0, 1, 1, 0, 0},
{0, 0, 0, 0, 0, 1, 1, 0},
{1, 1, 1, 1, 1, 1, 1, 1},
{1, 1, 1, 1, 1, 1, 1, 1},
{0, 0, 0, 0, 0, 1, 1, 0},
{0, 0, 0, 0, 1, 1, 0, 0},
{0, 0, 0, 1, 1, 0, 0, 0},
{0, 0, 1, 1, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0},
{0, 0, 0, 0, 0, 0, 0, 0}
};

SPRITE rightArrow_SP = {(unsigned int) rightArrow_DA, 8, 13, 1, 1};

unsigned char upArrow_DA[13][8] =
{ 
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 0},
    {1, 1, 0, 1, 1, 0, 1, 1},
    {1, 0, 0, 1, 1, 0, 0, 1},
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0}
};

SPRITE upArrow_SP = {(unsigned int) upArrow_DA, 8, 13, 1, 1};

unsigned char up2Arrow_DA[13][8] =
{ 
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 1, 1, 0},
    {1, 1, 0, 1, 1, 0, 1, 1},
    {1, 0, 0, 1, 1, 0, 0, 1},
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 1, 1}
};

SPRITE up2Arrow_SP = {(unsigned int) up2Arrow_DA, 8, 13, 1, 1};

unsigned char up2ArrowInv_DA[13][8] =
{ 
    {1, 1, 1, 0, 0, 1, 1, 1},
    {1, 1, 0, 0, 0, 0, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 1},
    {0, 0, 1, 0, 0, 1, 0, 0},
    {0, 1, 1, 0, 0, 1, 1, 0},
    {1, 1, 1, 0, 0, 1, 1, 1},
    {1, 1, 1, 0, 0, 1, 1, 1},
    {1, 1, 1, 0, 0, 1, 1, 1},
    {1, 1, 1, 0, 0, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {0, 0, 0, 0, 0, 0, 0, 0}
};

SPRITE up2ArrowInv_SP = {(unsigned int) up2ArrowInv_DA, 8, 13, 1, 1};

unsigned char del_DA[13][8] =
{ 
    {0, 0, 0, 1, 1, 0, 0, 0},
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 0, 0, 1, 1, 0},
    {1, 1, 0, 0, 0, 0, 1, 1},
    {1, 0, 0, 0, 0, 1, 1, 1},
    {1, 0, 0, 0, 1, 1, 0, 1},
    {1, 0, 0, 1, 1, 0, 0, 1},
    {1, 0, 1, 1, 0, 0, 0, 1},
    {1, 1, 1, 0, 0, 0, 0, 1},
    {0, 1, 0, 0, 0, 0, 1, 0},
    {0, 1, 1, 0, 0, 1, 1, 0},
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 0}
};

SPRITE del_SP = {(unsigned int) del_DA, 8, 13, 1, 1};

unsigned char delInv_DA[13][8] =
{ 
    {1, 1, 1, 0, 0, 1, 1, 1},
    {1, 1, 0, 0, 0, 0, 1, 1},
    {1, 0, 0, 1, 1, 0, 0, 1},
    {0, 0, 1, 1, 1, 1, 0, 0},
    {0, 1, 1, 1, 1, 0, 0, 0},
    {0, 1, 1, 1, 0, 0, 1, 0},
    {0, 1, 1, 0, 0, 1, 1, 0},
    {0, 1, 0, 0, 1, 1, 1, 0},
    {0, 0, 0, 1, 1, 1, 1, 0},
    {1, 0, 1, 1, 1, 1, 0, 1},
    {1, 0, 0, 1, 1, 0, 0, 1},
    {1, 1, 0, 0, 0, 0, 1, 1},
    {1, 1, 1, 0, 0, 1, 1, 1}
};

SPRITE delInv_SP = {(unsigned int) delInv_DA, 8, 13, 1, 1};

unsigned char sup_DA[13][8] =
{ 
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 1, 0, 0, 1},
    {0, 0, 1, 1, 0, 0, 0, 1},
    {0, 1, 1, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 0, 1},
    {1, 1, 1, 1, 1, 1, 0, 1},
    {0, 1, 1, 0, 0, 0, 0, 1},
    {0, 0, 1, 1, 0, 0, 0, 1},
    {0, 0, 0, 1, 1, 0, 0, 1},
    {0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0}
};

SPRITE supSP = {(unsigned int) sup_DA, 8, 13, 1, 1};
        
unsigned char num_DA[13][8] =
{ 
    {0, 0, 0, 0, 1, 1, 0, 0},
    {0, 0, 0, 1, 0, 0, 1, 0},
    {0, 1, 0, 0, 0, 0, 1, 0},
    {1, 1, 0, 0, 0, 1, 0, 0},
    {0, 1, 0, 0, 1, 0, 0, 0},
    {0, 1, 0, 1, 1, 1, 1, 0},
    {0, 1, 0, 0, 0, 0, 0, 0},
    {0, 1, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 1, 1, 0},
    {0, 0, 0, 0, 0, 0, 0, 1},
    {0, 0, 0, 0, 0, 1, 1, 0}
};

SPRITE num_SP = {(unsigned int) num_DA, 8, 13, 1, 1};

unsigned char numInv_DA[13][8] =
{ 
    {1, 1, 1, 1, 0, 0, 1, 1},
    {1, 1, 1, 0, 1, 1, 0, 1},
    {1, 0, 1, 1, 1, 1, 0, 1},
    {0, 0, 1, 1, 1, 0, 1, 1},
    {1, 0, 1, 1, 0, 1, 1, 1},
    {1, 0, 1, 0, 0, 0, 0, 1},
    {1, 0, 1, 1, 1, 1, 1, 1},
    {1, 0, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 0},
    {1, 1, 1, 1, 1, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 0},
    {1, 1, 1, 1, 1, 0, 0, 1}
};

SPRITE numInv_SP = {(unsigned int) numInv_DA, 8, 13, 1, 1};

unsigned char ins_DA[13][8] =
{ 
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 0, 0, 0},
    {0, 0, 1, 0, 0, 1, 0, 0},
    {1, 0, 1, 1, 0, 1, 0, 0},
    {0, 0, 1, 0, 1, 1, 0, 0},
    {1, 0, 1, 0, 0, 1, 0, 0},
    {1, 0, 0, 0, 0, 0, 0, 0},
    {1, 0, 0, 0, 0, 1, 1, 1},
    {1, 0, 0, 0, 0, 1, 0, 0},
    {1, 0, 0, 0, 0, 0, 1, 0},
    {0, 0, 0, 0, 0, 1, 1, 1}
};

SPRITE ins_SP = {(unsigned int) ins_DA, 8, 13, 1, 1};

unsigned char insInv_DA[13][8] =
{ 
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1},
    {1, 1, 0, 1, 1, 0, 1, 1},
    {0, 1, 0, 0, 1, 0, 1, 1},
    {1, 1, 0, 1, 0, 0, 1, 1},
    {0, 1, 0, 1, 1, 0, 1, 1},
    {0, 1, 1, 1, 1, 1, 1, 1},
    {0, 1, 1, 1, 1, 0, 0, 0},
    {0, 1, 1, 1, 1, 0, 1, 1},
    {0, 1, 1, 1, 1, 1, 0, 1},
    {1, 1, 1, 1, 1, 0, 0, 0}
};

SPRITE insInv_SP = {(unsigned int) insInv_DA, 8, 13, 1, 1};

char char_horiz[3][6][NB_CELL_PER_LINE]= {
    {
        /* TOP */
        { 'a', 'b', 'c' },
        { 'd', 'e', 'f' },
        { 'g', 'h', 'i' },
        /* BOTTOM */
        { ' ','\0', '.' },
        {'\0','\0','\0'},
        {'\0','\0','\0'}
    },
    {
        /* TOP */
        { 'A', 'B', 'C' },
        { 'D', 'E', 'F' },
        { 'G', 'H', 'I' },
        /* BOTTOM */
        { ' ','\0', '.' },
        {'\0','\0','\0'},
        {'\0','\0','\0'}
    },
    {
        /* TOP */
        {'\0','\0','\0' },
        {'\0', '0','\0' },
        { '1', '2', '3' },
        /* BOTTOM */
        { ' ','\0', '.' },
        {'\0','\0','\0'},
        {'\0','\0','\0'}
    }
};

char char_vert[3][6][NB_CELL_PER_LINE]= {
    {
        /* LEFT */
        { 'j', 'k', 'l' },
        { 'm', 'n', 'o' },
        { 'p', 'q', 'r' },
        /* RIGHT */
        { 's', 't', 'u' },
        { 'v', 'w', 'x' },
        { 'y', 'z', '_' }
    },
    {
        /* LEFT */
        { 'J', 'K', 'L' },
        { 'M', 'N', 'O' },
        { 'P', 'Q', 'R' },
        /* RIGHT */
        { 'S', 'T', 'U' },
        { 'V', 'W', 'X' },
        { 'Y', 'Z', '_' }
    },
    {
        /* LEFT */
        {'\0','\0','\0' },
        {'\0','\0','\0' },
        { '4', '5', '6' },
        /* RIGHT */
        { '7', '8', '9' },
        {'\0','\0','\0' },
        {'\0','\0', '_' }
    }
};

SPRITE * spec_horiz[3][6][NB_CELL_PER_LINE]= {
    {
        /* TOP */
        { NULL, NULL, NULL },
        { NULL, NULL, NULL },
        { NULL, NULL, NULL },
        /* BOTTOM */
        { NULL, NULL, NULL },
        { NULL, NULL, NULL },
        { NULL, NULL, NULL }
    },
    {
        /* TOP */
        { NULL, NULL, NULL },
        { NULL, NULL, NULL },
        { NULL, NULL, NULL },
        /* BOTTOM */
        { NULL, NULL, NULL },
        { NULL, NULL, NULL },
        { NULL, NULL, NULL }
    },
    {
        /* TOP */
        { NULL, NULL, NULL },
        { NULL, NULL, NULL },
        { NULL, NULL, NULL },
        /* BOTTOM */
        { NULL, NULL, NULL },
        { NULL, NULL, NULL },
        { NULL, NULL, NULL }
    }
};

SPRITE * spec_vert[3][6][NB_CELL_PER_LINE]= {
    {
        /* LEFT */
        { NULL, NULL, NULL },
        { NULL, NULL, NULL },
        { NULL, NULL, NULL },
        /* RIGHT */
        { NULL, NULL, NULL },
        { NULL, NULL, NULL },
        { NULL, NULL, NULL }
    },
    {
        /* LEFT */
        { NULL, NULL, NULL },
        { NULL, NULL, NULL },
        { NULL, NULL, NULL },
        /* RIGHT */
        { NULL, NULL, NULL },
        { NULL, NULL, NULL },
        { NULL, NULL, NULL }
    },
    {
        /* LEFT */
        { NULL, NULL, NULL },
        { NULL, NULL, NULL },
        { NULL, NULL, NULL },
        /* RIGHT */
        { NULL, NULL, NULL },
        { NULL, NULL, NULL },
        { NULL, NULL, NULL }
    }
};



void (*action_horiz[3][6][NB_CELL_PER_LINE])= {
    {
        /* TOP */
        { NULL, NULL, NULL },
        { NULL, NULL, NULL },
        { NULL, NULL, NULL },
        /* BOTTOM */
        { NULL, numPage, NULL },
        { leftCursor, shiftCursor, rightCursor },
        { insCursor, delCursor, supCursor }
    },
    {
        /* TOP */
        { NULL, NULL, NULL },
        { NULL, NULL, NULL },
        { NULL, NULL, NULL },
        /* BOTTOM */
        { NULL, numPage, NULL },
        { leftCursor, shiftCursor, rightCursor },
        { insCursor, delCursor, supCursor }
    },
    {
        /* TOP */
        { NULL, NULL, NULL },
        { NULL, NULL, NULL },
        { NULL, NULL, NULL },
        /* BOTTOM */
        { NULL, numPage, NULL },
        { leftCursor, shiftCursor, rightCursor },
        { insCursor, delCursor, supCursor }
    }
};

void (*action_vert[3][6][NB_CELL_PER_LINE])= {
    {
        /* TOP */
        { NULL, NULL, NULL },
        { NULL, NULL, NULL },
        { NULL, NULL, NULL },
        /* BOTTOM */
        { NULL, NULL, NULL },
        { NULL, NULL, NULL },
        { NULL, NULL, NULL }
    },
    {
        /* TOP */
        { NULL, NULL, NULL },
        { NULL, NULL, NULL },
        { NULL, NULL, NULL },
        /* BOTTOM */
        { NULL, NULL, NULL },
        { NULL, NULL, NULL },
        { NULL, NULL, NULL }
    },
    {
        /* TOP */
        { NULL, NULL, NULL },
        { NULL, NULL, NULL },
        { NULL, NULL, NULL },
        /* BOTTOM */
        { NULL, NULL, NULL },
        { NULL, NULL, NULL },
        { NULL, NULL, NULL }
    }
};
