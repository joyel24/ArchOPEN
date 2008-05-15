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
void* # malloc   # long size                             # Dynamic_Memory # Memory allocation,only SDRAM is currently available, not IRAM. NULL is returned if there isn't enough space
void* # realloc  # void *buffer, long newsize            # Dynamic_Memory # buffer reallocation, used to change the size of an existing buffer. For size increase, the implementation is not optimum as buffer is moved to a place with enough room
void  # free     # void *buf                             # Dynamic_Memory # free an allocated buffer (standard version)
void  # kfree    # void *buf                             # Dynamic_Memory # free an allocated buffer (kernel version, use it only if you know what you are doing)
void* # calloc   # unsigned int nmemb,unsigned int size  # Dynamic_Memory # allocate a buffer of <i>nmemb</i> element of size <i>size</i>

#k_inc#kernel/lang.h
char* #lang_getStr # int id                                   # Misc # Returns a string in current lang from an id, for a list of idea look in lang file from trunk/tools/lang

#k_inc#kernel/timer.h
unsigned int # tmr_getTick   # void      # Time # returns current tick timer value (1tick=10ms / 100Hz)
unsigned int # tmr_getMicroTick   # void # Time # 1000 times more precise than tmr_getTick (1microtick=10µs / 100Khz)

#k_inc#driver/time.h
#api_inc#sys_def/time.h
MED_RET_T # time_get # struct med_tm * valTime # Time # reads current date/time from clock, struct med_tm is defined in sys_def/time.h
MED_RET_T # time_set # struct med_tm * newTime # Time # changes date/time of clock, struct med_tm is defined in

#k_inc#kernel/delay.h
void # udelay  # unsigned long usecs                          # Time # waits for <i>usecs</i>
void # mdelay  # unsigned long msecs                          # Time # waits for <i>msecs</i>

#k_inc#driver/uart.h
int  # uart_in          # unsigned char * data,int uartNum    # Uart # if data is available in uart <i>uartNum</i> buffer, reads next char from uart (in <i>data</i>) and returns 1, returns 0 otherwise (for uartNum you shall use one of the define from trunk/include/sys_def/uart.h)
void # uart_out         # unsigned char data,int uartNum      # Uart # send char <i>data</i> on uart <i>uartNum</i>
void # uart_outString   # unsigned char * data,int uartNum    # Uart # send a full string (NULL terminated) on uart <i>uartNum</i>
void # uart_changeSpeed # int speed,int uart_num              # Uart # change the speed of uart <i>uartNum</i>

#k_inc#driver/energy.h
void # set_timer_status # int timer_type,int power_mode,int status # Energy_Saving # enable/disable timer according to power mode
void # set_timer_delay  # int timer_type,int power_mode,int delay # Energy_Saving # set timer delay according to power mode
int  # get_timer_status # int timer_type,int power_mode # Energy_Saving # reads timer status according to power mode
int  # get_timer_delay  # int timer_type,int power_mode # Energy_Saving # reads timer delay according to power mode
void # halt_disableTimer# int disable                   # Energy_Saving # pause the Halt timer and keeps it curent setting (usefull for sound player, usb, ...)

#k_inc#driver/batDc.h
int  # DC_isConnected    # void                              # Energy_Saving # DC connector status
int  # batLevel          # void                              # Energy_Saving # measures current bat level

#k_inc#driver/lcd.h
void # lcd_setBrightness # int br                            # Misc # changes brightness (not an Hardware option)
int  # lcd_getBrightness # void                              # Misc # reads current brightness (not an Hardware option)

#k_inc#driver/usb_fw.h
int # usb_isConnected # void                                  # USb_/_FW # checks if usb cable is connected
int # FW_isConnected  # void                                  # USb_/_FW # checks if FireWire cable is connected

#k_inc#kernel/evt.h
#api_inc#sys_def/evt.h
int          # evt_getHandler        # unsigned int mask   # Events # get a new event handler from kernel, returns an error if all handlers are busy
MED_RET_T    # evt_freeHandler       # int num_evt_pipe    # Events # release an event handler
int          # evt_getStatus         # int num_evt_pipe    # Events # reads next event and returns the event number (returns NO_EVENT if there is no event)
MED_RET_T    # evt_getFullStatus     # int num_evt_pipe, struct evt_t * evt # Events # same as aove, but full evt info will be send
int          # evt_getStatusBlocking # int num_evt_pipe # Events # same as evt_getStatus but thread is blocked until an event occurs
const char * # getBtnName            # int btn                              # Buttons # returns button name

