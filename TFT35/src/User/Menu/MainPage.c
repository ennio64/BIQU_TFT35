#include "MainPage.h"
#include "includes.h"

const MENUITEMS mainPageItems = {
  LABEL_READY,
  {{ICON_HOME,     LABEL_HOME},       /* HOME */
   {ICON_MOVE,     LABEL_JOG},        /* JOG */
   {ICON_ORIGIN,   LABEL_ORIGIN},     /* ORIGINE */
   {ICON_FOCUS,    LABEL_Z_FOCUS},    /* FUOCO Z */
   {ICON_LASER,    LABEL_LASER},      /* LASER */
   {ICON_AIR,      LABEL_AIR_FUME},   /* ARIA/FUMI */
   {ICON_SETTINGS, LABEL_SETTINGS},   /* SETUP */
   {ICON_STOP,     LABEL_STOP}}       /* STOP */
};

static void drawMainMpos(void)
{
  static float lastPos[3] = {0.0f, 0.0f, 0.0f};
  static bool haveValues = false;
  const float x = coordinateGetAxis(X_AXIS);
  const float y = coordinateGetAxis(Y_AXIS);
  const float z = coordinateGetAxis(Z_AXIS);

  if (haveValues && lastPos[0] == x && lastPos[1] == y && lastPos[2] == z)
    return;

  lastPos[0] = x;
  lastPos[1] = y;
  lastPos[2] = z;
  haveValues = true;

  GUI_ClearRect(0, 20, 260, 100);
  GUI_DispString(10, 25, (u8 *)"MPos X", 1);
  GUI_DispFloat(90, 25, x, 3, 3);
  GUI_DispString(10, 45, (u8 *)"MPos Y", 1);
  GUI_DispFloat(90, 45, y, 3, 3);
  GUI_DispString(10, 65, (u8 *)"MPos Z", 1);
  GUI_DispFloat(90, 65, z, 3, 3);
}

enum
{
  MAIN_STOP_IDLE = 0,
  MAIN_STOP_RESET_SENT,
  MAIN_STOP_WAIT_RESET,
  MAIN_STOP_UNLOCK_SENT,
};

static u8 mainStopState = MAIN_STOP_IDLE;
static u32 mainStopStamp = 0;

static void mainStopHandle(void)
{
  switch (mainStopState)
  {
    case MAIN_STOP_IDLE:
      break;

    case MAIN_STOP_RESET_SENT:
      if ((OS_GetTime() - mainStopStamp) >= 100U)
      {
        mainStopStamp = OS_GetTime();
        mainStopState = MAIN_STOP_WAIT_RESET;
      }
      break;

    case MAIN_STOP_WAIT_RESET:
      if (infoHost.connected == true)
      {
        infoHost.wait = false;
        USART1_Puts("$X\n");
        mainStopState = MAIN_STOP_UNLOCK_SENT;
      }
      else if ((OS_GetTime() - mainStopStamp) >= 350U)
      {
        reminderMessage(LABEL_WARNING, STATUS_NORMAL);
        mainStopState = MAIN_STOP_IDLE;
      }
      break;

    case MAIN_STOP_UNLOCK_SENT:
      if (infoHost.connected == false)
      {
        reminderMessage(LABEL_WARNING, STATUS_NORMAL);
        mainStopState = MAIN_STOP_IDLE;
      }
      break;

    default:
      break;
  }
}

void menuMain(void)
{
  KEY_VALUES key = KEY_IDLE;
  static u32 lastPosRefresh = 0;

  mainStopState = MAIN_STOP_IDLE;
  mainStopStamp = 0;

  menuDrawPage(&mainPageItems);
  drawMainMpos();
  lastPosRefresh = OS_GetTime();

  while (infoMenu.menu[infoMenu.cur] == menuMain) {
    key = menuKeyGetValue();
    switch (key) {
      case KEY_ICON_0: infoMenu.menu[++infoMenu.cur] = menuHome; break;
      case KEY_ICON_1: infoMenu.menu[++infoMenu.cur] = menuMove; break;
      case KEY_ICON_2: infoMenu.menu[++infoMenu.cur] = menuOrigin; break;
      case KEY_ICON_3: infoMenu.menu[++infoMenu.cur] = menuFocusZ; break;
      case KEY_ICON_4: infoMenu.menu[++infoMenu.cur] = menuLaser; break;
      case KEY_ICON_5: infoMenu.menu[++infoMenu.cur] = menuAirFume; break;
      case KEY_ICON_6: infoMenu.menu[++infoMenu.cur] = menuSettings; break;
      case KEY_ICON_7:
        if (mainStopState == MAIN_STOP_IDLE)
        {
          mainStopState = MAIN_STOP_RESET_SENT;
          mainStopStamp = OS_GetTime();
          infoHost.connected = false;
          infoHost.wait = false;
          parseAckInvalidateMPos();
          USART1_Puts("\x18");
        }
        break;
      default: break;
    }

    mainStopHandle();

    if ((OS_GetTime() - lastPosRefresh) >= 25U)
    {
      drawMainMpos();
      lastPosRefresh = OS_GetTime();
    }

    loopProcess();
  }
}
