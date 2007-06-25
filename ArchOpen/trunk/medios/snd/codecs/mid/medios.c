#include "medios.h"

char currentFile[256]="4444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444444";

static const char _ctype_b[128 + 256] = {
	_CTYPE_DATA_128_256,
	_CTYPE_DATA_0_127,
	_CTYPE_DATA_128_256
};

const char *__ctype_ptr = _ctype_b + 128;

FILE *fopen(const char *file, const char *mode){
  return (FILE *)open(file, (int)O_RDONLY);
}

int access(char *file, int mode){
  int test_fp;
  
  test_fp = open(file, O_RDONLY);
  if ( test_fp >= 0 ) {
    close(test_fp);
    return(0);
  }
  return(-1);
}

int fprintf(int f,const char * s,...){
  switch(f){
  case stderr:
    printf("STDERR: %s",s);
    break;
  case stdout:
    printf("STDOUT: %s",s);
    break;
  default:
    printf("FILE %d: %s",f,s);
    break;
  }

  return 0;
};

int fscanf(int f,const char * s,...){
  return 0;
};

void * getenv(const char * s){
  return NULL;
}

void setbuf(int f,void * p){
}

void getchar(){
}

/* Disclaimer: this is currently just used by CHILL in GDB and therefore
   has not been tested well.  It may have been tested for nothing except
   that it compiles.  */

double atof (const char *str){
  char **ptr=NULL;
  char *p;
  
  if (ptr == (char **)0)
    return atof (str);
  
  p = str;
  
  while (ISSPACE (*p))
    ++p;
  
  if (*p == '+' || *p == '-')
    ++p;

  /* INF or INFINITY.  */
  if ((p[0] == 'i' || p[0] == 'I')
      && (p[1] == 'n' || p[1] == 'N')
      && (p[2] == 'f' || p[2] == 'F'))
    {
      if ((p[3] == 'i' || p[3] == 'I')
	  && (p[4] == 'n' || p[4] == 'N')
	  && (p[5] == 'i' || p[5] == 'I')
	  && (p[6] == 't' || p[6] == 'T')
	  && (p[7] == 'y' || p[7] == 'Y'))
	{
	  *ptr = p + 8;
	  return atof (str);
	}
      else
	{
	  *ptr = p + 3;
	  return atof (str);
	}
    }

  /* NAN or NAN(foo).  */
  if ((p[0] == 'n' || p[0] == 'N')
      && (p[1] == 'a' || p[1] == 'A')
      && (p[2] == 'n' || p[2] == 'N'))
    {
      p += 3;
      if (*p == '(')
	{
	  ++p;
	  while (*p != '\0' && *p != ')')
	    ++p;
	  if (*p == ')')
	    ++p;
	}
      *ptr = p;
      return atof (str);
    }

  /* digits, with 0 or 1 periods in it.  */
  if (ISDIGIT (*p) || *p == '.')
    {
      int got_dot = 0;
      while (ISDIGIT (*p) || (!got_dot && *p == '.'))
	{
	  if (*p == '.')
	    got_dot = 1;
	  ++p;
	}

      /* Exponent.  */
      if (*p == 'e' || *p == 'E')
	{
	  int i;
	  i = 1;
	  if (p[i] == '+' || p[i] == '-')
	    ++i;
	  if (ISDIGIT (p[i]))
	    {
	      while (ISDIGIT (p[i]))
		++i;
	      *ptr = p + i;
	      return atof (str);
	    }
	}
      *ptr = p;
      return atof (str);
    }
  /* Didn't find any digits.  Doesn't look like a number.  */
  *ptr = str;
  return 0.0;
}

#ifdef BUILD_STDALONE
struct browser_data * browser;

bool gui_browse(){
  gfx_openGraphics();

  // browser init
  //icon_init();
  iniBrowser();
  browser=browser_NewBrowse();
  browser->mode=MODE_STRING;

  gfx_planeHide(VID1);
  gfx_setPlane(BMAP1);
  gfx_clearScreen(COLOR_WHITE);
  gfx_planeShow(BMAP1);
  
  //gui_showGuiPlane();
  
  viewNewDir(browser,NULL);

  redrawBrowser(browser);

  if (browserEvt(browser)==MED_OK){
    sprintf(currentFile,"%s/%s",browser->path,browser->list[browser->pos+browser->nselect].name);
    browser_disposeBrowse(browser);
    return true;
  }else{
    browser_disposeBrowse(browser);
    return false;
  }
}

void selectInputFile(){
  gui_browse();
  //*inputFile=currentFile;
}
#else
void selectInputFile(){
}
#endif //BUILD_STDALONE
