#include "Origin.h"
#include "includes.h"

static const MENUITEMS originItems = {
  LABEL_LEVELING,
  {{ICON_ORIGIN_X, LABEL_ORIGIN_X},
   {ICON_ORIGIN_Y, LABEL_ORIGIN_Y},
   {ICON_ORIGIN_XY, LABEL_ORIGIN_XY},
   {ICON_BACKGROUND, LABEL_BACKGROUND},
   {ICON_BACKGROUND, LABEL_BACKGROUND},
   {ICON_BACKGROUND, LABEL_BACKGROUND},
   {ICON_BACKGROUND, LABEL_BACKGROUND},
   {ICON_BACK, LABEL_BACK}}
};

void menuOrigin(void)
{
  KEY_VALUES key = KEY_IDLE;
  menuDrawPage(&originItems);
  while (infoMenu.menu[infoMenu.cur] == menuOrigin) {
    key = menuKeyGetValue();
    switch (key) {
      case KEY_ICON_0: mustStoreCmd("G92 X0\n"); break;
      case KEY_ICON_1: mustStoreCmd("G92 Y0\n"); break;
      case KEY_ICON_2: mustStoreCmd("G92 X0 Y0\n"); break;
      case KEY_ICON_7: infoMenu.cur--; break;
      default: break;
    }
    loopProcess();
  }
}
