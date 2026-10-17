/* Exercise the actual 68k file-service C source with a native DOS API facade. */
#include "fs_mock/platform.h"
#include "fs_host.h"
#include "fs_wire.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <errno.h>
#include "fs_mock/implementation.h"
static _Alignas(64) uint8_t memory[0x190000];static unsigned sequence;
static unsigned metadata_sync;
static void sync_range(volatile void*p,size_t n,void*u){(void)u;if(p==memory+FS_NAME||p==memory+FS_DIR)metadata_sync+=(unsigned)n;}
static void barrier(void*u){(void)u;}
static unsigned call(unsigned op,unsigned h,unsigned n,unsigned pos,unsigned whence,const char *name,const char *other){
 memset(memory+FS_NAME,0,32);if(name)memcpy(memory+FS_NAME,name,strlen(name)<13?strlen(name):13);if(other)memcpy(memory+FS_NAME2,other,strlen(other)<13?strlen(other):13);
 unsigned v[]={++sequence,op,h,n,pos,whence};for(unsigned i=0;i<6;i++)ad_put(memory,FS_CMD+i*4,v[i]);assert(db_fs_service());assert(ad_get(memory,FS_REPLY)==sequence);assert(!current);return ad_get(memory,FS_REPLY+4);
}
int main(void){
 struct ad_io io={sync_range,sync_range,barrier,barrier,NULL};char tmp[]="/tmp/zzdos-fs-XXXXXX",linkpath[1200];assert(mkdtemp(tmp));assert(db_fs_open(memory,&io,tmp));
 /* Dedicated HDD: no out-of-range write, no simultaneous file access. */
 snprintf(linkpath,sizeof(linkpath),"%s/HDD.IMG",tmp);int fd=open(linkpath,O_CREAT|O_EXCL|O_RDWR,0600);assert(fd>=0);assert(!ftruncate(fd,FS_DISK_BYTES));close(fd);
 assert(!call(FS_OPEN,0,0,0,0,"HDD.IMG",NULL));unsigned held=ad_get(memory,FS_REPLY+8);
 assert(call(FS_DISK_OPEN,0,0,0,0,"HDD.IMG",NULL)==5);
 assert(!call(FS_CLOSE,held,0,0,0,NULL,NULL));
 assert(!call(FS_DISK_OPEN,0,0,0,0,"HDD.IMG",NULL));assert(ad_get(memory,FS_REPLY+8)==FS_DISK_SECTORS);
 /* Sequential disk transfers skip only redundant seeks. A short transfer
  * invalidates the cursor even if the next request matches its old position. */
 unsigned seeks=mock_seek_calls;
 assert(!call(FS_DISK_READ,0,8,0,0,NULL,NULL));assert(mock_seek_calls==seeks+1);
 assert(!call(FS_DISK_READ,0,8,8,0,NULL,NULL));assert(mock_seek_calls==seeks+1);
 memset(memory+FS_DATA,0x6b,512);
 assert(!call(FS_DISK_WRITE,0,1,16,0,NULL,NULL));assert(mock_seek_calls==seeks+1);
 assert(!call(FS_DISK_READ,0,1,17,0,NULL,NULL));assert(mock_seek_calls==seeks+1);
 mock_short_read=1;assert(call(FS_DISK_READ,0,8,18,0,NULL,NULL)==5);
 assert(!call(FS_DISK_READ,0,8,18,0,NULL,NULL));assert(mock_seek_calls==seeks+2);
 assert(!call(FS_DISK_READ,0,1,16,0,NULL,NULL));assert(mock_seek_calls==seeks+3);
 for(unsigned k=0;k<512;k++)assert(memory[FS_DATA+k]==0x6b);
 assert(call(FS_OPEN,0,0,0,0,"HDD.IMG",NULL)!=0);
 assert(call(FS_CREATE,0,0,0,0,"HDD.IMG",NULL)!=0);
 assert(call(FS_DISK_OPEN,0,0,0,0,"HDD.IMG",NULL)==5);
 assert(call(FS_DISK_WRITE,0,1,FS_DISK_SECTORS,0,NULL,NULL)==5);
 assert(call(FS_DISK_WRITE,0,2,FS_DISK_SECTORS-1,0,NULL,NULL)==5);
 assert(call(FS_DISK_WRITE,0,0,0,0,NULL,NULL)==5);
 assert(call(FS_DISK_WRITE,0,9,0,0,NULL,NULL)==5);
 assert(call(FS_DISK_WRITE,0,1,0xffffffffu,0,NULL,NULL)==5);
 memset(memory+FS_DATA,0x73,512);assert(!call(FS_DISK_WRITE,0,1,FS_DISK_SECTORS-1,0,NULL,NULL));
 assert(!call(FS_DISK_CLOSE,0,0,0,0,NULL,NULL));
 assert(!call(FS_DISK_OPEN,0,0,0,0,"HDD.IMG",NULL));memset(memory+FS_DATA,0,512);
 assert(!call(FS_DISK_READ,0,1,FS_DISK_SECTORS-1,0,NULL,NULL));for(unsigned k=0;k<512;k++)assert(memory[FS_DATA+k]==0x73);
 assert(!call(FS_DISK_CLOSE,0,0,0,0,NULL,NULL));assert(call(FS_DISK_READ,0,1,0,0,NULL,NULL)==6);
 assert(!unlink(linkpath));
 /* Larger images retain per-file bounds and reject invalid geometry. */
 for(unsigned sectors=FS_DISK_SECTORS*2;sectors<=FS_DISK_MAX_SECTORS;sectors*=2){
  int large=open(linkpath,O_CREAT|O_EXCL|O_RDWR,0600);assert(large>=0);assert(!ftruncate(large,(off_t)sectors*512));close(large);
  assert(!call(FS_DISK_OPEN,0,0,0,0,"HDD.IMG",NULL));assert(ad_get(memory,FS_REPLY+8)==sectors);
  memset(memory+FS_DATA,0xa5,512);assert(!call(FS_DISK_WRITE,0,1,sectors-1,0,NULL,NULL));
  assert(call(FS_DISK_READ,0,2,sectors-1,0,NULL,NULL)==5);assert(call(FS_DISK_WRITE,0,1,sectors,0,NULL,NULL)==5);
  memset(memory+FS_DATA,0,512);assert(!call(FS_DISK_READ,0,1,sectors-1,0,NULL,NULL));for(unsigned k=0;k<512;k++)assert(memory[FS_DATA+k]==0xa5);
  assert(!call(FS_DISK_CLOSE,0,0,0,0,NULL,NULL));assert(!unlink(linkpath));
 }
 const unsigned invalid_sizes[]={FS_DISK_BYTES-512,FS_DISK_BYTES+1,FS_DISK_BYTES+512,FS_DISK_MAX_SECTORS*512u+262144u};
 for(unsigned i=0;i<sizeof(invalid_sizes)/sizeof(*invalid_sizes);i++){
  int invalid=open(linkpath,O_CREAT|O_EXCL|O_RDWR,0600);assert(invalid>=0);assert(!ftruncate(invalid,invalid_sizes[i]));close(invalid);
  assert(call(FS_DISK_OPEN,0,0,0,0,"HDD.IMG",NULL)==5);assert(!unlink(linkpath));
 }
 const char *bad[]={"../SECRET","SYS:STARTUP","A/B","A\\B","..","A.","TOOLONG99.TXT","NAME.LONG","*","#?","lower.txt","ABCDEFGHIJKLM"};
 for(unsigned i=0;i<sizeof(bad)/sizeof(*bad);i++)assert(call(FS_CREATE,0,0,0,0,bad[i],NULL)==5);
 snprintf(linkpath,sizeof(linkpath),"%s/LINK.TXT",tmp);assert(!symlink("/etc/hosts",linkpath));assert(call(FS_OPEN,0,0,0,0,"LINK.TXT",NULL)==5);assert(call(FS_CREATE,0,0,0,0,"LINK.TXT",NULL)==5);
 assert(!call(FS_CREATE,0,0,0,0,"LARGE.BIN",NULL));unsigned h=ad_get(memory,FS_REPLY+8);
 assert(call(FS_WRITE,h,4097,0,0,NULL,NULL)==5);
 for(unsigned j=0;j<25;j++){for(unsigned i=0;i<4096;i++)memory[FS_DATA+i]=(j*4096+i)%251;assert(!call(FS_WRITE,h,4096,0,0,NULL,NULL));assert(ad_get(memory,FS_REPLY+8)==4096);}
 assert(!call(FS_SEEK,h,0,99999,0,NULL,NULL));assert(!call(FS_WRITE,h,0,0,0,NULL,NULL));assert(!call(FS_CLOSE,h,0,0,0,NULL,NULL));assert(call(FS_READ,h,1,0,0,NULL,NULL)==6);
 assert(!call(FS_STAT,0,0,0,0,"LARGE.BIN",NULL));assert(ad_get(memory,FS_REPLY+12)==99999);
 unsigned file_date=ad_get(memory,FS_REPLY+20),file_time=ad_get(memory,FS_REPLY+24),file_attr=ad_get(memory,FS_REPLY+16);assert(file_date!=0);
 assert(!call(FS_OPEN,0,0,0,0,"LARGE.BIN",NULL));assert(ad_get(memory,FS_REPLY+12)==99999&&ad_get(memory,FS_REPLY+20)==file_date&&ad_get(memory,FS_REPLY+24)==file_time&&ad_get(memory,FS_REPLY+16)==file_attr);h=ad_get(memory,FS_REPLY+8);assert(call(FS_WRITE,h,1,0,0,NULL,NULL)==5);assert(!call(FS_SEEK,h,0,99990,0,NULL,NULL));assert(!call(FS_READ,h,20,0,0,NULL,NULL));assert(ad_get(memory,FS_REPLY+8)==9);for(unsigned i=0;i<9;i++)assert(memory[FS_DATA+i]==(99990+i)%251);assert(!call(FS_CLOSE,h,0,0,0,NULL,NULL));
 /* Reset RTC: pre-1980 Amiga timestamps still return a valid DOS date. */
 mock_days=4;assert(!call(FS_STAT,0,0,0,0,"LARGE.BIN",NULL));assert(ad_get(memory,FS_REPLY+20)==0x21&&ad_get(memory,FS_REPLY+24)==0);
 assert(!call(FS_OPEN,0,0,0,0,"LARGE.BIN",NULL));h=ad_get(memory,FS_REPLY+8);assert(ad_get(memory,FS_REPLY+20)==0x21);assert(!call(FS_CLOSE,h,0,0,0,NULL,NULL));mock_days=17817;
 assert(!call(FS_RENAME,0,0,0,0,"LARGE.BIN","NEW.BIN"));assert(call(FS_RENAME,0,0,0,0,"NEW.BIN","LINK.TXT")==5);
 assert(!call(FS_LIST,0,0,0,0,NULL,NULL));assert(!strcmp((char*)memory+FS_NAME,"NEW.BIN"));assert(call(FS_LIST,1,0,0,0,NULL,NULL)==18);
 /* A nested install tree, cross-directory moves, and no link traversal. */
 assert(!call(FS_MKDIR,0,0,0,0,"GAMES",NULL));
 strcpy((char*)memory+FS_DIR,"GAMES");
 assert(!call(FS_TESTDIR,0,0,0,0,NULL,NULL));
 assert(!call(FS_MKDIR,0,0,0,0,"SAVE",NULL));
 strcpy((char*)memory+FS_DIR,"GAMES\\SAVE");
 assert(!call(FS_CREATE,0,0,0,0,"STATE.DAT",NULL));h=ad_get(memory,FS_REPLY+8);
 /* Nested file I/O must use the acquired handle without enumerating parents.
  * Even changing the supplied cwd cannot redirect or invalidate this handle. */
 unsigned enumerations=mock_exnext_calls,metadata=metadata_sync;
 memset(memory+FS_DIR,0xff,FS_PATH_SIZE*2);
 assert(!call(FS_SEEK,h,0,0,0,NULL,NULL));
 assert(!call(FS_READ,h,1,0,0,NULL,NULL));
 assert(mock_exnext_calls==enumerations&&metadata_sync==metadata);
 strcpy((char*)memory+FS_DIR,"GAMES\\SAVE");memory[FS_DIR2]=0;
 memcpy(memory+FS_DATA,"saved",5);assert(!call(FS_WRITE,h,5,0,0,NULL,NULL));assert(!call(FS_CLOSE,h,0,0,0,NULL,NULL));
 puts("PASS handle reads/seeks/writes/close: zero path enumeration and zero metadata sync");
 assert(mock_exnext_calls==enumerations&&metadata_sync==metadata);
 assert(!call(FS_LIST,0,0,0,0,NULL,NULL));assert(!strcmp((char*)memory+FS_NAME,"STATE.DAT"));
 strcpy((char*)memory+FS_DIR2,"GAMES");assert(!call(FS_RENAME,0,0,0,0,"STATE.DAT","MOVED.DAT"));
 strcpy((char*)memory+FS_DIR,"GAMES");assert(!call(FS_OPEN,0,0,0,0,"MOVED.DAT",NULL));h=ad_get(memory,FS_REPLY+8);
 assert(!call(FS_READ,h,5,0,0,NULL,NULL));assert(!memcmp(memory+FS_DATA,"saved",5));assert(!call(FS_CLOSE,h,0,0,0,NULL,NULL));
 assert(!call(FS_STAT,0,0,0,0,"SAVE",NULL));assert(ad_get(memory,FS_REPLY+16)==16);
 assert(call(FS_DELETE,0,0,0,0,"SAVE",NULL)==5);
 assert(!call(FS_RMDIR,0,0,0,0,"SAVE",NULL));
 memory[FS_DIR]=memory[FS_DIR2]=0;assert(call(FS_RMDIR,0,0,0,0,"GAMES",NULL)!=0);
 snprintf(linkpath,sizeof(linkpath),"%s/GAMES/LINKDIR",tmp);assert(!symlink("/tmp",linkpath));
 strcpy((char*)memory+FS_DIR,"GAMES\\LINKDIR");assert(call(FS_TESTDIR,0,0,0,0,NULL,NULL)==3);assert(!unlink(linkpath));
 strcpy((char*)memory+FS_DIR,"GAMES\\..");assert(call(FS_TESTDIR,0,0,0,0,NULL,NULL)==5);
 strcpy((char*)memory+FS_DIR,"MISSING");assert(call(FS_TESTDIR,0,0,0,0,NULL,NULL)==3);
 strcpy((char*)memory+FS_DIR,"GAMES");assert(!call(FS_DELETE,0,0,0,0,"MOVED.DAT",NULL));
 memory[FS_DIR]=0;assert(!call(FS_RMDIR,0,0,0,0,"GAMES",NULL));
 snprintf(linkpath,sizeof(linkpath),"%s/LINK.TXT",tmp);
 for(unsigned i=0;i<16;i++)assert(!call(FS_OPEN,0,0,0,0,"NEW.BIN",NULL));assert(call(FS_OPEN,0,0,0,0,"NEW.BIN",NULL)==4);
 db_fs_close();assert(!unlink(linkpath));snprintf(linkpath,sizeof(linkpath),"%s/NEW.BIN",tmp);assert(!unlink(linkpath));assert(!rmdir(tmp));puts("PASS actual FS host: names, link rejection, bounds, permissions, exclusive HDD ownership/persistence/bounds, 100 KiB I/O, truncation, seek/EOF, rename, nested directories, cross-directory moves, traversal rejection, enumeration, handle limit/cleanup");
}
