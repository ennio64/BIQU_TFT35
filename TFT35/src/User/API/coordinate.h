#ifndef _COORDINATE_H_
#define _COORDINATE_H_

#include "stm32f10x.h"
#include "stdbool.h"


typedef enum
{
  X_AXIS=0,
  Y_AXIS,
  Z_AXIS,
  TOTAL_AXIS
}AXIS;


typedef struct
{
  float axis[TOTAL_AXIS];
}COORDINATE;

extern const char axis_id[TOTAL_AXIS];

bool  coorGetRelative(void);
void  coorSetRelative(bool mode);
float coordinateGetAxis(AXIS axis);
void  coordinateSetAxis(AXIS axis, float position);
void  coordinateSetMPos(float x, float y, float z);

#endif