#k_inc#driver/buttons.h
#api_inc#sys_def/buttons.h
int # btn_readState # void  # Buttons # direct access to button state, use it with BTMASK_* from include/buttons.h

#k_inc#init/exit.h
void # halt_device     # void  # Misc # launches halt procedure
void # reload_firmware # void  # Misc # according to arch this reloads archos firmware or reset the device

#k_inc#kernel/kernel.h
void # printf        # char *fmt, ...                     # Debug # Prints text on UART and on device screen if mediOS was build with debug on screen
void # print_data    # char * data,int length             # Debug # Outputs a buffer to UART and debug screen
void # do_bkpt       # void                               # Debug # Adds a breakpoint in code for aoMulator (needs specific build option)
int  # getArch       # void  # Arch_Info # gets arch type, include sys_def/arch.h to get available arch
char *      # getArchName   # void           # Arch_Info # gets the name of the running device
void        # getResolution # int * width,int * height # Arch_Info # returns resolution for this arch
MED_RET_T   # getErrno      # void   # Debug # returns last error number

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
int            # open      # const char* pathname, int flags          # File_system
int            # close     # int fdesc                                # File_system
int            # fsync     # int fdesc                                # File_system
int            # read      # int fdesc, void *buf, int count          # File_system
int            # lseek     # int fdesc, int offset, int whence        # File_system
int            # write     # int fdesc, const void *buf, int count    # File_system
int            # truncate  # int fdesc, int length                    # File_system
int            # filesize  # int fdesc                                # File_system
int            # ftell     # int fdesc                                # File_system

DIR*           # opendir    # const char* name                        # File_system
int            # closedir   # DIR* dir                                # File_system
struct dirent* # readdir    # DIR* dir                                # File_system
int            # mkdir      # const char *name, int mode              # File_system

MED_RET_T      # fs_pwd     # char * name                             # File_system
MED_RET_T      # fs_cd      # char * name                             # File_system

#k_inc#gfx/graphics.h
#api_inc#sys_def/graphics.h
void   # gfx_openGraphics          # void                                                      # GFX
void   # gfx_initGraphics          # void                                                      # GFX
void   # gfx_closeGraphics         # void                                                      # GFX
void   # gfx_setPlane              # int vplane                                                # GFX
int    # gfx_getPlane              # void                                                      # GFX
void   # gfx_planeHide             # int vplane                                                # GFX
void   # gfx_planeShow             # int vplane                                                # GFX
int    # gfx_planeIsShown          # int vplane                                                # GFX
void   # gfx_planeSetState         # int vplane,int state                                      # GFX
int    # gfx_planeGetState         # int vplane                                                # GFX
void   # gfx_planeSetSize          # int vplane,int width,int height,int bitsPerPixel,int mode # GFX
void   # gfx_planeGetSize          # int vplane,int * width,int * height,int * bitsPerPixel    # GFX
void   # gfx_planeSetPos           # int vplane,int x,int y                                    # GFX
void   # gfx_planeGetPos           # int vplane,int * x,int * y                                # GFX
void*  # gfx_planeGetBufferOffset  # int vplane                                                # GFX
void   # gfx_planeSetBufferOffset  # int vplane, void * offset                                 # GFX
void   # gfx_setRectCursorColor    # int color,int ram_palette                                 # GFX
void   # gfx_setRectCursorBorder   # int horiz,int verti                                       # GFX
void   # gfx_enableRectCursor      # int state                                                 # GFX
void   # gfx_setRectCursorPos      # int x,int y                                               # GFX
void   # gfx_setRectCursorSize     # int width,int height                                      # GFX
void   # gfx_clearScreen           # unsigned int color                                        # Drawing
void   # gfx_drawPixel             # unsigned int color,int x, int y                           # Drawing
unsigned int # gfx_readPixel       # int x, int y                                              # Drawing
void   # gfx_drawRect              # unsigned int color, int x, int y, int width, int height   # Drawing
void   # gfx_fillRect              # unsigned int color, int x, int y, int width, int height   # Drawing
void   # gfx_drawLine              # unsigned int color, int x1, int y1, int x2, int y2        # Drawing
void   # gfx_putS                  # unsigned int color, unsigned int bg_color, int x, int y, unsigned char *s # Drawing
void   # gfx_putnS                 # unsigned int color, unsigned int bg_color, int x, int y, int n, unsigned char *s # Drawing
void   # gfx_getStringSize         # unsigned char *str, int *w, int *h                        # Drawing
void   # gfx_putC                  # unsigned int color, unsigned int bg_color, int x, int y, unsigned char s  # Drawing
void   # gfx_drawSprite            # unsigned int * palette, SPRITE * sprite, int x, int y     # Drawing
void   # gfx_drawBitmap            # BITMAP * bitmap, int x, int y                             # Drawing
void   # gfx_drawResizedBitmap     # BITMAP * bitmap, int x, int y,int width,int height,int mode               # Drawing
void   # gfx_scrollWindowVert      # unsigned int bgColor, int x, int y, int width, int height, int scroll, int UP # Drawing
void   # gfx_scrollWindowHoriz     # unsigned int bgColor, int x, int y, int width, int height, int scroll, int RIGHT # Drawing
void   # gfx_setPalletteRGB        # int r, int g, int b, int index                            # GFX
void   # gfx_setEntirePalette      # int palette[256][3],int size                              # GFX
void   # gfx_fontSet               # int font_nb                                               # Drawing
int    # gfx_fontGet               # void                                                      # Drawing

