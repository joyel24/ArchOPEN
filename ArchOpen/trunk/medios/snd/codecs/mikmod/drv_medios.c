/*	MikMod sound library
	(c) 1998, 1999, 2000 Miodrag Vallat and others - see file AUTHORS for
	complete list.

	This library is free software; you can redistribute it and/or modify
	it under the terms of the GNU Library General Public License as
	published by the Free Software Foundation; either version 2 of
	the License, or (at your option) any later version.
 
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU Library General Public License for more details.
 
	You should have received a copy of the GNU Library General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
	02111-1307, USA.
*/

/*==============================================================================

  $Id: drv_nos.c,v 1.1.1.1 2004/01/21 01:36:35 raph Exp $

  Driver for no output

==============================================================================*/

/*

	Written by Jean-Paul Mikkers <mikmak@via.nl>

*/

#include <api.h>

#include "mikmod_internals.h"

#define DATA_BUFFER_SIZE 8192

char dataBuf[DATA_BUFFER_SIZE];

static BOOL Med_IsThere(void)
{
	return 1;
}

static BOOL Med_Init(void)
{
    return VC_Init();
}

static void Med_Exit(void)
{
	VC_Exit();
}

static void Med_Update(void)
{
    VC_WriteBytes(dataBuf,DATA_BUFFER_SIZE);
    output_write(dataBuf,DATA_BUFFER_SIZE);
}

MIKMODAPI MDRIVER drv_medios={
	NULL,
	"Medios",
	"Medios Driver v1.0",
	255,255,
	"medios",

	NULL,
	Med_IsThere,
	VC_SampleLoad,
	VC_SampleUnload,
	VC_SampleSpace,
	VC_SampleLength,
	Med_Init,
	Med_Exit,
	NULL,
	VC_SetNumVoices,
	VC_PlayStart,
	VC_PlayStop,
	Med_Update,
	NULL,
	VC_VoiceSetVolume,
	VC_VoiceGetVolume,
	VC_VoiceSetFrequency,
	VC_VoiceGetFrequency,
	VC_VoiceSetPanning,
	VC_VoiceGetPanning,
	VC_VoicePlay,
	VC_VoiceStop,
	VC_VoiceStopped,
	VC_VoiceGetPosition,
	VC_VoiceRealVolume
};


/* ex:set ts=4: */
