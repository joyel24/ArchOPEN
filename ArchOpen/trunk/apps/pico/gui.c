#include "shared.h"
#include "pico.h"
#include "gui.h"

WIDGETLIST menuList;
WIDGETMENU standardMenu, soundMenu, gfxMenu, inputsMenu;

char * tvOut_items[]={"Off","PAL","NTSC"};
char * input_items[]={"A","B","C","Start"};

bool gui_browserNeedInit=true;
struct browser_data * browser;

bool gui_wantExit;

int gui_eventHandler;

extern int state_slot;
extern int tvOut;
extern int key_btn1_config, key_btn2_config, key_f3_config, key_on_config;

void menu_onClick(MENU m, WIDGETMENU_ITEM mi)
{

    state_slot=standardMenu->getTrackbar(standardMenu,standardMenu->indexFromCaption(standardMenu,"Slot"))->value;

    // no oc during hdd access
    clk_overclock(false);

    if (!strcmp(mi->caption,"Load state"))
    {
        gui_showText("Load state");
        SaveLoadGame(1,0);
        gui_wantExit=true;
    }

    if (!strcmp(mi->caption,"Save state"))
    {
        gui_showText("Save state");
        SaveLoadGame(0,0);
        gui_wantExit=true;
    }

    clk_overclock(true);
}