#api_inc#sys_def/bmp.h
#k_inc#gfx/bmp.h
MED_RET_T # gfx_loadBmp_toPlane_center # char * filename,int plane                                # Drawing
MED_RET_T # gfx_loadBmp_toPlane_coord  # char * filename,int plane,int x,int y                    # Drawing
MED_RET_T # gfx_loadBmp_toMem_center   # char * filename,char * dest,int w,int h,int output_mode  # Drawing
MED_RET_T # gfx_loadBmp_toMem_coord    # char * filename,char * dest,int w,int h,int output_mode,int x,int y # Drawing

#api_inc#stdarg.h
#api_inc#sys_def/types.h
void   # strlwr                    # char *s                                      # String
void   # qsort     # void* __base, size_t __nmemb, size_t __size, int(*_compar)(const void*, const void*) # String
int    # atoi                      # const char *str                              # String
char * # strdup                    # const char * s                               # String
int    # strcasecmp                # const char *s1, const char *s2               # String
int    # strncasecmp               # const char *s1, const char *s2, size_t n     # String
char * # strpbrk                   # const char *,const char *                    # String
char * # strtok                    # char *,const char *                          # String
char * # strtok_r                  # char *ptr, const char *sep, char **end       # String
char * # strsep                    # char **,const char *                         # String
size_t # strspn                    # const char *,const char *                    # String
char * # strcpy                    # char *,const char *                          # String
char * # strncpy                   # char *,const char *, size_t                  # String
char * # strcat                    # char *, const char *                         # String
char * # strncat                   # char *, const char *, size_t                 # String
int    # strcmp                    # const char *,const char *                    # String
int    # strncmp                   # const char *,const char *,size_t             # String
int    # strnicmp                  # const char *, const char *, size_t           # String
char * # strchr                    # const char *,int                             # String
char * # strrchr                   # const char *,int                             # String
char * # strstr                    # const char *,const char *                    # String
size_t # strlen                    # const char *                                 # String
size_t # strnlen                   # const char *,size_t                          # String
void * # memset                    # void *,int,size_t                            # String
void * # memcpy                    # void *,const void *,size_t                   # String
void * # memmove                   # void *,const void *,size_t                   # String
void * # memscan                   # void *,int,size_t                            # String
int    # memcmp                    # const void *,const void *,size_t             # String
void * # memchr                    # const void *,int,size_t                      # String
int    # vsnprintf                 # char *buf, size_t size, const char *fmt, va_list args   # String
int    # snprintf                  # char * buf, size_t size, const char *fmt, ...           # String
int    # vsprintf                  # char *buf, const char *fmt, va_list args                # String
int    # sprintf                   # char * buf, const char *fmt, ...                        # String
int    # vsscanf                   # const char * buf, const char * fmt, va_list args        # String
int    # sscanf                    # const char * buf, const char * fmt, ...                 # String
double # atof                      # char * str                                   # String
double # strtod                    # char * str, char ** ptr                      # String

