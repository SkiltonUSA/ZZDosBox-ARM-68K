/* GPL-3.0-or-later. Physical button count and optional DOS key bindings. */
#pragma once
static const unsigned char db_joy_raw_keys[]={255,0x40,0x44,0x63,0x64,0x31,0x32,0x33,0x34,0x60,0x42,0x45};
#define DB_JOY_KEYS (sizeof(db_joy_raw_keys)/sizeof(db_joy_raw_keys[0]))
static inline int db_joy_config_valid(unsigned count,const unsigned keys[4]){
 unsigned i;if(count<1||count>6)return 0;for(i=0;i<4;i++)if(keys[i]>=DB_JOY_KEYS)return 0;return 1;
}
static inline unsigned db_joy_mask(unsigned bits,unsigned count){return bits&((1u<<count)-1u);}
static inline void db_joy_key_state(unsigned char out[128],unsigned buttons,unsigned count,const unsigned keys[4]){
 unsigned i;for(i=0;i<128;i++)out[i]=0;
 for(i=2;i<count;i++)if((buttons&(1u<<i))&&keys[i-2])out[db_joy_raw_keys[keys[i-2]]]=1;
}
