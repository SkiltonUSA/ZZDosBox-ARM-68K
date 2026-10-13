/* Test-only AmigaDOS ABI facade; filesystem operations use a temporary root. */
#pragma once
#include <stdint.h>
#include <stddef.h>
typedef uint32_t ULONG;typedef int32_t LONG;typedef uint8_t UBYTE;typedef uintptr_t BPTR;typedef char *STRPTR;
struct DateStamp {LONG ds_Days,ds_Minute,ds_Tick;};
struct FileInfoBlock {LONG fib_DirEntryType,fib_Size,fib_Protection;char fib_FileName[108];struct DateStamp fib_Date;};
struct InfoData {LONG id_NumBlocks,id_NumBlocksUsed,id_BytesPerBlock;};
#define ST_FILE -3
#define ST_USERDIR 2
#define FIBF_WRITE 4
#define TICKS_PER_SECOND 50
#define ACCESS_READ -2
#define EXCLUSIVE_LOCK -1
#define MODE_NEWFILE 1006
#define MODE_OLDFILE 1005
#define OFFSET_BEGINNING -1
#define OFFSET_CURRENT 0
#define OFFSET_END 1
#define ERROR_NO_MORE_ENTRIES 232
#define ERROR_OBJECT_NOT_FOUND 205
#define ERROR_DISK_FULL 221
LONG IoErr(void);BPTR Lock(STRPTR,LONG);void UnLock(BPTR);LONG Examine(BPTR,struct FileInfoBlock*);LONG ExNext(BPTR,struct FileInfoBlock*);BPTR CurrentDir(BPTR);BPTR Open(STRPTR,LONG);LONG Close(BPTR);LONG Read(BPTR,void*,ULONG);LONG Write(BPTR,const void*,ULONG);LONG Seek(BPTR,LONG,LONG);LONG SetFileSize(BPTR,LONG,LONG);LONG DeleteFile(STRPTR);LONG Rename(STRPTR,STRPTR);LONG Info(BPTR,struct InfoData*);

BPTR CreateDir(STRPTR);LONG NameFromLock(BPTR,STRPTR,LONG);LONG AddPart(STRPTR,STRPTR,LONG);

BPTR OpenFromLock(BPTR);LONG Flush(BPTR);