#k_inc#lib/random.h
void   # srand                     # unsigned int seed                            # Misc # Init random generator
int    # rand                      # void                                         # Misc # get a random number

#api_inc#gui_user/widget.h
#k_inc#gui/widget.h
WIDGET                  # widget_create               # void       # GUI

#api_inc#gui_user/checkbox.h
#k_inc#gui/checkbox.h
CHECKBOX                # checkbox_create             # void       # GUI

#api_inc#gui_user/trackbar.h
#k_inc#gui/trackbar.h
TRACKBAR                # trackbar_create             # void       # GUI

#api_inc#gui_user/chooser.h
#k_inc#gui/chooser.h
CHOOSER                 # chooser_create              # void       # GUI

#api_inc#gui_user/button.h
#k_inc#gui/button.h
BUTTON                  # button_create               # void       # GUI

#api_inc#gui_user/menu.h
#k_inc#gui/menu.h
MENU                    # menu_create                 # void       # GUI
MENU_ITEM               # menuItem_create             # void       # GUI

#api_inc#gui_user/textmenu.h
#k_inc#gui/textmenu.h
TEXTMENU                # textMenu_create             # void       # GUI
TEXTMENU_ITEM           # textMenuItem_create         # void       # GUI

#api_inc#gui_user/widgetmenu.h
#k_inc#gui/widgetmenu.h
WIDGETMENU              # widgetMenu_create           # void       # GUI
WIDGETMENU_ITEM         # widgetMenuItem_create       # void       # GUI
WIDGETMENU_CHECKBOX     # widgetMenuCheckbox_create   # void       # GUI
WIDGETMENU_TRACKBAR     # widgetMenuTrackbar_create   # void       # GUI
WIDGETMENU_CHOOSER      # widgetMenuChooser_create    # void       # GUI

#api_inc#gui_user/iconmenu.h
#k_inc#gui/iconmenu.h
ICONMENU                # iconMenu_create             # void       # GUI
ICONMENU_ITEM           # iconMenuItem_create         # void       # GUI

#api_inc#gui_user/widgetlist.h
#k_inc#gui/widgetlist.h
WIDGETLIST              # widgetList_create           # void       # GUI

#api_inc#gui_user/label.h
#k_inc#gui/label.h
LABEL                   # label_create                # void       # GUI

#k_inc#gui/file_browser.h
int                     # browser_simpleBrowse        # char * path,char * res       # GUI

#api_inc#gui_user/icons.h
#k_inc#gui/icons.h
ICON   # icon_load                 # char * filename                               # GUI
ICON   # icon_add                  # char * name,unsigned char * data,int w,int h  # GUI
ICON   # icon_get                  # char * name                                   # GUI

#k_inc#gui/msgBox.h
#api_inc#gui_user/msgBox.h

int    # msgBox_show    # unsigned char* caption, unsigned char* msg, int type, int icon,int evt_hanlder  # GUI
void   # msgBox_info    # unsigned char* msg                                                              # GUI

#k_inc#gui/virtKbd.h
void   # virtKbd        # int evt_hanlder, char * str          # GUI # Virtual Keyboard, str will be changed by the string inputed, evt_handler should be an active event pipe get from evt_getHandler

#api_inc#sys_def/cfg_file.h
#k_inc#fs/cfg_file.h

void       # cfg_clear        # CFG_DATA * data                                 # Cfg_file # deallocates data (you must call this when you're finished with data)

CFG_DATA * # cfg_newFile      # void                                            # Cfg_file # allocates data for a new file
CFG_DATA * # cfg_readFile     # char * filename                                 # Cfg_file # allocates and reads data from an existing file, returns NULL if the file can't be red
bool       # cfg_writeFile    # CFG_DATA * data, char * filename                # Cfg_file # writes data to a file, returns true upon success

