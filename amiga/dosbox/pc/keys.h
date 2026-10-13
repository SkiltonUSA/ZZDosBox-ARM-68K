/* GPL-2.0-or-later. Amiga raw key to DOSBox PC key, with independent release. */
#pragma once
#include "keyboard.h"
static inline KBD_KEYS pc_key(unsigned code){
 static const KBD_KEYS row1[]={KBD_grave,KBD_1,KBD_2,KBD_3,KBD_4,KBD_5,KBD_6,KBD_7,KBD_8,KBD_9,KBD_0,KBD_minus,KBD_equals,KBD_backslash};
 static const KBD_KEYS row2[]={KBD_q,KBD_w,KBD_e,KBD_r,KBD_t,KBD_y,KBD_u,KBD_i,KBD_o,KBD_p,KBD_leftbracket,KBD_rightbracket};
 static const KBD_KEYS row3[]={KBD_a,KBD_s,KBD_d,KBD_f,KBD_g,KBD_h,KBD_j,KBD_k,KBD_l,KBD_semicolon,KBD_quote};
 static const KBD_KEYS row4[]={KBD_extra_lt_gt,KBD_z,KBD_x,KBD_c,KBD_v,KBD_b,KBD_n,KBD_m,KBD_comma,KBD_period,KBD_slash};
 code&=127;
 if(code<sizeof(row1)/sizeof(*row1))return row1[code];
 if(code>=0x10&&code<0x1c)return row2[code-0x10];
 if(code>=0x20&&code<0x2b)return row3[code-0x20];
 if(code>=0x30&&code<0x3b)return row4[code-0x30];
 if(code>=0x50&&code<=0x58)return (KBD_KEYS)(KBD_f1+code-0x50);
 switch(code){
 case 0x40:return KBD_space;case 0x41:return KBD_backspace;case 0x42:return KBD_tab;case 0x43:return KBD_kpenter;case 0x44:return KBD_enter;case 0x45:return KBD_esc;case 0x46:return KBD_delete;
 case 0x4c:return KBD_up;case 0x4d:return KBD_down;case 0x4e:return KBD_right;case 0x4f:return KBD_left;
 case 0x60:return KBD_leftshift;case 0x61:return KBD_rightshift;case 0x62:return KBD_capslock;case 0x63:return KBD_leftctrl;case 0x64:return KBD_leftalt;case 0x65:return KBD_rightalt;
 case 0x0f:return KBD_kp0;case 0x1d:return KBD_kp1;case 0x1e:return KBD_kp2;case 0x1f:return KBD_kp3;case 0x2d:return KBD_kp4;case 0x2e:return KBD_kp5;case 0x2f:return KBD_kp6;case 0x3d:return KBD_kp7;case 0x3e:return KBD_kp8;case 0x3f:return KBD_kp9;
 case 0x3c:return KBD_kpperiod;case 0x4a:return KBD_kpminus;case 0x5e:return KBD_kpplus;case 0x5c:return KBD_kpdivide;case 0x5d:return KBD_kpmultiply;
 default:return KBD_NONE;
 }
}
