#include "Laser.h"
#include "includes.h"

static const MENUITEMS laserItems = {
  LABEL_LASER,
  {{ICON_LASER, LABEL_LASER_TEST},
   {ICON_BACKGROUND, LABEL_BACKGROUND},
   {ICON_BACKGROUND, LABEL_BACKGROUND},
   {ICON_BACKGROUND, LABEL_BACKGROUND},
   {ICON_BACKGROUND, LABEL_BACKGROUND},
   {ICON_BACKGROUND, LABEL_BACKGROUND},
   {ICON_BACKGROUND, LABEL_BACKGROUND},
   {ICON_BACK, LABEL_BACK}}
};

void menuLaser(void)
{
  KEY_VALUES key = KEY_IDLE;
  menuDrawPage(&laserItems);
  while (infoMenu.menu[infoMenu.cur] == menuLaser) {
    key = menuKeyGetValue();
    if (key == KEY_ICON_0) {
      mustStoreCmd("M3 S20\n");
      mustStoreCmd("G4 P1.5\n");
      mustStoreCmd("M5\n");
    } else if (key == KEY_ICON_7) {
      infoMenu.cur--;
    }
    loopProcess();
  }
}
