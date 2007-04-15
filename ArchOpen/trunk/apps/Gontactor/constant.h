#ifndef GTN_CONSTANT
#define GTN_CONSTANT

//Log
#define LOG_ON

//Files paths
#define GTN_PATH				"/medios/data/Gtn"
#define LOG_FILENAME 			"/medios/data/Gtn/log.txt"
#define GROUP_FILENAME 		"/medios/data/Gtn/groups.cfg"
#define DATABASE_FILENAME 	"/medios/data/Gtn/gtn_bd.db"

//Titles
#define TITLE_GTN "Gontactor 0.7"


//Constant
//Device infos
//~ #define LCD_WIDTH		220
//~ #define LCD_HEIGTH		176

#ifndef LCD_RES_VALUES
#define LCD_RES_VALUES
int LCD_WIDTH, LCD_HEIGTH;
#endif


//gui constant
	//Bar
	#define BAR_TOP_H		15
	#define BAR_BOT_H		11
	//List bend
	#define LIST_LPAD			20
	#define LIST_RPAD			10
	#define CAP_LPAD			10
	
	//print gtn
	#define DLG_HEIGTH		84

	//Colors
//Default theme
	#define COLOR_BG			COLOR_WHITE
	#define COLOR_LINE1		21
	#define COLOR_LINE2		19
	#define COLOR_SHADOW	20

//Blue theme
	//~ #define COLOR_BG			251
	//~ #define COLOR_LINE1		89
	//~ #define COLOR_LINE2		88
	//~ #define COLOR_SHADOW	23



#endif
