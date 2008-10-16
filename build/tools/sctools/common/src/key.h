#ifndef	_KEY_H_
#define	_KEY_H_

#ifdef __cplusplus
extern "C" {
#endif


u16 m_get_key_trigger(void);
u16 m_get_key_code(void);

/*
  u16 keyData;
  keyData = m_get_key_code(void);
  if ( keyData & PAD_KEY_DOWN ) {
  } if ( keyData & PAD_KEY_UP ) {
  } if ( keyData & PAD_KEY_LEFT ) {
  } if ( keyData & PAD_KEY_RIGHT ) {
  } if ( keyData & PAD_BUTTON_R ) {
  } if ( keyData & PAD_BUTTON_L ) {
  } if( keyData & PAD_BUTTON_A ) {
  } if( keyData & PAD_BUTTON_B ) {
  } if( keyData & PAD_BUTTON_SELECT ) {
  } if( keyData & PAD_BUTTON_START ) {
  }

*/

#ifdef __cplusplus
}
#endif


#endif /* _KEY_H_ */
