#api_inc#sys_def/arch.h
#api_inc#sys_def/colordef.h
#api_inc#sys_def/font.h
#api_inc#sys_def/section_types.h
#api_inc#sys_def/stddef.h
#api_inc#sys_def/types.h
#api_inc#kernel/io.h

#k_inc#sys_def/errors.h
#api_inc#sys_def/errors.h

#k_inc#kernel/malloc.h
void* # malloc   # long size
void* # realloc  # void *buffer, long newsize
void  # free     # void *buf
void* # calloc   # unsigned int nmemb,unsigned int size

#k_inc#kernel/timer.h
unsigned int # tmr_getTick   # void
unsigned int # tmr_getMicroTick   # void

#k_inc#driver/time.h
#api_inc#sys_def/time.h
MED_RET_T # time_get # struct med_tm * valTime
MED_RET_T # time_set # struct med_tm * newTime

#k_inc#kernel/delay.h
void # udelay  # unsigned long usecs
void # mdelay  # unsigned long msecs

#k_inc#driver/energy.h
void # set_timer_status  # int timer_type, int power_mode, int status
void # set_timer_delay   # int timer_type, int power_mode, int delay
int  # get_timer_status  # int timer_type, int power_mode
int  # get_timer_delay   # int timer_type, int power_mode

#k_inc#driver/batDc.h
int  # DC_isConnected    # void
int  # batLevel          # void

#k_inc#driver/lcd.h
void # lcd_setBrightness # int br
int  # lcd_getBrightness # void

#k_inc#driver/usb_fw.h
int # usb_isConnected # void
int # FW_isConnected  # void

#k_inc#kernel/evt.h
#api_inc#sys_def/evt.h
int          # evt_getHandler     # unsigned int mask
MED_RET_T    # evt_freeHandler    # int num_evt_pipe
int          # evt_getStatus      # int num_evt_pipe
MED_RET_T    # evt_getFullStatus  # int num_evt_pipe, struct evt_t * evt
const char * # getBtnName         # int btn

#k_inc#driver/buttons.h
#api_inc#sys_def/buttons.h
int # btn_readState # void

#k_inc#init/exit.h
void # halt_device     # void
void # reload_firmware # void

#k_inc#kernel/kernel.h
void        # printf        # char *fmt, ...
void        # print_data    # char * data,int length
void        # do_bkpt       # void
int         # getArch       # void
void        # getResolution # int * width,int * height
MED_RET_T   # getErrno      # void

#k_inc#lib/libfloat.h
long # float_modsi3# long a, long b
long # float_umodsi3# long a, long b
long # float_divsi3# long a, long b
long # float_udivsi3# long a, long b
long long # float_divdi3# long long a, long long b
long long # float_fixsfdi # float a
long long # float_moddi3  # long long a, long long b
long long # float_ashrdi3 # long long a, unsigned int b
long long # float_ashldi3 # long long a, unsigned int b
float # float_addsf3 # float a, float b
float # float_subsf3 # float a, float b
float # float_mulsf3 # float a, float b
float # float_divsf3 # float a, float b
double # float_floatsidf # register long a
double # float_floatdidf # register long long a
float # float_floatsisf # register long  a
float # float_floatdisf # register long long  a
float # float_negsf2 # float a
double # float_negdf2 # double a
double # float_extendsfdf2 # float a
float # float_truncdfsf2 # double a
long # float_fixsfsi # float a
long # float_fixdfsi # double a
long long # float_fixdfdi # double a
unsigned long # float_fixunsdfsi # double a
unsigned long long # float_fixunsdfdi # double a
double # float_adddf3 # double a, double b
double # float_subdf3 # double a, double b
double # float_muldf3 # double a, double b
double # float_divdf3 # double a, double b
int # float_gtdf2 # double a, double b
int # float_gedf2 # double a, double b
int # float_ltdf2 # double a, double b
int # float_ledf2 # double a, double b
int # float_eqdf2 # double a, double b
int # float_nedf2 # double a, double b
int # float_gtsf2 # float a, float b
int # float_gesf2 # float a, float b
int # float_ltsf2 # float a, float b
int # float_lesf2 # float a, float b
int # float_eqsf2 # float a, float b
int # float_nesf2 # float a, float b

#k_inc#lib/libmath.h
double # math_cos # double a
double # math_sqrt # double a
double # math_tan  # double a

#k_inc#fs/stdfs.h
#api_inc#sys_def/stdfs.h
int            # open      # const char* pathname, int flags
int            # close     # int fdesc
int            # fsync     # int fdesc
int            # read      # int fdesc, void *buf, int count
int            # lseek     # int fdesc, int offset, int whence
int            # write     # int fdesc, const void *buf, int count
int            # truncate  # int fdesc, int length
int            # filesize  # int fdesc
int            # ftell     # int fdesc