void gui_init()
{
    int w, h;
    WIDGETMENU_ITEM mi;
    WIDGETMENU_CHOOSER mih;
    WIDGETMENU_CHECKBOX mic;
    WIDGETMENU_TRACKBAR mit;

    getResolution(&w, &h);

    icon_init();
    iniBrowser();
    browser=browser_NewBrowse();
    browser->mode=MODE_STRING;
    gui_browserNeedInit=true;

    gui_eventHandler = evt_getHandler(BTN_CLASS|GUI_CLASS);

    menuList=widgetList_create();
    menuList->ownWidgets=true;

    standardMenu=widgetMenu_create();
    standardMenu->setRect(standardMenu, 0, 0, w, h);
    standardMenu->ownWidgets=true;
    standardMenu->onClick=(MENU_CLICKEVENT)menu_onClick;
    standardMenu->menuList=menuList;
    menuList->addWidget(menuList,standardMenu);

    mi=widgetMenuItem_create();
    mi->caption="General settings : ";
    mi->foreColor=COLOR_RED;
    mi->canFocus=false;
    mi->widgetWidth=0;
    standardMenu->addItem(standardMenu,mi);

    mit=widgetMenuTrackbar_create();
    mit->caption="Volume";
    mit->cfgStored=true;
    mit->cfgName="volume";
    mit->trackbar->maximum=100;
    mit->trackbar->value=75;
    standardMenu->addItem(standardMenu,mit);

    mit=widgetMenuTrackbar_create();
    mit->caption="Backlight strength";
    mit->cfgStored=true;
    mit->cfgName="backlight";
    mit->trackbar->maximum=100;
    mit->trackbar->value=100;
    standardMenu->addItem(standardMenu,mit);

    mic=widgetMenuCheckbox_create();
    mic->caption="Z80 emulation : ";
    mic->cfgStored=true;
    mic->cfgName="z80_emulation";
    mic->checkbox->checked=0;
    standardMenu->addItem(standardMenu, mic);

    mic=widgetMenuCheckbox_create();
    mic->caption="Overclocking : ";
    mic->checkbox->checked=0;
    mic->checkbox->caption="Enabled";
    standardMenu->addItem(standardMenu, mic);

    mit=widgetMenuTrackbar_create();
    mit->caption="ARM Clock : ";
    mit->cfgStored=true;
    mit->cfgName="arm_clock";
    mit->trackbar->maximum=133;
    mit->trackbar->minimum=75;
    mit->trackbar->value=clkc_getClockFrequency(CLK_ARM)/1000000;
    standardMenu->addItem(standardMenu,mit);

    mih=widgetMenuChooser_create();
    mih->caption="TV out";
    mih->cfgName="tv_out";
    mih->cfgStored=false;
    mih->chooser->items=tvOut_items;
    mih->chooser->itemCount=3;
    mih->chooser->index=0;
    standardMenu->addItem(standardMenu,mih);

    mit=widgetMenuTrackbar_create();
    mit->caption="Slot";
    mit->trackbar->maximum=9;
    mit->trackbar->minimum=1;
    mit->trackbar->value=1;
    mit->trackbar->numTicks=7;
    standardMenu->addItem(standardMenu,mit);

    mi=widgetMenuItem_create();
    mi->caption="Load state";
    mi->widgetWidth=0;
    standardMenu->addItem(standardMenu,mi);

    mi=widgetMenuItem_create();
    mi->caption="Save state";
    mi->widgetWidth=0;
    standardMenu->addItem(standardMenu,mi);

    mi=widgetMenuItem_create();
    mi->caption="Sound menu";
    mi->widgetWidth=0;
    standardMenu->addItem(standardMenu,mi);

    mi=widgetMenuItem_create();
    mi->caption="Gfx menu";
    mi->widgetWidth=0;
    standardMenu->addItem(standardMenu,mi);

    mi=widgetMenuItem_create();
    mi->caption="Inputs menu";
    mi->widgetWidth=0;
    standardMenu->addItem(standardMenu,mi);

    // Gfx menu
    gfxMenu=widgetMenu_create();
    gfxMenu->setRect(gfxMenu, 0, 0, w, h);
    gfxMenu->ownWidgets=true;
    gfxMenu->onClick=(MENU_CLICKEVENT)menu_onClick;
    gfxMenu->menuList=menuList;
    menuList->addWidget(menuList,gfxMenu);

    mic=widgetMenuCheckbox_create();
    mic->caption="Frame renderer : ";
    mic->cfgStored=true;
    mic->cfgName="frame_renderer";
    mic->checkbox->checked=0;
    gfxMenu->addItem(gfxMenu, mic);

    mic=widgetMenuCheckbox_create();
    mic->caption="Plane A : ";
    mic->checkbox->checked=1;
    gfxMenu->addItem(gfxMenu, mic);

    mic=widgetMenuCheckbox_create();
    mic->caption="Plane B : ";
    mic->checkbox->checked=1;
    gfxMenu->addItem(gfxMenu, mic);

    mic=widgetMenuCheckbox_create();
    mic->caption="Sprites : ";
    mic->checkbox->checked=1;
    gfxMenu->addItem(gfxMenu, mic);

    // sound menu
    soundMenu=widgetMenu_create();
    soundMenu->setRect(soundMenu, 0, 0, w, h);
    soundMenu->ownWidgets=true;
    soundMenu->onClick=(MENU_CLICKEVENT)menu_onClick;
    soundMenu->menuList=menuList;
    menuList->addWidget(menuList,soundMenu);

    mic=widgetMenuCheckbox_create();
    mic->caption="YM2612 : ";
    mic->cfgStored=true;
    mic->cfgName="ym2612";
    mic->checkbox->checked=1;
    soundMenu->addItem(soundMenu, mic);

    mic=widgetMenuCheckbox_create();
    mic->caption="PSG : ";
    mic->cfgStored=true;
    mic->cfgName="psg";
    mic->checkbox->checked=1;
    soundMenu->addItem(soundMenu, mic);

    mic=widgetMenuCheckbox_create();
    mic->caption="DAC : ";
    mic->cfgStored=true;
    mic->cfgName="dac";
    mic->checkbox->checked=1;
    soundMenu->addItem(soundMenu, mic);

    // inputs menu
    inputsMenu=widgetMenu_create();
    inputsMenu->setRect(inputsMenu, 0, 0, w, h);
    inputsMenu->ownWidgets=true;
    inputsMenu->onClick=(MENU_CLICKEVENT)menu_onClick;
    inputsMenu->menuList=menuList;
    menuList->addWidget(menuList,inputsMenu);

    mih=widgetMenuChooser_create();
    mih->caption="BTN1:";
    mih->cfgName="btn1";
    mih->cfgStored=true;
    mih->chooser->items=input_items;
    mih->chooser->itemCount=4;
    mih->chooser->index=0;
    inputsMenu->addItem(inputsMenu,mih);

    mih=widgetMenuChooser_create();
    mih->caption="BTN2:";
    mih->cfgName="btn2";
    mih->cfgStored=true;
    mih->chooser->items=input_items;
    mih->chooser->itemCount=4;
    mih->chooser->index=1;
    inputsMenu->addItem(inputsMenu,mih);

    mih=widgetMenuChooser_create();
    mih->caption="F3:";
    mih->cfgName="f3";
    mih->cfgStored=true;
    mih->chooser->items=input_items;
    mih->chooser->itemCount=4;
    mih->chooser->index=3;
    inputsMenu->addItem(inputsMenu,mih);

    mih=widgetMenuChooser_create();
    mih->caption="On:";
    mih->cfgName="on";
    mih->cfgStored=true;
    mih->chooser->items=input_items;
    mih->chooser->itemCount=4;
    mih->chooser->index=2;
    inputsMenu->addItem(inputsMenu,mih);

    mi=widgetMenuItem_create();
    mi->caption="Back";
    mi->widgetWidth=0;
    soundMenu->addItem(soundMenu,mi);
    gfxMenu->addItem(gfxMenu,mi);
    inputsMenu->addItem(inputsMenu,mi);

    standardMenu->items[standardMenu->indexFromCaption(standardMenu,"Sound menu")]->subMenu=(MENU)soundMenu;
    standardMenu->items[standardMenu->indexFromCaption(standardMenu,"Gfx menu")]->subMenu=(MENU)gfxMenu;
    standardMenu->items[standardMenu->indexFromCaption(standardMenu,"Inputs menu")]->subMenu=(MENU)inputsMenu;
    soundMenu->items[soundMenu->indexFromCaption(soundMenu,"Back")]->subMenu=(MENU)standardMenu;
    gfxMenu->items[gfxMenu->indexFromCaption(gfxMenu,"Back")]->subMenu=(MENU)standardMenu;
    inputsMenu->items[inputsMenu->indexFromCaption(inputsMenu,"Back")]->subMenu=(MENU)standardMenu;

    standardMenu->cfgLoad(standardMenu,CFG_FILE_PATH);
    soundMenu->cfgLoad(soundMenu,CFG_FILE_PATH);
    gfxMenu->cfgLoad(gfxMenu,CFG_FILE_PATH);
    inputsMenu->cfgLoad(inputsMenu,CFG_FILE_PATH);
}

