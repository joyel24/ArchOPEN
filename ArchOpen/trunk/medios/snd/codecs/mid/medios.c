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
  gfx_clearScreen(COLOR_TRSP);
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
