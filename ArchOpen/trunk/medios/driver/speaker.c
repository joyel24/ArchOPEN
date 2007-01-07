/*
*   kernel/driver/speaker.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/

#include <driver/speaker.h>

void speaker_enable(int enable)
{
    if(enable)
    {
        SPKR_ON();
    }
    else
    {
        SPKR_OFF();
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
