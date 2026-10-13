/* GPL-2.0-or-later. DOS_Drive frontend forwarding file work to the 68k. */
#include "drive.h"
#include "drive_decl.h"
#include "fs_wire.h"
#include "shell_api.h"
#include "disk_cache.h"
#include <string.h>
extern "C" volatile uint8_t *db_shared();
extern bool db_shell_finishing();
static uint32_t sequence;
static char search_dirs[64][FS_PATH_SIZE];static unsigned search_count;
static void fence(){__asm__ volatile("dsb sy" ::: "memory");}
static unsigned reply(unsigned n){return ad_get(db_shared(),FS_REPLY+n*4);}
static bool split_path(const char *path,char *dir,char *leaf){
 if(!path||strlen(path)>=FS_PATH_SIZE)return false;
 const char *last=strrchr(path,'\\');
 if(last){unsigned n=last-path;memcpy(dir,path,n);dir[n]=0;path=last+1;}else dir[0]=0;
 if(!fs_dir_valid(dir)||!fs_name_valid(path))return false;strcpy(leaf,path);return true;
}
static bool rpc(unsigned op,unsigned handle=0,unsigned count=0,unsigned pos=0,unsigned whence=0,const char *name=nullptr,const char *name2=nullptr){
 if(db_shell_finishing()){DOS_SetError(DOSERR_ACCESS_DENIED);return false;}
 volatile uint8_t *s=db_shared();db_shell_poll();char dir[FS_PATH_SIZE]={},dir2[FS_PATH_SIZE]={},leaf[FS_NAME_SIZE]={},leaf2[FS_NAME_SIZE]={};
 bool valid=true;
 if(op==FS_LIST||op==FS_TESTDIR){if(name&&strlen(name)<FS_PATH_SIZE&&fs_dir_valid(name))strcpy(dir,name);else if(name)valid=false;}
 else if(name)valid=split_path(name,dir,leaf);
 if(name2)valid=valid&&split_path(name2,dir2,leaf2);
 if(!valid){DOS_SetError(DOSERR_ACCESS_DENIED);return false;}
 for(unsigned i=0;i<FS_NAME_SIZE;i++){s[FS_NAME+i]=leaf[i];s[FS_NAME2+i]=leaf2[i];}
 for(unsigned i=0;i<FS_PATH_SIZE;i++){s[FS_DIR+i]=dir[i];s[FS_DIR2+i]=dir2[i];}
 ad_put(s,FS_CMD+4,op);ad_put(s,FS_CMD+8,handle);ad_put(s,FS_CMD+12,count);ad_put(s,FS_CMD+16,pos);ad_put(s,FS_CMD+20,whence);
 fence();ad_put(s,FS_CMD,++sequence);fence();
 while(ad_get(s,FS_REPLY)!=sequence)db_shell_poll();fence();
 unsigned error=reply(1);if(error){DOS_SetError(error);return false;}return true;
}
DOS_Drive_Cache::DOS_Drive_Cache(){memset(static_cast<void*>(this),0,sizeof(*this));}
DOS_Drive_Cache::~DOS_Drive_Cache(){}
void DOS_Drive_Cache::EmptyCache(){}
DOS_Drive::DOS_Drive(){curdir[0]=0;strcpy(info,"ZZDosBox C drive");}
char *DOS_Drive::GetInfo(){return info;}
class RemoteFile:public DOS_File {
 unsigned handle;
public:
 RemoteFile(unsigned h,unsigned f):handle(h){open=true;flags=f;}
 bool Read(Bit8u *p,Bit16u *n)override{
  if((flags&3)==OPEN_WRITE){DOS_SetError(DOSERR_ACCESS_DENIED);return false;}
  unsigned want=*n,done=0;
  while(done<want){unsigned k=want-done;if(k>FS_CHUNK)k=FS_CHUNK;
   if(!rpc(FS_READ,handle,k)){*n=done;return false;}unsigned got=reply(2);if(got>k)E_Exit("Invalid FS read response");
   for(unsigned i=0;i<got;i++)p[done+i]=db_shared()[FS_DATA+i];done+=got;if(got<k)break;
  }*n=done;return true;
 }
 bool Write(Bit8u *p,Bit16u *n)override{
  if((flags&3)==OPEN_READ){DOS_SetError(DOSERR_ACCESS_DENIED);return false;}
  unsigned want=*n,done=0;
  do{unsigned k=want-done;if(k>FS_CHUNK)k=FS_CHUNK;
   for(unsigned i=0;i<k;i++)db_shared()[FS_DATA+i]=p[done+i];fence();
   if(!rpc(FS_WRITE,handle,k)){*n=done;return false;}unsigned got=reply(2);if(got>k)E_Exit("Invalid FS write response");done+=got;if(got<k)break;
  }while(done<want);*n=done;return true;
 }
 bool Seek(Bit32u *p,Bit32u type)override{if(!rpc(FS_SEEK,handle,0,*p,type))return false;*p=reply(2);return true;}
 bool Close()override{if(!open)return true;bool ok=rpc(FS_CLOSE,handle);open=false;return ok;}
 Bit16u GetInformation()override{return 0;}
};
class RemoteDrive:public DOS_Drive {
public:
 bool FileOpen(DOS_File **f,char *name,Bit32u flags)override{if(!rpc(FS_OPEN,0,flags&3,0,0,name))return false;*f=new RemoteFile(reply(2),flags);return true;}
 bool FileCreate(DOS_File **f,char *name,Bit16u attr)override{if(attr&DOS_ATTR_DIRECTORY)return false;if(!rpc(FS_CREATE,0,0,0,0,name))return false;*f=new RemoteFile(reply(2),OPEN_READWRITE);return true;}
 bool FileUnlink(char *name)override{return rpc(FS_DELETE,0,0,0,0,name);}
 bool RemoveDir(char*name)override{return rpc(FS_RMDIR,0,0,0,0,name);}
 bool MakeDir(char*name)override{return rpc(FS_MKDIR,0,0,0,0,name);}
 bool TestDir(char *name)override{return rpc(FS_TESTDIR,0,0,0,0,name);}
 bool FindFirst(char *dir,DOS_DTA &dta,bool=false)override{
  if(!fs_dir_valid(dir)||!TestDir(dir))return false;
  unsigned i;for(i=0;i<search_count&&strcmp(search_dirs[i],dir);i++){}
  if(i==search_count){if(search_count==64){DOS_SetError(DOSERR_TOO_MANY_OPEN_FILES);return false;}strcpy(search_dirs[search_count++],dir);}
  dta.SetDirIDCluster(i);dta.SetDirID(0);return FindNext(dta);
 }
 bool FindNext(DOS_DTA &dta)override{
  Bit8u attr;char pattern[DOS_NAMELENGTH_ASCII];dta.GetSearchParams(attr,pattern);
  if(attr==DOS_ATTR_VOLUME){DOS_SetError(DOSERR_NO_MORE_FILES);return false;}
  for(unsigned i=dta.GetDirID();i<4096;i++){
   if(dta.GetDirIDCluster()>=search_count){DOS_SetError(DOSERR_NO_MORE_FILES);return false;}
   if(!rpc(FS_LIST,i,0,0,0,search_dirs[dta.GetDirIDCluster()]))return false;char name[FS_NAME_SIZE];for(unsigned k=0;k<FS_NAME_SIZE;k++)name[k]=db_shared()[FS_NAME+k];
   if(!fs_name_valid(name))E_Exit("Invalid FS list response");
   if(!(reply(4)&DOS_ATTR_DIRECTORY)|| (attr&DOS_ATTR_DIRECTORY))if(WildFileCmp(name,pattern)){dta.SetResult(name,reply(3),reply(5),reply(6),reply(4));dta.SetDirID(i+1);return true;}
  }DOS_SetError(DOSERR_NO_MORE_FILES);return false;
 }
 bool GetFileAttr(char *name,Bit16u *attr)override{if(!*name){*attr=DOS_ATTR_DIRECTORY;return true;}if(!rpc(FS_STAT,0,0,0,0,name))return false;*attr=reply(4);return true;}
 bool Rename(char *a,char *b)override{return rpc(FS_RENAME,0,0,0,0,a,b);}
 bool AllocationInfo(Bit16u *b,Bit8u *s,Bit16u *t,Bit16u *f)override{if(!rpc(FS_SPACE))return false;*b=512;*s=64;*t=reply(2);*f=reply(3);return true;}
 bool FileExists(const char *name)override{return rpc(FS_STAT,0,0,0,0,name)&&!(reply(4)&DOS_ATTR_DIRECTORY);}
 bool FileStat(const char *name,FileStat_Block *st)override{if(!rpc(FS_STAT,0,0,0,0,name))return false;memset(st,0,sizeof(*st));st->size=reply(3);st->attr=reply(4);return true;}
 Bit8u GetMediaByte()override{return 0xf8;}bool isRemote()override{return true;}bool isRemovable()override{return false;}Bits UnMount()override{return 0;}
};
void db_mount(const uint8_t*,unsigned){Drives[2]=new RemoteDrive();DOS_SetDefaultDrive(2);sequence=0;search_count=0;}
void db_stdio(DOS_PSP &){for(unsigned i=0;i<3;i++){Bit16u h;if(!DOS_OpenFile("CON",OPEN_READWRITE,&h)||h!=i)E_Exit("Standard handle setup failed");}}
extern DOS_Device *Devices[DOS_DEVICES];
void db_unmount(){for(unsigned i=0;i<DOS_DEVICES;i++){delete Devices[i];Devices[i]=nullptr;}for(unsigned i=0;i<DOS_FILES;i++){delete Files[i];Files[i]=nullptr;}for(unsigned i=0;i<DOS_DRIVES;i++){delete Drives[i];Drives[i]=nullptr;}}

