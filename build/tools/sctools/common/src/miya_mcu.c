#include <twl.h>
#include        "miya_mcu.h"



static OSMessage MyMesgBuffer[1];
static OSMessageQueue MyMesgQueue;


static volatile u8 miya_mcu_free_register = 0x44;
static volatile u32 my_mcu_command = 0;
static volatile u8 my_mcu_volume = 0;
static volatile u8 my_mcu_brightness = 0;


static void miya_mcu_free_reg_pxi_callback(PXIFifoTag tag, u32 data, BOOL err)
{
#pragma unused(tag)
#pragma unused(err)
  switch( my_mcu_command ) {

  case MIYA_MCU_COMMAND_GET_FREE_REG:
    miya_mcu_free_register = (u8)(0xff & data);
    break;
  case MIYA_MCU_COMMAND_GET_VOLUME:
    my_mcu_volume = (u8)(0xff & data);
    break;
  case MIYA_MCU_COMMAND_GET_BRIGHTNESS:
    my_mcu_brightness = (u8)(0xff & data);
    break;
  case MIYA_MCU_COMMAND_SET_VOLUME:
    break;
     
  case MIYA_MCU_COMMAND_SET_BRIGHTNESS:
    break;
    
  default:
    miya_mcu_free_register = (u8)(0xff & data);
    break;
  }    
  (void)OS_SendMessage(&MyMesgQueue, (OSMessage)0, OS_MESSAGE_NOBLOCK);
}

void MIYA_MCU_Init(void)
{
  OS_InitMessageQueue(&MyMesgQueue, &MyMesgBuffer[0], 1);
  PXI_SetFifoRecvCallback(PXI_FIFO_TAG_USER_0, miya_mcu_free_reg_pxi_callback);

}

static void miya_mcu_send_pxi_data(u32 data)
{
  my_mcu_command = 0x0f & data;
  while (PXI_SendWordByFifo(PXI_FIFO_TAG_USER_0, data, FALSE) != PXI_FIFO_SUCCESS)
    {
      // do nothing
    }
}


static void miya_mcu_get_free_reg(void)
{
  miya_mcu_send_pxi_data(MIYA_MCU_COMMAND_GET_FREE_REG);
}

static void miya_mcu_get_volume(void)
{
  miya_mcu_send_pxi_data(MIYA_MCU_COMMAND_GET_VOLUME);
}

static void miya_mcu_set_volume(u8 vol)
{
  u32 data;
  data = MIYA_MCU_COMMAND_SET_VOLUME;
  data |= ((u32)vol << 4);
  miya_mcu_send_pxi_data(data);
}

static void miya_mcu_get_brightness(void)
{
  miya_mcu_send_pxi_data(MIYA_MCU_COMMAND_GET_BRIGHTNESS);
}

static void miya_mcu_set_brightness(u8 brightness)
{
  u32 data;
  data = MIYA_MCU_COMMAND_SET_BRIGHTNESS;
  data |= ((u32)brightness << 4);
  miya_mcu_send_pxi_data(data);
}



u8 MCU_GetFreeReg( void )
{
  OSMessage message;
  miya_mcu_get_free_reg();
  if( TRUE == OS_ReceiveMessage(&MyMesgQueue, &message, OS_MESSAGE_BLOCK) ) {
  }
  return miya_mcu_free_register;
}


u8 MCU_GetVolume( void )
{
  OSMessage message;
  miya_mcu_get_volume();
  if( TRUE == OS_ReceiveMessage(&MyMesgQueue, &message, OS_MESSAGE_BLOCK) ) {
  }
 return my_mcu_volume;
}

u8 MCU_GetBackLightBrightness( void )
{
  OSMessage message;

  miya_mcu_get_brightness();
  if( TRUE == OS_ReceiveMessage(&MyMesgQueue, &message, OS_MESSAGE_BLOCK) ) {
  }
 return my_mcu_brightness;
}


BOOL MCU_SetBackLightBrightness( u8 brightness )
{
  OSMessage message;
  miya_mcu_set_brightness(brightness);
  if( TRUE == OS_ReceiveMessage(&MyMesgQueue, &message, OS_MESSAGE_BLOCK) ) {
  }
  return TRUE;
}


BOOL MCU_SetVolume( u8 vol )
{
  OSMessage message;
  miya_mcu_set_volume( vol );
  if( TRUE == OS_ReceiveMessage(&MyMesgQueue, &message, OS_MESSAGE_BLOCK) ) {
  }
  return TRUE;
}



