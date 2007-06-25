#ifndef __WILDMIDI__
#define __WILDMIDI__

int (*open_output) (void);
int (*send_output) (char * output_data, int output_size);
void (*close_output) (void);

#endif //__WILDMIDI__
