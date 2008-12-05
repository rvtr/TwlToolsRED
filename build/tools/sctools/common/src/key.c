#include <twl.h>
#include "key.h"

static u16 old_keydata = 0;
#define REPEAT_ON 1
#define REPEAT_COUNT 20
#define ONLY_PLUS_KEY 1
static int repeat_counter[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

#ifdef ONLY_PLUS_KEY
static u16 plus_key[4] = {
  PAD_KEY_DOWN,
  PAD_KEY_UP,
  PAD_KEY_LEFT,
  PAD_KEY_RIGHT
};
#endif

u16 m_get_key_trigger(void)
{
  u16 keydata;
  u16 trigger;
  int i;
#ifndef ONLY_PLUS_KEY
  u16 r;
#endif

  keydata = (u16)PAD_Read();
  trigger = (u16)(keydata & (keydata ^ old_keydata));

#ifdef ONLY_PLUS_KEY
#if REPEAT_ON
  /*  r = 1; */
  /* 十字ボタンだけリピートする。 */
  for( i = 0 ; i < 4 ; i++ ) {
    if( plus_key[i] & old_keydata & keydata ) {
      repeat_counter[i]++;
      if( repeat_counter[i] > REPEAT_COUNT ) {
	trigger |= (plus_key[i]);
      }
    }
    else {
      repeat_counter[i] = 0;
    }
  }
#endif  


#else
#if REPEAT_ON
  r = 1;
  for( i = 0 ; i < 15 ; i++ ) {
    if( r & old_keydata & keydata ) {
      repeat_counter[i]++;
      if( repeat_counter[i] > REPEAT_COUNT ) {
	trigger |= r;
      }
    }
    else {
      repeat_counter[i] = 0;
    }
    r <<= 1;
  }
#endif  
#endif  

  old_keydata = keydata;
  return trigger;
}

u16 m_get_key_code(void)
{
  return old_keydata;
}
