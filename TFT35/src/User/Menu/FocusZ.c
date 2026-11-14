#include "FocusZ.h"
#include "includes.h"

#define MPOS_FRESH_TICKS 100U
#define FOCUS_THICKNESS_MAX Z_MAX_POS

static const MENUITEMS focusItems = {
  LABEL_Z_FOCUS,
  {{ICON_ORIGIN, LABEL_PLANE_0},
   {ICON_1_MM, LABEL_THICKNESS},
   {ICON_Z_INC, LABEL_JOG_Z},
   {ICON_FOCUS, LABEL_GO_TO_FOCUS},
   {ICON_BACKGROUND, LABEL_BACKGROUND},
   {ICON_BACKGROUND, LABEL_BACKGROUND},
   {ICON_BACKGROUND, LABEL_BACKGROUND},
   {ICON_BACK, LABEL_BACK}}
};

static const MENUITEMS plane0Items = {
  LABEL_PLANE_0,
  {{ICON_ORIGIN_XY, LABEL_SAVE},
   {ICON_BACKGROUND, LABEL_BACKGROUND},
   {ICON_BACKGROUND, LABEL_BACKGROUND},
   {ICON_BACKGROUND, LABEL_BACKGROUND},
   {ICON_BACKGROUND, LABEL_BACKGROUND},
   {ICON_BACKGROUND, LABEL_BACKGROUND},
   {ICON_BACKGROUND, LABEL_BACKGROUND},
   {ICON_BACK, LABEL_BACK}}
};

static MENUITEMS thicknessItems = {
  LABEL_THICKNESS,
  {{ICON_DEC, LABEL_DEC},
   {ICON_BACKGROUND, LABEL_BACKGROUND},
   {ICON_1_MM, LABEL_STEP},
   {ICON_INC, LABEL_INC},
   {ICON_SETTINGS, LABEL_SAVE},
   {ICON_BACKGROUND, LABEL_BACKGROUND},
   {ICON_BACKGROUND, LABEL_BACKGROUND},
   {ICON_BACK, LABEL_BACK}}
};

static MENUITEMS focusJogItems = {
  LABEL_JOG_Z,
  {{ICON_Z_DEC, LABEL_Z_DEC},
   {ICON_BACKGROUND, LABEL_BACKGROUND},
   {ICON_BACKGROUND, LABEL_BACKGROUND},
   {ICON_01_MM, LABEL_01_MM},
   {ICON_Z_INC, LABEL_Z_INC},
   {ICON_BACKGROUND, LABEL_BACKGROUND},
   {ICON_BACKGROUND, LABEL_BACKGROUND},
   {ICON_BACK, LABEL_BACK}}
};

static const float thicknessSteps[] = {0.1f, 0.5f, 1.0f};
static u8 thicknessStepIndex = 0;
static const ITEM jogStepItems[] = {
  {ICON_001_MM, LABEL_001_MM},
  {ICON_01_MM, LABEL_01_MM},
  {ICON_1_MM, LABEL_1_MM}
};

static void updateThicknessStepLabel(void)
{
  thicknessItems.items[KEY_ICON_2].label =
    (thicknessStepIndex == 1) ? LABEL_05_MM
    : (thicknessStepIndex == 2) ? LABEL_1_MM : LABEL_01_MM;
}

static void drawFocusValue(int16_t label, float value)
{
  GUI_ClearRect(160, 10, 470, 10 + BYTE_HEIGHT);
  GUI_DispString(170, 10, textSelect(label), 0);
  GUI_DispFloat(300, 10, value, 3, 2);
}

static void requestMPos(void)
{
  USART1_Puts("?");
}

static void drawPlane0Info(void)
{
  GUI_ClearRect(160, 10, 470, 55);
  GUI_DispString(170, 10, (u8 *)"P0:", 0);
  if(infoK40Settings.plane0_valid)
    GUI_DispFloat(220, 10, infoK40Settings.plane0_z, 3, 2);
  else
    GUI_DispString(220, 10, (u8 *)"N/D", 0);

  GUI_DispString(170, 30, (u8 *)"MPos Z:", 0);
  if(parseAckMPosFresh(MPOS_FRESH_TICKS))
    GUI_DispFloat(270, 30, coordinateGetAxis(Z_AXIS), 3, 2);
  else
    GUI_DispString(270, 30, (u8 *)"N/D", 0);
}

