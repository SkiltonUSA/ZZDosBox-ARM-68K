/* GPL-2.0-or-later. Initial paged JIT subset: register-only integer operations.
 * No memory operands, stack operations, branches, mode changes or x87 helpers.
 * Restricting a block to one instruction avoids speculative translation across
 * a faulting fetch and makes fallback retain the interpreter's restart rules. */
#pragma once
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
