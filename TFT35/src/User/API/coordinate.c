#include "coordinate.h"

const char axis_id[TOTAL_AXIS]={'X','Y','Z'};

COORDINATE coordinate={{0.0f,0.0f,0.0f}};

static bool relative_mode = false;

bool coorGetRelative(void)
{
  return relative_mode;
}

void coorSetRelative(bool mode)
{
  relative_mode = mode;
}

void coordinateSetAxis(AXIS axis,float position)
{
  if(relative_mode == false)
  {
    coordinate.axis[axis] = position;
  }
  else
  {
    coordinate.axis[axis] += position;
  }
}

void coordinateSetMPos(float x, float y, float z)
{
  coordinate.axis[X_AXIS] = x;
  coordinate.axis[Y_AXIS] = y;
  coordinate.axis[Z_AXIS] = z;
}

float coordinateGetAxis(AXIS axis)
{
  return coordinate.axis[axis];
}