DIR*           # opendir    # const char* name
int            # closedir   # DIR* dir
struct dirent* # readdir    # DIR* dir
int            # mkdir      # const char *name, int mode

MED_RET_T      # fs_pwd     # char * name
MED_RET_T      # fs_cd      # char * name

#k_inc#gfx/graphics.h
#api_inc#sys_def/graphics.h
void   # gfx_openGraphics          # void
void   # gfx_initGraphics          # void
void   # gfx_closeGraphics         # void
void   # gfx_setPlane              # int vplane
int    # gfx_getPlane              # void
void   # gfx_planeHide             # int vplane
void   # gfx_planeShow             # int vplane
int    # gfx_planeIsShown          # int vplane
void   # gfx_planeSetState         # int vplane,int state
int    # gfx_planeGetState         # int vplane
void   # gfx_planeSetSize          # int vplane,int width,int height,int bitsPerPixel
void   # gfx_planeGetSize          # int vplane,int * width,int * height,int * bitsPerPixel
void   # gfx_planeSetPos           # int vplane,int x,int y
void   # gfx_planeGetPos           # int vplane,int * x,int * y
void*  # gfx_planeGetBufferOffset  # int vplane
void   # gfx_planeSetBufferOffset  # int vplane, void * offset
void   # gfx_clearScreen           # unsigned int color
void   # gfx_drawPixel             # unsigned int color,int x, int y
unsigned int # gfx_readPixel       # int x, int y
void   # gfx_drawRect              # unsigned int color, int x, int y, int width, int height
void   # gfx_fillRect              # unsigned int color, int x, int y, int width, int height
void   # gfx_drawLine              # unsigned int color, int x1, int y1, int x2, int y2
void   # gfx_putS                  # unsigned int color, unsigned int bg_color, int x, int y, unsigned char *s
void   # gfx_putnS                 # unsigned int color, unsigned int bg_color, int x, int y, int n, unsigned char *s
void   # gfx_getStringSize         # unsigned char *str, int *w, int *h
void   # gfx_putC                  # unsigned int color, unsigned int bg_color, int x, int y, unsigned char s
void   # gfx_drawSprite            # unsigned int * palette, SPRITE * sprite, int x, int y
void   # gfx_drawBitmap            # BITMAP * bitmap, int x, int y
void   # gfx_drawResizedBitmap     # BITMAP * bitmap, int x, int y,int width,int height,int mode
void   # gfx_scrollWindowVert      # unsigned int bgColor, int x, int y, int width, int height, int scroll, int UP
void   # gfx_scrollWindowHoriz     # unsigned int bgColor, int x, int y, int width, int height, int scroll, int RIGHT
void   # gfx_setPalletteRGB        # int r, int g, int b, int index
void   # gfx_setEntirePalette      # int palette[256][3],int size
void   # gfx_fontSet               # int font_nb
int    # gfx_fontGet               # void

#api_inc#stdarg.h
#api_inc#sys_def/types.h
void   # strlwr                    # char *s

void   # qsort     # void* __base, size_t __nmemb, size_t __size, int(*_compar)(const void*, const void*)

int    # atoi                      # const char *str

char * # strdup                    # const char * s
int    # strcasecmp                # const char *s1, const char *s2
int    # strncasecmp               # const char *s1, const char *s2, size_t n
char * # strpbrk                   # const char *,const char *
char * # strtok                    # char *,const char *
char * # strtok_r                  # char *ptr, const char *sep, char **end
char * # strsep                    # char **,const char *
size_t # strspn                    # const char *,const char *
char * # strcpy                    # char *,const char *
char * # strncpy                   # char *,const char *, size_t
char * # strcat                    # char *, const char *
char * # strncat                   # char *, const char *, size_t
int    # strcmp                    # const char *,const char *
int    # strncmp                   # const char *,const char *,size_t
int    # strnicmp                  # const char *, const char *, size_t
char * # strchr                    # const char *,int
char * # strrchr                   # const char *,int
char * # strstr                    # const char *,const char *
size_t # strlen                    # const char *
size_t # strnlen                   # const char *,size_t
void * # memset                    # void *,int,size_t
void * # memcpy                    # void *,const void *,size_t
void * # memmove                   # void *,const void *,size_t
void * # memscan                   # void *,int,size_t
int    # memcmp                    # const void *,const void *,size_t
void * # memchr                    # const void *,int,size_t
int    # vsnprintf                 # char *buf, size_t size, const char *fmt, va_list args
int    # snprintf                  # char * buf, size_t size, const char *fmt, ...
int    # vsprintf                  # char *buf, const char *fmt, va_list args
int    # sprintf                   # char * buf, const char *fmt, ...
int    # vsscanf                   # const char * buf, const char * fmt, va_list args
int    # sscanf                    # const char * buf, const char * fmt, ...

#k_inc#lib/random.h
void   # srand                     # unsigned int seed
int    # rand                      # void