static void menuFocusPlane0(void)
{
  KEY_VALUES key = KEY_IDLE;
  u32 lastRequest = OS_GetTime();
  u32 lastInfoRefresh = OS_GetTime();

  menuDrawPage(&plane0Items);
  requestMPos();
  drawPlane0Info();
  while(infoMenu.menu[infoMenu.cur] == menuFocusPlane0)
  {
    key = menuKeyGetValue();
    if(key == KEY_ICON_0)
    {
      float z = coordinateGetAxis(Z_AXIS);
      if(infoHost.connected == false || parseAckMPosFresh(MPOS_FRESH_TICKS) == false)
      {
        reminderMessage(LABEL_MPOS_NOT_READY, STATUS_NORMAL);
      }
      else if(z < -Z_MAX_POS || z > 0.0f)
      {
        reminderMessage(LABEL_Z_OUT_OF_RANGE, STATUS_NORMAL);
      }
      else
      {
        K40_SETTINGS previous = infoK40Settings;
        infoK40Settings.plane0_z = z;
        infoK40Settings.plane0_valid = true;
        if(storePara())
          reminderMessage(LABEL_PLANE_0_SAVED, STATUS_NORMAL);
        else
        {
          infoK40Settings = previous;
          reminderMessage(LABEL_STORAGE_ERROR, STATUS_NORMAL);
        }
      }
    }
    else if(key == KEY_ICON_7)
    {
      infoMenu.cur--;
    }

    if((OS_GetTime() - lastRequest) >= MPOS_FRESH_TICKS)
    {
      requestMPos();
      lastRequest = OS_GetTime();
    }
    if((OS_GetTime() - lastInfoRefresh) >= 25U)
    {
      drawPlane0Info();
      lastInfoRefresh = OS_GetTime();
    }
    loopProcess();
  }
}

