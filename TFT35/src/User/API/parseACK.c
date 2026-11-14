#include "parseACK.h"

char ack_rev_buf[ACK_MAX_SIZE];
static u16 ack_index=0;
static u32 mposUpdateTime = 0;
static bool mposValid = false;
static u32 machineStatusUpdateTime = 0;
static bool machineStatusValid = false;
static bool machineIdle = false;

bool parseAckMPosFresh(u32 maxAgeTicks)
{
  return mposValid && (OS_GetTime() - mposUpdateTime) <= maxAgeTicks;
}

bool parseAckMachineIdle(u32 maxAgeTicks)
{
  return machineStatusValid && machineIdle
      && (OS_GetTime() - machineStatusUpdateTime) <= maxAgeTicks;
}

void parseAckInvalidateMPos(void)
{
  mposValid = false;
  machineStatusValid = false;
}

static char ack_seen(const char *str)
{
  u16 i;	
  for(ack_index=0; ack_index<ACK_MAX_SIZE && ack_rev_buf[ack_index]!=0; ack_index++)
  {
    for(i=0; str[i]!=0 && ack_rev_buf[ack_index+i]!=0 && ack_rev_buf[ack_index+i]==str[i]; i++)
    {}
    if(str[i]==0)
    {
      ack_index += i;      
      return true;
    }
  }
  return false;
}

static char ack_cmp(const char *str)
{
  u16 i;
  for(i=0; i<ACK_MAX_SIZE && str[i]!=0 && ack_rev_buf[i]!=0; i++)
  {
    if(str[i] != ack_rev_buf[i])
    return false;
  }
  if(ack_rev_buf[i] != 0) return false;
  return true;
}

void ackPopupInfo(const char *info)
{
    popupDrawPage(&bottomSingleBtn , (u8*)info, (u8 *)ack_rev_buf + ack_index, textSelect(LABEL_CONFIRM), NULL);    
    if(infoMenu.menu[infoMenu.cur] != menuPopup)
      infoMenu.menu[++infoMenu.cur] = menuPopup;
}

void parseACK(void)
{
  if(infoHost.rx_ok != true) return;      // Nessun dato ricevuto

  // 1. Connessione automatica con FluidNC (accetta ok, Grbl, FluidNC o <status>)
  if(infoHost.connected == false)
  {
    if(ack_seen("ok") || ack_seen("Grbl") || ack_seen("FluidNC") || ack_rev_buf[0] == '<')
    {
      infoHost.connected = true;
    }
    else
    {
      goto parse_end;
    }
  }

  // 2. Parsing Status Report FluidNC: <Idle|MPos:0.000,0.000,0.000|FS:0,0>
  if(ack_rev_buf[0] == '<')
  {
    char *state = ack_rev_buf + 1;
    char *stateEnd = strpbrk(state, "|>");
    machineStatusValid = stateEnd != NULL;
    machineIdle = machineStatusValid
               && (stateEnd - state == 4)
               && strncmp(state, "Idle", 4) == 0;
    machineStatusUpdateTime = OS_GetTime();

    char *mpos = strstr(ack_rev_buf, "MPos:");
    if(mpos != NULL)
    {
      float x, y, z;
      if(sscanf(mpos + 5, "%f,%f,%f", &x, &y, &z) == 3
        && x == x && y == y && z == z
        && x <= 100000.0f && x >= -100000.0f
        && y <= 100000.0f && y >= -100000.0f
        && z <= 100000.0f && z >= -100000.0f)
      {
        coordinateSetMPos(x, y, z);
        mposUpdateTime = OS_GetTime();
        mposValid = true;
      }
    }
    goto parse_end;
  }

  // 3. Sblocco coda comandi su "ok"
  if(ack_cmp("ok\r\n") || ack_cmp("ok\n") || ack_seen("ok"))
  {
    infoHost.wait = false;	
  }
  // 4. Segnalazione errori o allarmi FluidNC a schermo
  else if(ack_seen("error:"))
  {
    infoHost.wait = false;
    ackPopupInfo("FluidNC Error");
  }
  else if(ack_seen("ALARM:"))
  {
    infoHost.wait = false;
    ackPopupInfo("FluidNC ALARM");
  }

parse_end:
  infoHost.rx_ok = false;
  USART1_DMAReEnable();
}
