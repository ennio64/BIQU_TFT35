#include "Settings.h"
#include "includes.h"

SETTINGS infoSettings;
K40_SETTINGS infoK40Settings;

#if HARDWARE_VERSION == V1_0
  #define VER  "V1.0"
#elif HARDWARE_VERSION == V1_1
  #define VER  "V1.1"
#elif HARDWARE_VERSION == V1_2
  #define VER  "V1.2"
#endif

static void waitForTouchRelease(void)
{
  while(!isPress()) loopProcess();
  while(isPress()) loopProcess();
  infoMenu.cur--;
}

void menuInfo(void)
{
  bool english = infoSettings.language == ENGLISH;
  GUI_Clear(BLACK);
  GUI_DispString(50, 110, (u8 *)("BIGTREETECH TFT35 " VER), 0);
  GUI_DispString(50, 145,
    (u8 *)(english ? "K40 Firmware " VER "." STRINGIFY(SOFTWARE_VERSION)
                   : "Firmware K40 " VER "." STRINGIFY(SOFTWARE_VERSION)), 0);
  GUI_DispString(50, 180, (u8 *)__DATE__, 0);
  waitForTouchRelease();
}

static void menuDiagnostics(void)
{
  char value[48];
  bool english = infoSettings.language == ENGLISH;
  GUI_Clear(BLACK);
  GUI_DispString(30, 70, (u8 *)"FluidNC:", 0);
  GUI_DispString(150, 70,
    (u8 *)(infoHost.connected
      ? (english ? "Connected" : "Connesso")
      : (english ? "Not connected" : "Non connesso")), 0);
  GUI_DispString(30, 110, (u8 *)"MPos Z:", 0);
  if(parseAckMPosFresh(100U))
  {
    my_sprintf(value, "%.3f", coordinateGetAxis(Z_AXIS));
    GUI_DispString(150, 110, (u8 *)value, 0);
  }
  else
  {
    GUI_DispString(150, 110, (u8 *)(english ? "N/A" : "N/D"), 0);
  }
  GUI_DispString(30, 150,
    (u8 *)(parseAckMPosFresh(100U)
      ? (english ? "MPos: fresh" : "MPos: aggiornata")
      : (english ? "MPos: stale" : "MPos: non aggiornata")), 0);
  GUI_DispString(30, 250, textSelect(LABEL_BACK), 0);
  waitForTouchRelease();
}

static void menuCommunication(void)
{
  bool english = infoSettings.language == ENGLISH;
  GUI_Clear(BLACK);
  GUI_DispString(30, 90, (u8 *)"UART: 115200 baud, 8N1", 0);
  GUI_DispString(30, 130,
    (u8 *)(infoHost.connected
      ? (english ? "FluidNC: connected" : "FluidNC: connesso")
      : (english ? "FluidNC: disconnected" : "FluidNC: non connesso")), 0);
  GUI_DispString(30, 250, textSelect(LABEL_BACK), 0);
  waitForTouchRelease();
}

static const MENUITEMS settingsItems = {
  LABEL_SETTINGS,
  {{ICON_LANGUAGE,           LABEL_LANGUAGE},
   {ICON_TOUCHSCREEN_ADJUST, LABEL_TOUCHSCREEN_ADJUST},
   {ICON_SCREEN_INFO,        LABEL_SCREEN_INFO},
   {ICON_SETTINGS,           LABEL_DIAGNOSTICS},
   {ICON_BAUDRATE,           LABEL_COMMUNICATION},
   {ICON_BACKGROUND,         LABEL_BACKGROUND},
   {ICON_BACKGROUND,         LABEL_BACKGROUND},
   {ICON_BACK,               LABEL_BACK}}
};

void menuSettings(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  uint8_t beforeLanguage = infoSettings.language;

  menuDrawPage(&settingsItems);
  while(infoMenu.menu[infoMenu.cur] == menuSettings)
  {
    key_num = menuKeyGetValue();
    switch(key_num)
    {
      case KEY_ICON_0:
        infoSettings.language = (infoSettings.language == ITALIAN) ? ENGLISH : ITALIAN;
        menuDrawPage(&settingsItems);
        break;

      case KEY_ICON_1:
        TSC_Calibration();
        if(storePara() == false)
          reminderMessage(LABEL_STORAGE_ERROR, STATUS_NORMAL);
        menuDrawPage(&settingsItems);
        break;

      case KEY_ICON_2:
        infoMenu.menu[++infoMenu.cur] = menuInfo;
        break;

      case KEY_ICON_3:
        infoMenu.menu[++infoMenu.cur] = menuDiagnostics;
        break;

      case KEY_ICON_4:
        infoMenu.menu[++infoMenu.cur] = menuCommunication;
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default:
        break;
    }
    loopProcess();
  }

  if(beforeLanguage != infoSettings.language)
  {
    if(storePara() == false)
      reminderMessage(LABEL_STORAGE_ERROR, STATUS_NORMAL);
  }
}
