/* GPL-3.0-or-later. Live DOS C: drive; never accepts an Amiga path. */
#include <dos/dos.h>
#include <dos/dosextens.h>
#include <proto/dos.h>
#include <stdio.h>
#include <string.h>
#include "fs_host.h"
#include "fs_wire.h"
#ifdef ZZ_PC_CD
#include "cd_host.h"
#endif
static volatile uint8_t *shared;
static const struct ad_io *syncio;
static BPTR root,working,disk,handles[FS_HANDLES];
static unsigned access_mode[FS_HANDLES];
static ULONG sequence,requests,read_bytes,written_bytes;
static struct FileInfoBlock fib;
static int leap(unsigned y){return !(y%4)&&(y%100||!(y%400));}
static void timestamp(ULONG *date,ULONG *time){
 static const UBYTE months[]={31,28,31,30,31,30,31,31,30,31,30,31};
 LONG days=fib.fib_Date.ds_Days;unsigned y=1978,m=0,n;
 if(days<0)return;
 while(y<2108&&(ULONG)days>=(ULONG)(365+leap(y))){days-=365+leap(y);y++;}
 if(y<1980||y>2107)return;
 while(m<11){n=months[m]+(m==1&&leap(y));if((ULONG)days<n)break;days-=n;m++;}
 *date=((y-1980)<<9)|((m+1)<<5)|(days+1);
 *time=((fib.fib_Date.ds_Minute/60)<<11)|((fib.fib_Date.ds_Minute%60)<<5)|((fib.fib_Date.ds_Tick/TICKS_PER_SECOND)/2);
}
static void uppercase(char *s){for(;*s;s++)if(*s>='a'&&*s<='z')*s-=32;}
/* ExNext observes link entry types without resolving them. No linked files or
 * directories are admitted. DOS names are literal, never AmigaDOS patterns. */
