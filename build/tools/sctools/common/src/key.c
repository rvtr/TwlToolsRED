#include <twl.h>
#include "key.h"

static u16 old_keydata = 0;
#define REPEAT_ON 1
#define REPEAT_COUNT 20
static int repeat_counter[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

u16 m_get_key_trigger(void)
{
  u16 keydata;
  u16 trigger;
  int i;
  u16 r;

  keydata = (u16)PAD_Read();
  trigger = (u16)(keydata & (keydata ^ old_keydata));

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

  old_keydata = keydata;
  return trigger;
}

u16 m_get_key_code(void)
{
  return old_keydata;
}
