#include "myfatfs.h"
#include "ff.h"

static FATFS updateFileSystem;

bool mountSDCard(void)
{
  return f_mount(&updateFileSystem, "SD:", 1) == FR_OK;
}