void gui_close()
{
    standardMenu->cfgSave(standardMenu,CFG_FILE_PATH);
    soundMenu->cfgSave(soundMenu,CFG_FILE_PATH);
    gfxMenu->cfgSave(gfxMenu,CFG_FILE_PATH);

    evt_freeHandler(gui_eventHandler);
    menuList->destroy(menuList);

    browser_disposeBrowse(browser);
}

bool gui_browse()
{

    gui_showGuiPlane();

    if (gui_browserNeedInit)
    {
        viewNewDir(browser,NULL);
        gui_browserNeedInit=false;
    }

    redrawBrowser(browser);

    if (browserEvt(browser)==MED_OK)
    {
        sprintf(CurrentROMFile,"%s/%s",browser->path,browser->list[browser->pos+browser->nselect].name);
        return true;
    }
    else
    {
        return false;
    }
}

void gui_showEmuPlane()
{
    gfx_planeHide(BMAP1);
    gfx_setPlane(VID1);
    gfx_planeShow(VID1);
    gfx_clearScreen(COLOR32_BLACK);
}

void gui_showGuiPlane()
{
    gfx_planeHide(VID1);
    gfx_setPlane(BMAP1);
    gfx_clearScreen(COLOR_WHITE);
    gfx_planeShow(BMAP1);
}

