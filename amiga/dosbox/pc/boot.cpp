/* GPL-2.0-or-later. BIOS floppy cache and exclusive, bounded host HDD I/O. */
#include "dosbox.h"
#include "cpu.h"
#include "bios_disk.h"
#include "callback.h"
#include <string.h>
#ifndef DB_NATIVE
#include "pc_wire.h"
extern "C" volatile uint8_t *db_shared();
#endif
char db_boot_name[13],db_hdd_name[13];
extern bool db_disk_open(const char*);
extern bool db_disk_sector(unsigned,void*,bool);
static uint8_t floppy[1440*1024];
static bool loaded;
extern bool db_load_boot(const char*,uint8_t*,unsigned);
Bit8u db_boot_sector(Bit32u sector,void *data){
 if(!loaded||sector>=2880)return 4;
 memcpy(data,floppy+sector*512,512);return 0;
}
Bit8u db_image_sector(bool hard,Bit32u sector,void *data,bool writing){
 if(hard)return db_disk_sector(sector,data,writing)?0:4;
 return writing?3:db_boot_sector(sector,data);
}
void db_boot_start(){
 if(*db_boot_name){
 if(!db_load_boot(db_boot_name,floppy,sizeof(floppy)))E_Exit("Boot image must be a readable 1.44 MiB file");
 if(floppy[510]!=0x55||floppy[511]!=0xaa)E_Exit("Boot sector signature missing");
 loaded=true;imageDiskList[0]=new imageDisk(nullptr,db_boot_name,1440,false);
 }
 uint8_t mbr[512];
 if(*db_hdd_name){
  if(!db_disk_open(db_hdd_name))E_Exit("HDD needs an unmounted, writable 32 MiB image");
  imageDiskList[2]=new imageDisk(nullptr,db_hdd_name,32768,true);
  imageDiskList[2]->Set_Geometry(16,128,32,512);updateDPT();mem_writeb(BIOS_HARDDISK_COUNT,1);
 }
 if(!*db_boot_name){
  if(!*db_hdd_name||!db_disk_sector(0,mbr,false)||mbr[510]!=0x55||mbr[511]!=0xaa)E_Exit("HDD boot sector missing");
 }
 loaded=true;MEM_BlockWrite(0x7c00,*db_boot_name?floppy:mbr,512);
 real_writed(0,4,0xf000ff53);real_writed(0,12,0xf000ff53);
 SegSet16(cs,0);reg_eip=0x7c00;SegSet16(ds,0);SegSet16(es,0);
 SegSet16(ss,0x7000);reg_esp=0x100;reg_esi=0;reg_ecx=1;
 reg_ebp=reg_eax=reg_edx=0;reg_ebx=0x7c00;reg_dl=*db_boot_name?0:0x80;
}
void db_boot_close(){
#ifndef DB_NATIVE
 if(loaded){volatile uint8_t *p=db_shared()+PC_BOOT_TEXT;
  for(unsigned i=0;i<2000;i++){unsigned c=mem_readb((machine==MCH_HERC?0xb0000:0xb8000)+i*2);p[i]=c>=32&&c<127?c:32;}
  __asm__ volatile("dsb sy":::"memory");ad_put(db_shared(),PC_BOOT_TEXT+2000,0x424f4f54);
 }
#endif
 for(unsigned i=0;i<MAX_DISK_IMAGES;i++){delete imageDiskList[i];imageDiskList[i]=nullptr;}
 loaded=false;
}
