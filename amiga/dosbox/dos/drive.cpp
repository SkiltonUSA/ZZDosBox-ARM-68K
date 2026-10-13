#include "drive.h"
#include "drive_decl.h"
#include <string.h>
#include <stdlib.h>
// Temporary platform frontend: DOSBox owns DOS services, PSPs and handles.
// The private drive contains only the program supplied by the launcher.
#include "files.h"
DbDiskFile db_files[DB_FILE_COUNT];
static int find(const char *name){for(unsigned i=0;i<DB_FILE_COUNT;i++)if(db_files[i].name[0]&&!strcmp(name,db_files[i].name))return i;return -1;}
static bool valid(const char *name){
 unsigned stem=0,ext=0;bool dot=false;
 for(;*name;name++){
  char c=*name;if(c=='.'){if(dot||!stem)return false;dot=true;continue;}
  if(!((c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='_'||c=='-'))return false;
  if(dot){if(++ext>3)return false;}else if(++stem>8)return false;
 }
 return stem&&(!dot||ext);
}
extern "C" void db_files_reset(){memset(db_files,0,sizeof(db_files));}
extern "C" int db_file_put(const char *name,const uint8_t *data,unsigned n,int ro){
 if(!valid(name)||n>DB_FILE_CAPACITY||(!data&&n))return 0;
 int i=find(name);if(i<0)for(unsigned j=0;j<DB_FILE_COUNT;j++)if(!db_files[j].name[0]){i=j;break;}
 if(i<0)return 0;auto &f=db_files[i];strcpy(f.name,name);f.size=n;f.readonly=ro;if(n)memcpy(f.data,data,n);return 1;
}
extern "C" int db_file_get(const char *name,const uint8_t **data,unsigned *n){int i=find(name);if(i<0)return 0;*data=db_files[i].data;*n=db_files[i].size;return 1;}
DOS_Drive_Cache::DOS_Drive_Cache(){memset(static_cast<void *>(this),0,sizeof(*this));}
DOS_Drive_Cache::~DOS_Drive_Cache(){}
void DOS_Drive_Cache::EmptyCache(){}
DOS_Drive::DOS_Drive(){curdir[0]=0;strcpy(info,"ZZDosBox private drive");}
char *DOS_Drive::GetInfo(){return info;}
class MemoryFile:public DOS_File {
 unsigned pos=0;DbDiskFile &file;
public:
 MemoryFile(DbDiskFile &f,unsigned access):file(f){open=true;flags=access;attr=f.readonly?DOS_ATTR_READ_ONLY:DOS_ATTR_ARCHIVE;}
 bool Read(Bit8u *data,Bit16u *size)override{if((flags&3)==OPEN_WRITE){DOS_SetError(DOSERR_ACCESS_DENIED);return false;}unsigned left=pos<file.size?file.size-pos:0;if(*size>left)*size=left;memcpy(data,file.data+pos,*size);pos+=*size;return true;}
 bool Write(Bit8u *data,Bit16u *size)override{if(file.readonly||(flags&3)==OPEN_READ){DOS_SetError(DOSERR_ACCESS_DENIED);return false;}if(*size>DB_FILE_CAPACITY-pos)*size=DB_FILE_CAPACITY-pos;if(!*size){if(pos>file.size)memset(file.data+file.size,0,pos-file.size);file.size=pos;return true;}if(pos>file.size)memset(file.data+file.size,0,pos-file.size);memcpy(file.data+pos,data,*size);pos+=*size;if(pos>file.size)file.size=pos;return true;}
 bool Seek(Bit32u *p,Bit32u type)override{int64_t dest=(type==0?0:type==1?pos:file.size)+(int64_t)(int32_t)*p;if(type>2||dest<0||dest>DB_FILE_CAPACITY){DOS_SetError(DOSERR_FUNCTION_NUMBER_INVALID);return false;}pos=(unsigned)dest;*p=pos;return true;}
 bool Close()override{open=false;return true;}
 Bit16u GetInformation()override{return 0;}
};
class MemoryDrive:public DOS_Drive {
public:
 bool FileOpen(DOS_File **f,char *name,Bit32u flags)override{if(!FileExists(name)){DOS_SetError(DOSERR_FILE_NOT_FOUND);return false;}if((flags&3)>2||((flags&3)!=OPEN_READ&&db_files[find(name)].readonly)){DOS_SetError(DOSERR_ACCESS_DENIED);return false;}*f=new MemoryFile(db_files[find(name)],flags);return true;}
 bool FileCreate(DOS_File **f,char *name,Bit16u)override{int i=find(name);if((i>=0&&db_files[i].readonly)||!db_file_put(name,nullptr,0,0)){DOS_SetError(DOSERR_ACCESS_DENIED);return false;}*f=new MemoryFile(db_files[find(name)],OPEN_READWRITE);return true;}
 bool FileUnlink(char*)override{DOS_SetError(DOSERR_ACCESS_DENIED);return false;}
 bool RemoveDir(char*)override{DOS_SetError(DOSERR_ACCESS_DENIED);return false;}
 bool MakeDir(char*)override{DOS_SetError(DOSERR_ACCESS_DENIED);return false;}
 bool TestDir(char *name)override{return !*name;}
 bool FindFirst(char *dir,DOS_DTA &dta,bool=false)override{if(*dir){DOS_SetError(DOSERR_PATH_NOT_FOUND);return false;}dta.SetDirID(0);return FindNext(dta);}
 bool FindNext(DOS_DTA &dta)override{Bit8u attr;char pattern[DOS_NAMELENGTH_ASCII];dta.GetSearchParams(attr,pattern);
  for(unsigned i=dta.GetDirID();i<DB_FILE_COUNT;i++){auto &f=db_files[i];if(f.name[0]&&WildFileCmp(f.name,pattern)&&!(attr&DOS_ATTR_VOLUME)){dta.SetResult(f.name,f.size,0x5c21,0,f.readonly?DOS_ATTR_READ_ONLY:DOS_ATTR_ARCHIVE);dta.SetDirID(i+1);return true;}}
  DOS_SetError(DOSERR_NO_MORE_FILES);return false;}
 bool GetFileAttr(char *name,Bit16u *attr)override{if(!FileExists(name))return false;*attr=db_files[find(name)].readonly?DOS_ATTR_READ_ONLY:DOS_ATTR_ARCHIVE;return true;}
 bool Rename(char*,char*)override{DOS_SetError(DOSERR_ACCESS_DENIED);return false;}
 bool AllocationInfo(Bit16u *b,Bit8u *s,Bit16u *t,Bit16u *f)override{*b=512;*s=1;*t=1024;*f=0;return true;}
 bool FileExists(const char *name)override{return find(name)>=0;}
 bool FileStat(const char *name,FileStat_Block *st)override{if(!FileExists(name))return false;memset(st,0,sizeof(*st));st->size=db_files[find(name)].size;st->attr=db_files[find(name)].readonly?DOS_ATTR_READ_ONLY:DOS_ATTR_ARCHIVE;return true;}
 Bit8u GetMediaByte()override{return 0xf8;}
 bool isRemote()override{return false;}
 bool isRemovable()override{return false;}
 Bits UnMount()override{return 0;}
};
void db_mount(const uint8_t *p,unsigned n){if(!db_file_put("TEST.COM",p,n,1))E_Exit("Cannot mount executable");Drives[2]=new MemoryDrive();DOS_SetDefaultDrive(2);}
void db_stdio(DOS_PSP &){for(unsigned i=0;i<3;i++){Bit16u handle;if(!DOS_OpenFile("CON",OPEN_READWRITE,&handle)||handle!=i)E_Exit("Standard handle setup failed");}}
extern DOS_Device *Devices[DOS_DEVICES];
void db_unmount(){for(unsigned i=0;i<DOS_DEVICES;i++){delete Devices[i];Devices[i]=nullptr;}for(unsigned i=0;i<DOS_FILES;i++){delete Files[i];Files[i]=nullptr;}for(unsigned i=0;i<DOS_DRIVES;i++){delete Drives[i];Drives[i]=nullptr;}}
