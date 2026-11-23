/* GPL-2.0-or-later. Initial paged JIT subset: register-only integer operations.
 * No memory operands, stack operations, branches, mode changes or x87 helpers.
 * The release baseline uses one instruction. The experimental group counter
 * accepts only complete instructions inside caller-validated page/segment RAM. */
#pragma once
#include <stdint.h>
static inline unsigned pc_paged_register_length(unsigned opcode,unsigned modrm,int big){
 unsigned word=big?4:2;
 if((opcode>=0x40&&opcode<=0x4f)||(opcode>=0x90&&opcode<=0x97))return 1;
 if(opcode>=0xb0&&opcode<=0xb7)return 2;
 if(opcode>=0xb8&&opcode<=0xbf)return 1+word;
 if(opcode<=0x3d&&(opcode&7)==4)return 2;
 if(opcode<=0x3d&&(opcode&7)==5)return 1+word;
 if((modrm&0xc0)!=0xc0)return 0;
 if((opcode<=0x3b&&(opcode&7)<=3)||(opcode>=0x88&&opcode<=0x8b)||opcode==0x84||opcode==0x85||opcode==0x86||opcode==0x87)return 2;
 if(opcode==0x80||opcode==0x83)return 3;
 if(opcode==0x81)return 2+word;
 if(opcode==0xc0||opcode==0xc1)return 3;
 if(opcode>=0xd0&&opcode<=0xd3)return 2;
 return 0;
}

/* The caller validates the page permissions and supplies only contiguous RAM
 * within the current code page/segment. Never inspect a following page, even
 * to classify an instruction which would subsequently fall back. */
static inline unsigned pc_paged_register_count(const uint8_t *code,
 unsigned available,int big,unsigned maximum){
 unsigned count=0,offset=0;
 while(count<maximum&&offset<available){
  unsigned remaining=available-offset;
  unsigned length=pc_paged_register_length(code[offset],
      remaining>1?code[offset+1]:0,big);
  if(!length||length>remaining)break;
  offset+=length;count++;
 }
 return count;
}
