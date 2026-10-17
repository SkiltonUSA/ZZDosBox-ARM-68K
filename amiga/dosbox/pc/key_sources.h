/* GPL-3.0-or-later. Merge keyboard and gamepad holds without premature key-up. */
#pragma once
struct db_key_sources {unsigned char physical[128],joystick[128];};
static int db_key_source_set(struct db_key_sources*s,unsigned code,int down,int joy,int(*emit)(unsigned,void*),void*ctx){
 unsigned char*own=joy?s->joystick:s->physical;unsigned char*other=joy?s->physical:s->joystick;
 unsigned old=own[code]||other[code],next=down||other[code];
 if((old!=next||(!joy&&down&&own[code]))&&!emit(code|(next?0:128),ctx))return 0;
 own[code]=down!=0;return 1;
}
