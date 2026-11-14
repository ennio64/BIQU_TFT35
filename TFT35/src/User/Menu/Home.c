#include "Home.h"
#include "includes.h"

/* FluidNC homing commands for the configured DLC32 axes.
 * X/Y share homing cycle 2; Z is configured in cycle 1.
 * $H=XY homes X and Y together; $H<axis> homes one axis.
 * Note: individual homing may unlock the controller while other axes
 * remain unhomed; use the complete XY/Z homing sequence before normal work.
 */
static const MENUITEMS homeItems = {
  LABEL_HOME,
  {{ICON_ORIGIN_XY, LABEL_HOME_XY}, /* HOME XY */
   {ICON_X_HOME, LABEL_X_HOME},     /* HOME X  */
   {ICON_Y_HOME, LABEL_Y_HOME},     /* HOME Y  */
   {ICON_Z_HOME, LABEL_Z_HOME},     /* HOME Z  */
   {ICON_BACKGROUND, LABEL_BACKGROUND},
   {ICON_BACKGROUND, LABEL_BACKGROUND},
   {ICON_BACKGROUND, LABEL_BACKGROUND},
   {ICON_BACK, LABEL_BACK}}
};

void menuHome(void)
{
  KEY_VALUES key_num = KEY_IDLE;
  menuDrawPage(&homeItems);

  while (infoMenu.menu[infoMenu.cur] == menuHome)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0: mustStoreCmd("$H=XY\n"); break;
      case KEY_ICON_1: mustStoreCmd("$HX\n");   break;
      case KEY_ICON_2: mustStoreCmd("$HY\n");   break;
      case KEY_ICON_3: mustStoreCmd("$HZ\n");   break;
      case KEY_ICON_7: infoMenu.cur--;           break;
      default: break;
    }
    loopProcess();
  }
}