/* Boot media uses the same checked read-only file service, before DOS starts. */
bool db_load_boot(const char *name,uint8_t *data,unsigned size){
 sequence=0;
 if(!rpc(FS_STAT,0,0,0,0,name)||reply(3)!=size||(reply(4)&16))return false;
 if(!rpc(FS_OPEN,0,0,0,0,name))return false;
 unsigned handle=reply(2);bool ok=true;
 for(unsigned at=0;at<size;at+=FS_CHUNK){
  unsigned n=size-at;if(n>FS_CHUNK)n=FS_CHUNK;
  if(!rpc(FS_READ,handle,n)||reply(2)!=n){ok=false;break;}
  for(unsigned i=0;i<n;i++)data[at+i]=db_shared()[FS_DATA+i];
 }
 return rpc(FS_CLOSE,handle)&&ok;
}

/* BIOS hard disk: the host retains an exclusive lock for the whole session. */
static DbDiskCache disk_cache;
static_assert(DbDiskCache::sectors*DbDiskCache::sector_bytes<=FS_CHUNK,"disk read-ahead exceeds mailbox");
bool db_disk_open(const char *name){disk_cache.reset();return rpc(FS_DISK_OPEN,0,0,0,0,name)&&reply(2)==FS_DISK_SECTORS;}
static bool disk_transfer(unsigned sector,unsigned n,void *data,bool writing){
 if(writing){for(unsigned i=0;i<n*512;i++)db_shared()[FS_DATA+i]=((uint8_t*)data)[i];fence();}
 if(!rpc(writing?FS_DISK_WRITE:FS_DISK_READ,0,n,sector)||reply(2)!=n)return false;
 if(!writing)for(unsigned i=0;i<n*512;i++)((uint8_t*)data)[i]=db_shared()[FS_DATA+i];
 return true;
}
bool db_disk_sector(unsigned sector,void *data,bool writing){
 db_shell_poll(); /* Cache hits retain cooperative cancellation. */
 return disk_cache.access(sector,data,writing,FS_DISK_SECTORS,disk_transfer);
}

/* Selected CD source is host-owned and always read-only. */
bool db_cd_sector(unsigned sector,void *data){
 if(!rpc(FS_CD_READ,0,1,sector)||reply(2)!=1)return false;
 for(unsigned i=0;i<2048;i++)((uint8_t*)data)[i]=db_shared()[FS_DATA+i];return true;
}
