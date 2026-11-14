#include "Move.h"
#include "includes.h"

/* Shared incremental jog distance for X, Y and Z. */
#define ITEM_MOVE_LEN_NUM 3
static const ITEM itemMoveLen[ITEM_MOVE_LEN_NUM] = {
  {ICON_001_MM, LABEL_001_MM},  /* 0.01 mm */
  {ICON_01_MM,  LABEL_01_MM},   /* 0.1 mm  */
  {ICON_1_MM,   LABEL_1_MM}     /* 1.0 mm  */
};
static const float item_move_len[ITEM_MOVE_LEN_NUM] = {0.01f, 0.1f, 1.0f};
static u8 item_move_len_i = 1;

static MENUITEMS moveItems = {
  LABEL_MOVE,
  {{ICON_X_DEC, LABEL_X_DEC},
   {ICON_Y_INC, LABEL_Y_INC},
   {ICON_Z_INC, LABEL_Z_INC},
   {ICON_01_MM, LABEL_01_MM},
   {ICON_X_INC, LABEL_X_INC},
   {ICON_Y_DEC, LABEL_Y_DEC},
   {ICON_Z_DEC, LABEL_Z_DEC},
   {ICON_BACK,  LABEL_BACK}}
};

static void jogAxis(char axis, bool positive)
{
  const float distance = item_move_len[item_move_len_i];
  /* Keep the Z jog deliberately slower than XY and below the configured
   * FluidNC Z maximum (600 mm/min). FluidNC still enforces its own limits. */
  const int feed = (axis == 'Z') ? 300 : DEFAULT_SPEED_MOVE;

  /* Force millimetres and bracket the incremental move. This prevents a
   * previous G20/G91 mode from changing the requested jog distance. */
  mustStoreCmd("G21\n");
  mustStoreCmd("G91\n");
  if (positive)
    mustStoreCmd("G1 %c%.2f F%d\n", axis, distance, feed);
  else
    mustStoreCmd("G1 %c-%.2f F%d\n", axis, distance, feed);
  mustStoreCmd("G90\n");
}

void moveJogZ(bool positive)
{
  jogAxis('Z', positive);
}

u8 moveGetJogStepIndex(void)
{
  return item_move_len_i;
}

void moveCycleJogStep(void)
{
  item_move_len_i = (item_move_len_i + 1) % ITEM_MOVE_LEN_NUM;
}

void menuMove(void)
{
  KEY_VALUES key_num = KEY_IDLE;

  menuDrawPage(&moveItems);
  while (infoMenu.menu[infoMenu.cur] == menuMove)
  {
    key_num = menuKeyGetValue();
    switch (key_num)
    {
      case KEY_ICON_0: jogAxis('X', false); break; /* X- */
      case KEY_ICON_1: jogAxis('Y', true);  break; /* Y+ */
      case KEY_ICON_2: jogAxis('Z', true);  break; /* Z+ */

      case KEY_ICON_3:
        moveCycleJogStep();
        moveItems.items[KEY_ICON_3] = itemMoveLen[item_move_len_i];
        menuDrawItem(&moveItems.items[KEY_ICON_3], KEY_ICON_3);
        break;

      case KEY_ICON_4: jogAxis('X', true);  break; /* X+ */
      case KEY_ICON_5: jogAxis('Y', false); break; /* Y- */
      case KEY_ICON_6: jogAxis('Z', false); break; /* Z- */
      case KEY_ICON_7: infoMenu.cur--;      break;
      default: break;
    }
    loopProcess();
  }
}