#api_inc#gui_user/widget.h
#k_inc#gui/widget.h
WIDGET                  # widget_create               # void

#api_inc#gui_user/checkbox.h
#k_inc#gui/checkbox.h
CHECKBOX                # checkbox_create             # void

#api_inc#gui_user/trackbar.h
#k_inc#gui/trackbar.h
TRACKBAR                # trackbar_create             # void

#api_inc#gui_user/chooser.h
#k_inc#gui/chooser.h
CHOOSER                 # chooser_create              # void

#api_inc#gui_user/button.h
#k_inc#gui/button.h
BUTTON                  # button_create               # void

#api_inc#gui_user/menu.h
#k_inc#gui/menu.h
MENU                    # menu_create                 # void
MENU_ITEM               # menuItem_create             # void

#api_inc#gui_user/textmenu.h
#k_inc#gui/textmenu.h
TEXTMENU                # textMenu_create             # void
TEXTMENU_ITEM           # textMenuItem_create         # void

#api_inc#gui_user/widgetmenu.h
#k_inc#gui/widgetmenu.h
WIDGETMENU              # widgetMenu_create           # void
WIDGETMENU_ITEM         # widgetMenuItem_create       # void
WIDGETMENU_CHECKBOX     # widgetMenuCheckbox_create   # void
WIDGETMENU_TRACKBAR     # widgetMenuTrackbar_create   # void
WIDGETMENU_CHOOSER      # widgetMenuChooser_create    # void

#api_inc#gui_user/iconmenu.h
#k_inc#gui/iconmenu.h
ICONMENU                # iconMenu_create             # void
ICONMENU_ITEM           # iconMenuItem_create         # void

#api_inc#gui_user/widgetlist.h
#k_inc#gui/widgetlist.h
WIDGETLIST              # widgetList_create           # void

#api_inc#gui_user/label.h
#k_inc#gui/label.h
LABEL                   # label_create                # void

#k_inc#gui/file_browser.h
int                     # browser_simpleBrowse        # char * path,char * res

#api_inc#gui_user/icons.h
#k_inc#gui/icons.h
ICON   # icon_load                 # char * filename
ICON   # icon_add                  # char * name,unsigned char * data,int w,int h
ICON   # icon_get                  # char * name

#k_inc#gui/msgBox.h
#api_inc#gui_user/msgBox.h

int    # msgBox_show    # unsigned char* caption, unsigned char* msg, int type, int icon,int evt_hanlder
void   # msgBox_info    # unsigned char* msg

#api_inc#sys_def/cfg_file.h
#k_inc#fs/cfg_file.h

void       # cfg_clear        # CFG_DATA * data

CFG_DATA * # cfg_newFile      #
CFG_DATA * # cfg_readFile     # char * filename
bool       # cfg_writeFile    # CFG_DATA * data, char * filename

void       # cfg_rewindItems  # CFG_DATA * data
bool       # cfg_nextItem     # CFG_DATA * data, char * * name,char * * value

bool       # cfg_itemExists   # CFG_DATA * data, char * name

char *     # cfg_readString   # CFG_DATA * data, char * name
int        # cfg_readInt      # CFG_DATA * data, char * name
bool       # cfg_readBool     # CFG_DATA * data, char * name

void       # cfg_writeString  # CFG_DATA * data, char * name,char * value
void       # cfg_writeInt     # CFG_DATA * data, char * name,int value
void       # cfg_writeBool    # CFG_DATA * data, char * name,bool value

void       # cfg_addDummyLine # CFG_DATA * data, char * text

bool       # cfg_deleteItem   # CFG_DATA * data, char * name

void       # cfg_printItems   # CFG_DATA * data

#k_inc#fs/csv_file.h
MED_RET_T  # csv_newFile      # char * filename
MED_RET_T  # csv_end          # void
MED_RET_T  # csv_line2Array   # int nbItem,char ** item_array,char sepChar
MED_RET_T  # csv_readLine     # void * data,char * formatStr,char sepChar

#k_inc#driver/speaker.h
void       # speaker_enable    # int enable
int        # speaker_state     # void
int        # speaker_available # void

#k_inc#snd/codec.h
#api_inc#snd_user/codec.h
bool         # codec_mustSeek       # int * time
void         # codec_setElapsed     # int elapsed
void         # codec_seekDone       #
bool         # codec_mustContinue   #

#k_inc#snd/output.h
void       # output_write               # void * buffer, int size
void       # output_outputParamsChanged #

#k_inc#snd/buffer.h
#api_inc#snd_user/playlist.h
int             # buffer_seek          # int offset,int whence
int             # buffer_read          # void * buf,int count
PLAYLIST_ITEM * # buffer_getActiveItem #

#k_inc#kernel/irq.h
void    #  irq_changeHandler # int irq_num,void(*fct)(int irq,struct pt_regs * regs)
void    #  irq_enable        # int irq
void    #  irq_disable       # int irq
