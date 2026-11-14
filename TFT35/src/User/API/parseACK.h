#ifndef _PARSEACK_H_
#define _PARSEACK_H_

#include "includes.h"

static const char errormagic[]        = "Error:";
static const char echomagic[]         = "echo:";
static const char busymagic[]         = "busy:";

#define ACK_MAX_SIZE 300
extern char ack_rev_buf[ACK_MAX_SIZE];

void parseACK(void);
bool parseAckMPosFresh(u32 maxAgeTicks);
bool parseAckMachineIdle(u32 maxAgeTicks);
void parseAckInvalidateMPos(void);

#endif
