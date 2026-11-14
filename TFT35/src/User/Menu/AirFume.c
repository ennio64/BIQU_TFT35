#include "AirFume.h"
#include "includes.h"

static const MENUITEMS airFumeItems = {
  LABEL_AIR_FUME,
  {{ICON_AIR,        LABEL_AIR_ASSISTANT_ON},
   {ICON_EXHAUST,    LABEL_EXHAUST_ON},
   {ICON_STOP,       LABEL_ALL_OFF},
   {ICON_BACKGROUND, LABEL_BACKGROUND},
   {ICON_BACKGROUND, LABEL_BACKGROUND},
   {ICON_BACKGROUND, LABEL_BACKGROUND},
   {ICON_BACKGROUND, LABEL_BACKGROUND},
   {ICON_BACK,       LABEL_BACK}}
};

void menuAirFume(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&airFumeItems);
  while (infoMenu.menu[infoMenu.cur] == menuAirFume)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0:
        mustStoreCmd("M7\n");
        break;

      case KEY_ICON_1:
        mustStoreCmd("M8\n");
        break;

      case KEY_ICON_2:
        mustStoreCmd("M9\n");
        break;

      case KEY_ICON_7:
        infoMenu.cur--;
        break;

      default:
        break;
    }
    loopProcess();
  }
}
