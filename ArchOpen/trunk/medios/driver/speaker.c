/*
*   kernel/driver/speaker.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/
#include <kernel/kernel.h>

#include <driver/speaker.h>

int spkrState=0;

void speaker_enable(int enable)
{
    if(enable)
    {
        SPKR_ON();
        spkrState=1;
    }
    else
    {
        SPKR_OFF();
        spkrState=0;
    }
}

int speaker_state(void)
{
    return SPKR_STATE();
}

int speaker_available(void)
{
    return SPKR_AVAILABLE();
}