static int entry(const char *name,ULONG ordinal,int listing){
 ULONG i=0;char candidate[108];
 if(!Examine(working,&fib))return -1;
 while(ExNext(working,&fib)){
  strncpy(candidate,(const char *)fib.fib_FileName,sizeof(candidate)-1);candidate[sizeof(candidate)-1]=0;uppercase(candidate);
  if(listing){
   if((fib.fib_DirEntryType!=ST_FILE&&fib.fib_DirEntryType!=ST_USERDIR)||!fs_name_valid(candidate)||fib.fib_Size<0)continue;
   if(i++==ordinal)return 1;
  }else if(!strcmp(candidate,name))return (fib.fib_DirEntryType==ST_FILE||fib.fib_DirEntryType==ST_USERDIR)&&fib.fib_Size>=0?1:-1;
 }
 return IoErr()==ERROR_NO_MORE_ENTRIES?0:-1;
}
/* Walk each validated component with ExNext before Lock: never resolve a link. */
static BPTR directory(const char *path){
 BPTR at=root,next,old;char part[FS_NAME_SIZE];unsigned n;
 if(!fs_dir_valid(path))return 0;
 while(*path){
  n=0;while(*path&&*path!='\\')part[n++]=*path++;part[n]=0;if(*path)path++;
  working=at;if(entry(part,0,0)!=1||fib.fib_DirEntryType!=ST_USERDIR){if(at!=root)UnLock(at);return 0;}
  old=CurrentDir(at);next=Lock((STRPTR)fib.fib_FileName,ACCESS_READ);CurrentDir(old);
  if(at!=root)UnLock(at);if(!next)return 0;at=next;
 }return at;
}
static ULONG attributes(void){return fib.fib_DirEntryType==ST_USERDIR?16:(fib.fib_Protection&FIBF_WRITE)?1:32;}
static ULONG error(void){LONG n=IoErr();return n==ERROR_OBJECT_NOT_FOUND?2:n==ERROR_DISK_FULL?112:5;}
int db_fs_open(volatile uint8_t *base,const struct ad_io *io,const char *path){
 shared=base;syncio=io;sequence=requests=read_bytes=written_bytes=0;memset(handles,0,sizeof(handles));
 root=Lock((STRPTR)path,ACCESS_READ);if(!root)return 0;
 if(!Examine(root,&fib)||fib.fib_DirEntryType<=0){UnLock(root);root=0;return 0;}
 return 1;
}
int db_fs_service(void){
 ULONG next,op,h,n,pos,whence,result=0,size=0,attr=0,date=0,time=0,err=0,i;
 BPTR old=0,parent=0,other=0;char name[FS_NAME_SIZE],name2[FS_NAME_SIZE],dir[FS_PATH_SIZE],dir2[FS_PATH_SIZE];UBYTE data[FS_CHUNK];LONG value;int found;
 syncio->pull(shared+FS_CMD,24,NULL);next=ad_get(shared,FS_CMD);if(next==sequence)return 1;
 if(next!=sequence+1){err=5;goto reply;}
 op=ad_get(shared,FS_CMD+4);h=ad_get(shared,FS_CMD+8);n=ad_get(shared,FS_CMD+12);pos=ad_get(shared,FS_CMD+16);whence=ad_get(shared,FS_CMD+20);
 syncio->pull(shared+FS_NAME,32,NULL);
 for(i=0;i<FS_NAME_SIZE;i++){name[i]=shared[FS_NAME+i];name2[i]=shared[FS_NAME2+i];}
 syncio->pull(shared+FS_DIR,FS_PATH_SIZE*2,NULL);
 for(i=0;i<FS_PATH_SIZE;i++){dir[i]=shared[FS_DIR+i];dir2[i]=shared[FS_DIR2+i];}
 if(!fs_dir_valid(dir)||!fs_dir_valid(dir2)){err=5;goto reply;}
 if((op==FS_OPEN||op==FS_CREATE||op==FS_STAT||op==FS_DELETE||op==FS_RENAME||op==FS_MKDIR||op==FS_RMDIR||op==FS_DISK_OPEN)&&!fs_name_valid(name)){err=5;goto reply;}
 if(op==FS_RENAME&&!fs_name_valid(name2)){err=5;goto reply;}
 if((op==FS_READ||op==FS_WRITE)&&n>FS_CHUNK){err=5;goto reply;}
 if((op==FS_CLOSE||op==FS_READ||op==FS_WRITE||op==FS_SEEK)&&(h==0||h>FS_HANDLES||!handles[h-1])){err=6;goto reply;}
 parent=directory(dir);if(!parent){err=3;goto reply;}working=parent;old=CurrentDir(parent);requests++;
 switch(op){
#ifdef ZZ_PC_CD
 case FS_CD_READ:
  if(!n||n>2||!db_cd_host_read(pos,n,data)){err=21;break;}
  result=n;for(i=0;i<n*2048;i++)shared[FS_DATA+i]=data[i];syncio->push(shared+FS_DATA,n*2048,NULL);read_bytes+=n*2048;break;
#endif
 case FS_DISK_OPEN:{
  BPTR lock;
  if(disk||*dir||entry(name,0,0)!=1||fib.fib_DirEntryType!=ST_FILE){err=5;break;}
  /* OpenFromLock preserves exclusivity without truncating the image. */
  lock=Lock((STRPTR)fib.fib_FileName,EXCLUSIVE_LOCK);if(!lock){err=5;break;}
  if(!Examine(lock,&fib)||fib.fib_DirEntryType!=ST_FILE||fib.fib_Size!=FS_DISK_BYTES){UnLock(lock);err=5;break;}
  disk=OpenFromLock(lock);if(!disk){UnLock(lock);err=5;break;}result=FS_DISK_SECTORS;break;
 }
 case FS_DISK_CLOSE:
  if(!disk){err=6;break;}
  if(!Flush(disk))err=5;
  if(!Close(disk))err=5;
  disk=0;break;
 case FS_DISK_READ:case FS_DISK_WRITE:
  if(!disk){err=6;break;}
  if(!n||n>FS_CHUNK/512||pos>=FS_DISK_SECTORS||n>FS_DISK_SECTORS-pos){err=5;break;}
  if(Seek(disk,pos*512u,OFFSET_BEGINNING)<0){err=5;break;}
  n*=512;
  if(op==FS_DISK_READ){
   value=Read(disk,data,n);if(value!=(LONG)n){err=5;break;}
   for(i=0;i<n;i++)shared[FS_DATA+i]=data[i];
   syncio->push(shared+FS_DATA,n,NULL);read_bytes+=n;
  }else{
   syncio->pull(shared+FS_DATA,n,NULL);for(i=0;i<n;i++)data[i]=shared[FS_DATA+i];
   value=Write(disk,data,n);if(value!=(LONG)n||!Flush(disk)){err=5;break;}written_bytes+=n;
  }result=n/512;break;
 case FS_OPEN:case FS_CREATE:
  if(op==FS_OPEN&&n>2){err=12;break;}
  found=entry(name,0,0);if(found<0||(found&&fib.fib_DirEntryType!=ST_FILE)){err=5;break;}if(found)strcpy(name,(const char*)fib.fib_FileName);if(!found&&op==FS_OPEN){err=2;break;}
  for(i=0;i<FS_HANDLES&&handles[i];i++){}
  if(i==FS_HANDLES){err=4;break;}
  handles[i]=Open((STRPTR)name,op==FS_CREATE?MODE_NEWFILE:MODE_OLDFILE);
  if(!handles[i]){err=error();break;}access_mode[i]=op==FS_CREATE?2:n;result=i+1;break;
 case FS_CLOSE:
  if(!Close(handles[h-1]))err=error();
  handles[h-1]=0;break;
 case FS_READ:
  if(access_mode[h-1]==1){err=5;break;}
  value=Read(handles[h-1],data,n);if(value<0){err=error();break;}
  result=value;read_bytes+=result;for(i=0;i<result;i++)shared[FS_DATA+i]=data[i];syncio->push(shared+FS_DATA,result,NULL);break;
 case FS_WRITE:
  if(access_mode[h-1]==0){err=5;break;}
  if(!n){if(SetFileSize(handles[h-1],0,OFFSET_CURRENT)<0)err=error();break;}
  syncio->pull(shared+FS_DATA,n,NULL);for(i=0;i<n;i++)data[i]=shared[FS_DATA+i];
  value=Write(handles[h-1],data,n);if(value<0){err=error();break;}result=value;written_bytes+=result;break;
 case FS_SEEK:
  if(whence>2){err=1;break;}
  if(Seek(handles[h-1],(LONG)pos,whence==0?OFFSET_BEGINNING:whence==1?OFFSET_CURRENT:OFFSET_END)<0){err=error();break;}
  value=Seek(handles[h-1],0,OFFSET_CURRENT);if(value<0){err=error();break;}result=value;break;
 case FS_STAT:
  found=entry(name,0,0);if(found!=1){err=found<0?5:2;break;}size=fib.fib_DirEntryType==ST_USERDIR?0:fib.fib_Size;attr=attributes();timestamp(&date,&time);break;
 case FS_LIST:
  if(h>=4096){err=18;break;}found=entry(NULL,h,1);if(found!=1){err=found<0?5:18;break;}
  strncpy(name,(const char *)fib.fib_FileName,sizeof(name)-1);name[sizeof(name)-1]=0;uppercase(name);
  size=fib.fib_DirEntryType==ST_USERDIR?0:fib.fib_Size;attr=attributes();timestamp(&date,&time);
  for(i=0;i<FS_NAME_SIZE;i++)shared[FS_NAME+i]=name[i];
  syncio->push(shared+FS_NAME,FS_NAME_SIZE,NULL);break;
 case FS_DELETE:
  if(entry(name,0,0)!=1||fib.fib_DirEntryType!=ST_FILE){err=5;break;}if(!DeleteFile((STRPTR)fib.fib_FileName))err=error();break;
 case FS_MKDIR:
  if(entry(name,0,0)!=0){err=5;break;}{BPTR made=CreateDir((STRPTR)name);if(!made)err=error();else UnLock(made);}break;
 case FS_RMDIR:
  if(entry(name,0,0)!=1||fib.fib_DirEntryType!=ST_USERDIR){err=5;break;}if(!DeleteFile((STRPTR)fib.fib_FileName))err=error();break;
 case FS_TESTDIR:break; /* directory() has already checked all components. */
 case FS_RENAME:{
  char from[108],to[1024];
  if(entry(name,0,0)!=1){err=5;break;}strcpy(from,(const char*)fib.fib_FileName);
  other=directory(dir2);if(!other){err=3;break;}working=other;
  if(entry(name2,0,0)!=0||!NameFromLock(other,to,sizeof(to))||!AddPart(to,(STRPTR)name2,sizeof(to))){err=5;break;}
  if(!Rename((STRPTR)from,(STRPTR)to))err=error();
  break;
 }
 case FS_SPACE:{
  struct InfoData info;unsigned long long total,free_bytes;
  if(!Info(root,&info)||info.id_NumBlocks<0||info.id_NumBlocksUsed<0||info.id_NumBlocksUsed>info.id_NumBlocks||info.id_BytesPerBlock<=0){err=5;break;}
  total=(unsigned long long)info.id_NumBlocks*info.id_BytesPerBlock/32768;
  free_bytes=(unsigned long long)(info.id_NumBlocks-info.id_NumBlocksUsed)*info.id_BytesPerBlock/32768;
  result=total>65535?65535:(ULONG)total;size=free_bytes>65535?65535:(ULONG)free_bytes;break;
 }
 default:err=1;break;
 }
 CurrentDir(old);
 if(other&&other!=root)UnLock(other);if(parent&&parent!=root)UnLock(parent);
reply:
 ad_put(shared,FS_REPLY+4,err);ad_put(shared,FS_REPLY+8,result);ad_put(shared,FS_REPLY+12,size);ad_put(shared,FS_REPLY+16,attr);ad_put(shared,FS_REPLY+20,date);ad_put(shared,FS_REPLY+24,time);
 syncio->push(shared+FS_REPLY+4,24,NULL);syncio->barrier(NULL);ad_put(shared,FS_REPLY,next);syncio->push(shared+FS_REPLY,4,NULL);sequence=next;return 1;
}
void db_fs_close(void){
 unsigned i;if(disk){int ok=Flush(disk);if(!Close(disk))ok=0;disk=0;printf("BIOS HDD close=%s\n",ok?"ok":"FAILED");}for(i=0;i<FS_HANDLES;i++)if(handles[i]){Close(handles[i]);handles[i]=0;}
 if(root){UnLock(root);root=0;}
 printf("FILES requests=%u read=%u written=%u; handles closed\n",requests,read_bytes,written_bytes);
}
