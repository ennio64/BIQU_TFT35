#ifndef _MOVE_H_
#define _MOVE_H_

#include "stdbool.h"
#include "stdint.h"

void menuMove(void);
void moveJogZ(bool positive);
uint8_t moveGetJogStepIndex(void);
void moveCycleJogStep(void);

#endif