void gui_applySettings()
{
    int vol,bl;

    if (gfxMenu->getCheckbox(gfxMenu, gfxMenu->indexFromCaption(gfxMenu,"Frame renderer : "))->checked)
        PicoOpt|=(int)0x10;
    else
        PicoOpt&=(int)0xffef;

    if (standardMenu->getCheckbox(standardMenu, standardMenu->indexFromCaption(standardMenu,"Z80 emulation : "))->checked)
        PicoOpt|=(int)0x4;
    else
        PicoOpt&=0xfffb;

    if (gfxMenu->getCheckbox(gfxMenu, gfxMenu->indexFromCaption(gfxMenu,"Plane A : "))->checked)
        PicoMask|=(int)0x24;
    else
        PicoMask&=0xffdb;

    if (gfxMenu->getCheckbox(gfxMenu, gfxMenu->indexFromCaption(gfxMenu,"Plane B : "))->checked)
        PicoMask|=(int)0x48;
    else
        PicoMask&=0xffb7;

    if (gfxMenu->getCheckbox(gfxMenu, gfxMenu->indexFromCaption(gfxMenu,"Sprites : "))->checked)
        PicoMask|=(int)0x90;
    else
        PicoMask&=0xff6f;

    if (soundMenu->getCheckbox(soundMenu, soundMenu->indexFromCaption(soundMenu,"YM2612 : "))->checked)
    {
        PicoOpt|=(int)0x1;
#ifdef SOUND_USE_DSP
        dspCom->YM2612Enabled=1;
#endif
    }
    else
    {
        PicoOpt&=0xfffe;
#ifdef SOUND_USE_DSP
        dspCom->YM2612Enabled=0;
#endif
    }

    if (soundMenu->getCheckbox(soundMenu, soundMenu->indexFromCaption(soundMenu,"PSG : "))->checked)
    {
        PicoOpt|=0x2;
#ifdef SOUND_USE_DSP
        dspCom->PSGEnabled=1;
#endif
    }
    else
    {
        PicoOpt&=0xfffd;
#ifdef SOUND_USE_DSP
        dspCom->PSGEnabled=0;
#endif
    }

    if (soundMenu->getCheckbox(soundMenu, soundMenu->indexFromCaption(soundMenu,"DAC : "))->checked)
    {
#ifdef SOUND_USE_DSP
        dspCom->DACEnabled=1;
#endif
    }
    else
    {
#ifdef SOUND_USE_DSP
        dspCom->DACEnabled=0;
#endif
    }

    key_btn1_config=0x10<<inputsMenu->getChooser(inputsMenu,inputsMenu->indexFromCaption(inputsMenu,"BTN1:"))->index;
    key_btn2_config=0x10<<inputsMenu->getChooser(inputsMenu,inputsMenu->indexFromCaption(inputsMenu,"BTN2:"))->index;
    key_f3_config=0x10<<inputsMenu->getChooser(inputsMenu,inputsMenu->indexFromCaption(inputsMenu,"F3:"))->index;
    key_on_config=0x10<<inputsMenu->getChooser(inputsMenu,inputsMenu->indexFromCaption(inputsMenu,"On:"))->index;

    overclocking=standardMenu->getCheckbox(standardMenu, standardMenu->indexFromCaption(soundMenu,"Overclocking : "))->checked;
    armFrequency=standardMenu->getTrackbar(standardMenu, standardMenu->indexFromCaption(standardMenu,"ARM Clock : "))->value;
    vol=standardMenu->getTrackbar(standardMenu,standardMenu->indexFromCaption(standardMenu,"Volume"))->value;
    bl=standardMenu->getTrackbar(standardMenu,standardMenu->indexFromCaption(standardMenu,"Backlight strength"))->value;
    tvOut=standardMenu->getChooser(standardMenu,standardMenu->indexFromCaption(standardMenu,"TV out"))->index;

    snd_setVolume(vol);
    lcd_setBrightness(bl);
}

void gui_launchMenu()
{
    int event;

    gui_wantExit=false;

    gui_showGuiPlane();

    // discard waiting events
    evt_purgeHandler(gui_eventHandler);

    // intial paint
    if (menuList->focusedWidget==NULL) menuList->setFocusedWidget(menuList,standardMenu);
    menuList->focusedWidget->paint(menuList->focusedWidget);

    do
    {
        event=evt_getStatusBlocking(gui_eventHandler);
        if (!event) continue; // no new events

        menuList->handleEvent(menuList,event);
    }
    while (event!=BTN_OFF && !gui_wantExit);

    while (btn_readState()); // make sure no button is left pressed

    gui_showEmuPlane();
}

void gui_showText(char * text)
{
    gui_showGuiPlane();
    gfx_putS(COLOR_BLACK,COLOR_WHITE,0,0,text);
}

