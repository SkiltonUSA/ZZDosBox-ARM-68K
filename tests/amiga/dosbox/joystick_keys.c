#include <assert.h>
#include <stdio.h>
#include "joystick_plan.h"
#include "key_sources.h"
static unsigned events[32],n,blocked;
static int emit(unsigned code,void*ctx){(void)ctx;if(blocked)return 0;events[n++]=code;return 1;}
int main(void){unsigned keys[4]={1,2,5,6};unsigned char desired[128];struct db_key_sources s={0};
 for(unsigned count=1;count<=6;count++){
  assert(db_joy_config_valid(count,keys));assert(db_joy_mask(63,count)==(1u<<count)-1);
  db_joy_key_state(desired,63,count,keys);
  for(unsigned k=0;k<4;k++)assert(desired[db_joy_raw_keys[keys[k]]]==(count>k+2));
 }
 assert(!db_joy_config_valid(0,keys)&&!db_joy_config_valid(7,keys));keys[3]=99;assert(!db_joy_config_valid(6,keys));keys[3]=6;
 /* A joystick key-up cannot release the same key held on the keyboard. */
 assert(db_key_source_set(&s,0x40,1,0,emit,0));assert(n==1&&events[0]==0x40);
 assert(db_key_source_set(&s,0x40,1,1,emit,0));assert(n==1);
 assert(db_key_source_set(&s,0x40,0,1,emit,0));assert(n==1);
 assert(db_key_source_set(&s,0x40,0,0,emit,0));assert(n==2&&events[1]==0xc0);
 /* And the reverse order, including retry on a full ring. */
 assert(db_key_source_set(&s,0x40,1,1,emit,0));assert(n==3);
 assert(db_key_source_set(&s,0x40,1,0,emit,0));assert(n==3);
 assert(db_key_source_set(&s,0x40,0,0,emit,0));assert(n==3);
 blocked=1;assert(!db_key_source_set(&s,0x40,0,1,emit,0));assert(s.joystick[0x40]&&n==3);
 blocked=0;assert(db_key_source_set(&s,0x40,0,1,emit,0));assert(n==4&&events[3]==0xc0);
 /* Duplicate bindings remain held until both physical buttons release. */
 keys[1]=keys[0];db_joy_key_state(desired,4|8,6,keys);assert(desired[0x40]);
 db_joy_key_state(desired,8,6,keys);assert(desired[0x40]);
 db_joy_key_state(desired,0,6,keys);assert(!desired[0x40]);
 keys[0]=0;db_joy_key_state(desired,4,6,keys);for(unsigned i=0;i<128;i++)assert(!desired[i]);
 puts("PASS joystick: counts 1-6, disabled/duplicate bindings, keyboard ownership and ring retry");
}
