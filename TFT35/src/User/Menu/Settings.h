#ifndef _SETTINGS_H_
#define _SETTINGS_H_

#include "stdint.h"

typedef struct
{
  uint32_t baudrate;
  uint8_t  language;
}SETTINGS;

typedef struct
{
  float plane0_z;
  float material_thickness;
  uint8_t plane0_valid;
}K40_SETTINGS;

extern SETTINGS infoSettings;
extern K40_SETTINGS infoK40Settings;

void menuInfo(void);
void menuSettings(void);

#endif
