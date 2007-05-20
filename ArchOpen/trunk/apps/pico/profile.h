#ifndef __PROFILE_H
#define __PROFILE_H

typedef struct sProfile{
        char *name;
        int start_time, end_time, n_called, total_time;
}Profile;

extern Profile profile_list[256];
extern int profile_count;

void profile_start(char *name);
void profile_stop(char *name);
void profile_reset();
void profile_print(int i);
void profile_printAll();
#endif
