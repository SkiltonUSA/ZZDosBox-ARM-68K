/* GPL-2.0-or-later. One outstanding filesystem RPC, offsets in owned DDR. */
#pragma once
#include "stream.h"
#define FS_CMD 0x186000u
#define FS_REPLY (FS_CMD+64u)
#define FS_NAME (FS_CMD+128u)
#define FS_NAME2 (FS_CMD+144u)
#define FS_DIR (FS_CMD+256u)
#define FS_DIR2 (FS_CMD+512u)
#define FS_PATH_SIZE 256u
#define FS_DATA 0x187000u
#define FS_CHUNK 4096u
#define FS_HANDLES 16u
#define FS_NAME_SIZE 13u
/* Command: seq, operation, handle/index, count/flags, position, whence.
 * Reply: ack, DOS error, result, size, attributes, DOS date, DOS time. */
enum { FS_OPEN=1,FS_CREATE,FS_CLOSE,FS_READ,FS_WRITE,FS_SEEK,FS_STAT,FS_LIST,FS_DELETE,FS_RENAME,FS_SPACE,FS_MKDIR,FS_RMDIR,FS_TESTDIR,FS_DISK_OPEN,FS_DISK_CLOSE,FS_DISK_READ,FS_DISK_WRITE,FS_CD_READ };
/* Only these operations publish/consume path metadata. Handle/device operations
 * leave it untouched; both ends must use this rule (no extra protocol fields). */
static inline int fs_uses_path(unsigned op){
 return op==FS_OPEN||op==FS_CREATE||op==FS_STAT||op==FS_LIST||op==FS_DELETE||
        op==FS_RENAME||op==FS_MKDIR||op==FS_RMDIR||op==FS_TESTDIR||op==FS_DISK_OPEN;
}
/* BIOS HDD geometry: 16 heads, 32 sectors, 128..1024 cylinders (32..256 MiB).
 * Disk RPCs use a dedicated exclusive handle, sector index and 1..8 sectors. */
#define FS_DISK_SECTORS 65536u
#define FS_DISK_BYTES (FS_DISK_SECTORS*512u)
#define FS_DISK_MAX_SECTORS 524288u
static inline int fs_disk_sectors_valid(unsigned n){return n>=FS_DISK_SECTORS && n<=FS_DISK_MAX_SECTORS && !(n%512u);}
/* DOS 8.3 path components. No path separator, device, parent, or wildcard. */
static inline int fs_name_valid(const char *s){
 unsigned i,stem=0,ext=0,dot=0;
 for(i=0;i<FS_NAME_SIZE;i++){
  unsigned char c=(unsigned char)s[i];if(!c)return stem&&(!dot||ext);
  if(c=='.'){if(dot||!stem)return 0;dot=1;continue;}
  if(!((c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='_'||c=='-'))return 0;
  if(dot){if(++ext>3)return 0;}else if(++stem>8)return 0;
 }
 return 0;
}
/* Relative canonical DOS directory: no drive prefix, parent traversal or links. */
static inline int fs_dir_valid(const char *s){
 unsigned i=0,j=0;char part[FS_NAME_SIZE];
 if(!*s)return 1;
 for(;i<FS_PATH_SIZE;i++){
  char c=s[i];
  if(!c||c=='\\'){part[j]=0;if(!fs_name_valid(part))return 0;j=0;if(!c)return 1;}
  else {if(j>=FS_NAME_SIZE-1)return 0;part[j++]=c;}
 }return 0;
}
#if FS_DATA+FS_CHUNK > DB_PROGRAM
#error Filesystem transport overlaps program storage
#endif