static void menuFocusThickness(void)
{
  KEY_VALUES key = KEY_IDLE;
  float workingThickness = infoK40Settings.material_thickness;

  updateThicknessStepLabel();
  menuDrawPage(&thicknessItems);
  drawFocusValue(LABEL_THICKNESS, workingThickness);
  while(infoMenu.menu[infoMenu.cur] == menuFocusThickness)
  {
    key = menuKeyGetValue();
    switch(key)
    {
      case KEY_ICON_0:
        workingThickness -= thicknessSteps[thicknessStepIndex];
        if(workingThickness < 0.0f) workingThickness = 0.0f;
        drawFocusValue(LABEL_THICKNESS, workingThickness);
        break;

      case KEY_ICON_2:
        thicknessStepIndex = (thicknessStepIndex + 1)
                           % (sizeof(thicknessSteps) / sizeof(thicknessSteps[0]));
        updateThicknessStepLabel();
        menuDrawItem(&thicknessItems.items[KEY_ICON_2], KEY_ICON_2);
        break;

      case KEY_ICON_3:
        workingThickness += thicknessSteps[thicknessStepIndex];
        if(workingThickness > FOCUS_THICKNESS_MAX)
          workingThickness = FOCUS_THICKNESS_MAX;
        drawFocusValue(LABEL_THICKNESS, workingThickness);
        break;

      case KEY_ICON_4:
        if(workingThickness < 0.1f || workingThickness > FOCUS_THICKNESS_MAX)
        {
          reminderMessage(LABEL_INVALID_THICKNESS, STATUS_NORMAL);
        }
        else
        {
          float previousThickness = infoK40Settings.material_thickness;
          infoK40Settings.material_thickness = workingThickness;
          if(storePara())
            reminderMessage(LABEL_THICKNESS_SAVED, STATUS_NORMAL);
          else
          {
            infoK40Settings.material_thickness = previousThickness;
            reminderMessage(LABEL_STORAGE_ERROR, STATUS_NORMAL);
          }
        }
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

static void menuFocusJog(void)
{
  KEY_VALUES key = KEY_IDLE;

  focusJogItems.items[KEY_ICON_3] = jogStepItems[moveGetJogStepIndex()];
  menuDrawPage(&focusJogItems);
  while(infoMenu.menu[infoMenu.cur] == menuFocusJog)
  {
    key = menuKeyGetValue();
    switch(key)
    {
      case KEY_ICON_0: moveJogZ(false); break;
      case KEY_ICON_3:
        moveCycleJogStep();
        focusJogItems.items[KEY_ICON_3] = jogStepItems[moveGetJogStepIndex()];
        menuDrawItem(&focusJogItems.items[KEY_ICON_3], KEY_ICON_3);
        break;
      case KEY_ICON_4: moveJogZ(true); break;
      case KEY_ICON_7: infoMenu.cur--; break;
      default: break;
    }
    loopProcess();
  }
}

static void menuGoToFocus(void)
{
  float currentZ;
  float targetZ;

  if(infoHost.connected == false)
  {
    reminderMessage(LABEL_UNCONNECTED, STATUS_NORMAL);
    return;
  }
  if(parseAckMPosFresh(MPOS_FRESH_TICKS) == false)
  {
    USART1_Puts("?");
    reminderMessage(LABEL_MPOS_NOT_READY, STATUS_NORMAL);
    return;
  }
  if(infoK40Settings.plane0_valid == false)
  {
    reminderMessage(LABEL_PLANE_0_MISSING, STATUS_NORMAL);
    return;
  }
  if(infoK40Settings.material_thickness < 0.1f
    || infoK40Settings.material_thickness > FOCUS_THICKNESS_MAX
    || infoK40Settings.material_thickness != infoK40Settings.material_thickness)
  {
    reminderMessage(LABEL_INVALID_THICKNESS, STATUS_NORMAL);
    return;
  }
  if(parseAckMachineIdle(MPOS_FRESH_TICKS) == false)
  {
    reminderMessage(LABEL_MACHINE_NOT_IDLE, STATUS_NORMAL);
    return;
  }
  if(infoCmd.count != 0 || infoHost.wait)
  {
    reminderMessage(LABEL_BUSY, STATUS_NORMAL);
    return;
  }

  currentZ = coordinateGetAxis(Z_AXIS);
  if(currentZ < -Z_MAX_POS || currentZ > 0.0f
    || infoK40Settings.plane0_z < -Z_MAX_POS
    || infoK40Settings.plane0_z > 0.0f
    || currentZ != currentZ
    || infoK40Settings.plane0_z != infoK40Settings.plane0_z)
  {
    reminderMessage(LABEL_Z_OUT_OF_RANGE, STATUS_NORMAL);
    return;
  }

  targetZ = infoK40Settings.plane0_z - infoK40Settings.material_thickness;
  if(targetZ < -Z_MAX_POS || targetZ > 0.0f)
  {
    reminderMessage(LABEL_Z_OUT_OF_RANGE, STATUS_NORMAL);
    return;
  }

  mustStoreCmd("G21\n");
  mustStoreCmd("G90\n");
  mustStoreCmd("G1 Z%.3f F300\n", targetZ);
  reminderMessage(LABEL_BUSY, STATUS_NORMAL);
}

void menuFocusZ(void)
{
  KEY_VALUES key = KEY_IDLE;
  menuDrawPage(&focusItems);
  while(infoMenu.menu[infoMenu.cur] == menuFocusZ)
  {
    key = menuKeyGetValue();
    switch(key)
    {
      case KEY_ICON_0:
        infoMenu.menu[++infoMenu.cur] = menuFocusPlane0;
        break;
      case KEY_ICON_1:
        infoMenu.menu[++infoMenu.cur] = menuFocusThickness;
        break;
      case KEY_ICON_2:
        infoMenu.menu[++infoMenu.cur] = menuFocusJog;
        break;
      case KEY_ICON_3:
        menuGoToFocus();
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
