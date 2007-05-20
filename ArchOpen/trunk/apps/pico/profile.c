#include "profile.h"
#include "shared.h"

Profile profile_list[256];
int profile_count=0;

int profile_find_profile(char *name)
{
    int i;

    for ( i=0 ; i<profile_count ; i++ )
    {
        if ( !strcmp(name, profile_list[i].name) )
        {
             return i;
        }
    }

    return -1;
}

void profile_start(char *name)
{
     int profile_i;

     profile_i=profile_find_profile(name);

     if ( profile_i >= 0 )
     {
          profile_list[profile_i].n_called++;
          profile_list[profile_i].start_time=tmr_getMicroTick();
     }
     else
     {
         profile_list[profile_count].name=name;
         profile_list[profile_count].n_called=1;
         profile_list[profile_count].total_time=0;
         profile_list[profile_count].start_time=tmr_getMicroTick();

         profile_count++;
     }
}

void profile_stop(char *name)
{
     int profile_i;

     profile_i=profile_find_profile(name);

     if ( profile_i >= 0 )
     {
          profile_list[profile_i].end_time=tmr_getMicroTick();
          profile_list[profile_i].total_time+=profile_list[profile_i].end_time-profile_list[profile_i].start_time;
     }
}

void profile_reset()
{
    int i;

    memset(profile_list, 0, sizeof(Profile));
    for ( i=0 ; i<profile_count ; i++ )
    {
        profile_list[i].name=NULL;
    }

    profile_count=0;
}

void profile_print(int i)
{
     if ( i >= 0 )
     {
          printf("[%s] called %dx, tot. time:%d, last:%d\n", profile_list[i].name,
                                                                          profile_list[i].n_called,
                                                                          profile_list[i].total_time,
                                                                          profile_list[i].end_time-profile_list[i].start_time);
     }
}

char * profile_sprint(char *s, int i)
{
     if ( i >= 0 )
     {
          sprintf(s,"[%s]%dx,tottime:%d,last:%d", profile_list[i].name,
                                                                          profile_list[i].n_called,
                                                                          profile_list[i].total_time,
                                                                          profile_list[i].end_time?profile_list[i].end_time-profile_list[i].start_time:tmr_getMicroTick()-profile_list[i].start_time);
     }
     return s;
}

void profile_printAll()
{
     int i;

     for ( i=0 ; i<profile_count ; i++ )
     {
         profile_print(i);
     }
}