void       # cfg_rewindItems  # CFG_DATA * data                                 # Cfg_file # used to enumerate all items, returns to the first one
bool       # cfg_nextItem     # CFG_DATA * data, char * * name,char * * value   # Cfg_file # used to enumerate all items, reads the next item and returns its name and value

bool       # cfg_itemExists   # CFG_DATA * data, char * name                    # Cfg_file # returns true if the item exists

char *     # cfg_readString   # CFG_DATA * data, char * name                    # Cfg_file # reads a string item
int        # cfg_readInt      # CFG_DATA * data, char * name                    # Cfg_file # reads an integer item
bool       # cfg_readBool     # CFG_DATA * data, char * name                    # Cfg_file # reads a boolean item

void       # cfg_writeString  # CFG_DATA * data, char * name,char * value       # Cfg_file # writes a string item
void       # cfg_writeInt     # CFG_DATA * data, char * name,int value          # Cfg_file # writes an integer item
void       # cfg_writeBool    # CFG_DATA * data, char * name,bool value         # Cfg_file # writes a boolean item

void       # cfg_addDummyLine # CFG_DATA * data, char * text                    # Cfg_file # adds a dummy line (usually a comment)

bool       # cfg_deleteItem   # CFG_DATA * data, char * name                    # Cfg_file # deletes an item, returns true if the item was found and deleted

void       # cfg_printItems   # CFG_DATA * data                                 # Cfg_file # outputs a list of all items in the console, usefull for debug

#k_inc#fs/csv_file.h
MED_RET_T  # csv_newFile      # char * filename                                 # csv_file
MED_RET_T  # csv_end          # void                                            # csv_file
MED_RET_T  # csv_line2Array   # int nbItem,char ** item_array,char sepChar      # csv_file
MED_RET_T  # csv_readLine     # void * data,char * formatStr,char sepChar       # csv_file

#k_inc#driver/speaker.h
void       # speaker_enable    # int enable                        # Sound # Changes the state of internal speaker
int        # speaker_state     # void                              # Sound # Returns internal speaker state
int        # speaker_available # void                              # Sound # Returns true if device supports an internal speaker

#k_inc#snd/codec.h
#api_inc#snd_user/codec.h
bool         # codec_mustSeek       # int * time                   # Sound
void         # codec_setElapsed     # int elapsed                  # Sound
void         # codec_seekDone       # void                         # Sound
bool         # codec_mustContinue   # void                         # Sound

#k_inc#snd/output.h
void       # output_write               # void * buffer, int size  # Sound
void       # output_outputParamsChanged # void                     # Sound

#k_inc#snd/buffer.h
#api_inc#snd_user/playlist.h
int             # buffer_seek          # int offset,int whence     # Sound
int             # buffer_read          # void * buf,int count      # Sound
PLAYLIST_ITEM * # buffer_getActiveItem # void                      # Sound

#k_inc#kernel/irq.h
void    #  irq_changeHandler # int irq_num,void(*fct)(int irq,struct pt_regs * regs) # Irq # Change Handler for a given IRQ
void    #  irq_enable        # int irq                              # Irq # enable IRQ
void    #  irq_disable       # int irq                              # Irq # disable IRQ

#k_inc#kernel/thread.h
unsigned long   #  yield # void                                    # Thread # yield current thread

#k_inc#driver/osd.h
int     # osd_getTrspBit         # void                            # GFX
int     # osd_getBlendFactorBit  # int factor                      # GFX

#k_inc#driver/mas.h
#api_inc#snd_user/mas.h
MED_RET_T # mas_chgMode         # int mode                         # Sound # MAS specific: change decoder mode
void      # mas_mp3LaunchDecode # void                             # Sound # MAS specific: launch MP3 decode
void      # mas_setMp3Buffer    # struct mas_sound_buffer * b1,struct mas_sound_buffer * b2 # Sound # MAS specific: init buffers
int       # mas_mp3DecodeState  # void                             # Sound # MAS specific: returns decoder state
void      # mas_mp3StopDecode   # void                             # Sound # MAS specific: stop decoder
void      # mas_i2sChgSRate     # int                              # Sound # MAS specific: change the sample rate for I2S PCM decoder
void      # mas_clearMp3Buffer  # void                             # Sound # MAS specific: clear buffer config
