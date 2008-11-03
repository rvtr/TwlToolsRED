#ifndef	_MIYA_MCU_H_
#define	_MIYA_MCU_H_

#define MIYA_MCU_COMMAND_GET_FREE_REG   1
#define MIYA_MCU_COMMAND_GET_VOLUME     2
#define MIYA_MCU_COMMAND_GET_BRIGHTNESS 3
#define MIYA_MCU_COMMAND_SET_VOLUME     4
#define MIYA_MCU_COMMAND_SET_BRIGHTNESS 5
#define MIYA_MCU_COMMAND_SET_FREE_REG   6


#ifdef __cplusplus
extern "C" {
#endif

void MIYA_MCU_Init(void);
u8 MCU_GetFreeReg( void );
u8 MCU_GetFreeRegister( void );
BOOL MCU_SetFreeRegister( u8 value );
BOOL MCU_SetBackLightBrightness( u8 brightness );
BOOL MCU_SetVolume( u8 vol );
u8 MCU_GetBackLightBrightness( void );
u8 MCU_GetVolume( void );


#ifdef __cplusplus
}
#endif


#endif /* _MIYA_MCU_H_ */
