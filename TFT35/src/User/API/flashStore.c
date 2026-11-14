#include "flashStore.h"
#include "Language.h"
#include <string.h>

#define PARA_SIGN 0x20190320
#define SIGN_ADDRESS (0x08040000 - 0x800)  //�������һҳ(2KB),���������û�����

extern u32 TSC_Para[7];        //������У׼ϵ��
#define TSC_ADDRESS (0x08040000 - 0x800 + 4)

extern SETTINGS infoSettings;  //�û�����
#define SETTING_ADDRESS (0x08040000 - 0x800 + 32) 
#define FOCUS_DATA_ADDRESS (SETTING_ADDRESS + 8)
#define FOCUS_DATA_MAGIC 0x4B343046
#define FOCUS_DATA_VERSION 1
#define FOCUS_DATA_MIN_Z (-150.0f)
#define FOCUS_DATA_MAX_Z 0.0f
#define FOCUS_DATA_MAX_THICKNESS 150.0f

extern K40_SETTINGS infoK40Settings;

static u32 floatToWord(float value)
{
  u32 word;
  memcpy(&word, &value, sizeof(word));
  return word;
}

static float wordToFloat(u32 word)
{
  float value;
  memcpy(&value, &word, sizeof(value));
  return value;
}

static bool isValidFocusData(float plane0, float thickness, u32 plane0Valid)
{
  if(plane0Valid > 1 || plane0 != plane0 || thickness != thickness)
    return false;
  if(plane0 < FOCUS_DATA_MIN_Z || plane0 > FOCUS_DATA_MAX_Z)
    return false;
  if(thickness < 0.0f || thickness > FOCUS_DATA_MAX_THICKNESS)
    return false;
  return true;
}

bool readStoredPara(void)
{
  if(*((vu32*)SIGN_ADDRESS) != PARA_SIGN)   return false;
    
  for(int i=0; i<sizeof(TSC_Para)/sizeof(TSC_Para[0]); i++)
  {
    TSC_Para[i] = *((vu32*)(TSC_ADDRESS + i*4));
  }
  infoSettings.baudrate = *((vu32*) SETTING_ADDRESS);
  infoSettings.language = *((vu32*)(SETTING_ADDRESS + 4));
  if(infoSettings.language != ITALIAN && infoSettings.language != ENGLISH)
    infoSettings.language = ITALIAN;
  infoSettings.baudrate = 115200;

  infoK40Settings.plane0_z = 0.0f;
  infoK40Settings.material_thickness = 0.0f;
  infoK40Settings.plane0_valid = false;
  if(*((vu32*)FOCUS_DATA_ADDRESS) == FOCUS_DATA_MAGIC
    && *((vu32*)(FOCUS_DATA_ADDRESS + 4)) == FOCUS_DATA_VERSION)
  {
    float plane0 = wordToFloat(*((vu32*)(FOCUS_DATA_ADDRESS + 8)));
    float thickness = wordToFloat(*((vu32*)(FOCUS_DATA_ADDRESS + 12)));
    u32 plane0Valid = *((vu32*)(FOCUS_DATA_ADDRESS + 16));
    if(isValidFocusData(plane0, thickness, plane0Valid))
    {
      infoK40Settings.plane0_z = plane0;
      infoK40Settings.material_thickness = thickness;
      infoK40Settings.plane0_valid = (uint8_t)plane0Valid;
    }
  }
  return true;
}

bool storePara(void)
{
  bool success = true;

  FLASH_Unlock();

  if(FLASH_ErasePage(SIGN_ADDRESS) != FLASH_COMPLETE)
  {
    success = false;
  }

  if(success)
  {
    for(int i=0; i<sizeof(TSC_Para)/sizeof(TSC_Para[0]); i++)
    {
      if(FLASH_ProgramWord((TSC_ADDRESS + i*4), TSC_Para[i]) != FLASH_COMPLETE)
      {
        success = false;
        break;
      }
    }

    if(success && FLASH_ProgramWord(SETTING_ADDRESS, infoSettings.baudrate) != FLASH_COMPLETE)
      success = false;
    if(success && FLASH_ProgramWord(SETTING_ADDRESS + 4, infoSettings.language) != FLASH_COMPLETE)
      success = false;
    if(success && FLASH_ProgramWord(FOCUS_DATA_ADDRESS + 4, FOCUS_DATA_VERSION) != FLASH_COMPLETE)
      success = false;
    if(success && FLASH_ProgramWord(FOCUS_DATA_ADDRESS + 8,
                         floatToWord(infoK40Settings.plane0_z)) != FLASH_COMPLETE)
      success = false;
    if(success && FLASH_ProgramWord(FOCUS_DATA_ADDRESS + 12,
                         floatToWord(infoK40Settings.material_thickness)) != FLASH_COMPLETE)
      success = false;
    if(success && FLASH_ProgramWord(FOCUS_DATA_ADDRESS + 16,
                         infoK40Settings.plane0_valid ? 1 : 0) != FLASH_COMPLETE)
      success = false;
    if(success && FLASH_ProgramWord(FOCUS_DATA_ADDRESS, FOCUS_DATA_MAGIC) != FLASH_COMPLETE)
      success = false;
    if(success && FLASH_ProgramWord(SIGN_ADDRESS, PARA_SIGN) != FLASH_COMPLETE)
      success = false;
  }

  FLASH_Lock();
  return success;
}
