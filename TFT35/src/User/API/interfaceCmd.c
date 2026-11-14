#include "interfaceCmd.h"
#include "includes.h"

QUEUE infoCmd;

static u8 cmd_index = 0;

static bool cmd_seen(char code)
{
  for(cmd_index = 0; cmd_index < CMD_MAX_CHAR
      && infoCmd.queue[infoCmd.index_r][cmd_index] != 0; cmd_index++)
  {
    if(infoCmd.queue[infoCmd.index_r][cmd_index] == code)
    {
      cmd_index++;
      return true;
    }
  }
  return false;
}

static float cmd_float(void)
{
  return (float)strtod(&infoCmd.queue[infoCmd.index_r][cmd_index], NULL);
}

bool storeCmd(const char *format, ...)
{
  QUEUE *queue = &infoCmd;
  if(queue->count >= CMD_MAX_LIST)
  {
    reminderMessage(LABEL_BUSY, STATUS_BUSY);
    return false;
  }

  my_va_list args;
  my_va_start(args, format);
  my_vsprintf(queue->queue[queue->index_w], format, args);
  my_va_end(args);

  queue->index_w = (queue->index_w + 1) % CMD_MAX_LIST;
  queue->count++;
  return true;
}

void mustStoreCmd(const char *format, ...)
{
  QUEUE *queue = &infoCmd;
  if(queue->count >= CMD_MAX_LIST)
    reminderMessage(LABEL_BUSY, STATUS_BUSY);

  while(queue->count >= CMD_MAX_LIST)
    loopProcess();

  my_va_list args;
  my_va_start(args, format);
  my_vsprintf(queue->queue[queue->index_w], format, args);
  my_va_end(args);

  queue->index_w = (queue->index_w + 1) % CMD_MAX_LIST;
  queue->count++;
}

void clearCmdQueue(void)
{
  infoCmd.count = 0;
  infoCmd.index_w = 0;
  infoCmd.index_r = 0;
}

void sendQueueCmd(void)
{
  if(infoHost.wait || infoCmd.count == 0)
    return;

  const char *line = infoCmd.queue[infoCmd.index_r];
  if(line[0] == 'G')
  {
    u16 command = (u16)strtol(&line[1], NULL, 10);
    switch(command)
    {
      case 0:
      case 1:
      {
        for(AXIS axis = X_AXIS; axis < TOTAL_AXIS; axis++)
        {
          if(cmd_seen(axis_id[axis]))
            coordinateSetAxis(axis, cmd_float());
        }
        break;
      }

      case 28:
        break;

      case 90:
        coorSetRelative(false);
        break;

      case 91:
        coorSetRelative(true);
        break;

      case 92:
        for(AXIS axis = X_AXIS; axis < TOTAL_AXIS; axis++)
        {
          if(cmd_seen(axis_id[axis]))
            coordinateSetAxis(axis, cmd_float());
        }
        break;

      default:
        break;
    }
  }

  USART1_Puts(infoCmd.queue[infoCmd.index_r]);
  infoCmd.count--;
  infoCmd.index_r = (infoCmd.index_r + 1) % CMD_MAX_LIST;
  infoHost.wait = infoHost.connected;
}
