/* Test-only POSIX implementation of the AmigaDOS calls used by fs_host.c. */
#include "platform.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <sys/file.h>
#include <fcntl.h>
struct mock_lock {char path[1024];DIR *listing;FILE *file;};
static LONG last_error;static BPTR current;
LONG IoErr(void){return last_error;}
static void pathof(char *out,const char *name){
 if(*name=='/')snprintf(out,1200,"%s",name);
 else if(current)snprintf(out,1200,"%s/%s",((struct mock_lock*)current)->path,name);
 else {char cwd[1024];assert(getcwd(cwd,sizeof(cwd)));snprintf(out,1200,"%s/%s",cwd,name);}
}
BPTR Lock(STRPTR name,LONG mode){char path[1200];struct stat st;struct mock_lock*p;
 pathof(path,name);if(lstat(path,&st)||(!S_ISDIR(st.st_mode)&&!S_ISREG(st.st_mode))){last_error=ERROR_OBJECT_NOT_FOUND;return 0;}
 if(strlen(path)>=1024)return 0;p=calloc(1,sizeof(*p));assert(p);strcpy(p->path,path);
 if(S_ISREG(st.st_mode)){p->file=fopen(path,"r+b");if(!p->file||flock(fileno(p->file),(mode==EXCLUSIVE_LOCK?LOCK_EX:LOCK_SH)|LOCK_NB)){if(p->file)fclose(p->file);free(p);return 0;}}
 return (BPTR)p;
}
void UnLock(BPTR h){struct mock_lock*p=(struct mock_lock*)h;if(p->listing)closedir(p->listing);if(p->file)fclose(p->file);free(p);}
LONG Examine(BPTR h,struct FileInfoBlock*f){struct mock_lock*p=(struct mock_lock*)h;memset(f,0,sizeof(*f));
 if(p->file){struct stat st;assert(!fstat(fileno(p->file),&st));f->fib_DirEntryType=ST_FILE;f->fib_Size=st.st_size;return 1;}
 if(p->listing)closedir(p->listing);p->listing=opendir(p->path);f->fib_DirEntryType=ST_USERDIR;return !!p->listing;
}
BPTR OpenFromLock(BPTR h){struct mock_lock*p=(struct mock_lock*)h;if(!p->file)return 0;FILE*f=p->file;p->file=NULL;UnLock(h);return (BPTR)f;}
LONG Flush(BPTR h){return !fflush((FILE*)h);}
LONG ExNext(BPTR h,struct FileInfoBlock*f){struct mock_lock*p=(struct mock_lock*)h;struct dirent*e=readdir(p->listing);struct stat st;char path[1200];if(!e){last_error=ERROR_NO_MORE_ENTRIES;return 0;}snprintf(path,sizeof(path),"%s/%s",p->path,e->d_name);assert(!lstat(path,&st));memset(f,0,sizeof(*f));snprintf(f->fib_FileName,sizeof(f->fib_FileName),"%s",e->d_name);f->fib_Size=st.st_size;f->fib_DirEntryType=S_ISREG(st.st_mode)?ST_FILE:S_ISDIR(st.st_mode)?ST_USERDIR:3;f->fib_Date.ds_Days=17817;return 1;}
BPTR CurrentDir(BPTR h){BPTR old=current;current=h;return old;}
BPTR CreateDir(STRPTR name){char p[1200];pathof(p,name);if(mkdir(p,0700))return 0;return Lock(p,ACCESS_READ);}
LONG NameFromLock(BPTR h,STRPTR out,LONG size){const char*p=((struct mock_lock*)h)->path;if(strlen(p)>=(size_t)size)return 0;strcpy(out,p);return 1;}
LONG AddPart(STRPTR out,STRPTR name,LONG size){if(strlen(out)+strlen(name)+2>(size_t)size)return 0;strcat(out,"/");strcat(out,name);return 1;}
BPTR Open(STRPTR name,LONG mode){char path[1200];pathof(path,name);
 int fd=open(path,O_RDWR|(mode==MODE_NEWFILE?O_CREAT:0),0600);if(fd<0){last_error=ERROR_OBJECT_NOT_FOUND;return 0;}
 if(flock(fd,(mode==MODE_NEWFILE?LOCK_EX:LOCK_SH)|LOCK_NB)){close(fd);return 0;}
 if(mode==MODE_NEWFILE&&ftruncate(fd,0)){close(fd);return 0;}FILE*f=fdopen(fd,"r+b");if(!f)close(fd);return (BPTR)f;
}
LONG Close(BPTR h){return fclose((FILE*)h)==0;}
static unsigned mock_seek_calls,mock_short_read;
LONG Read(BPTR h,void*p,ULONG n){if(mock_short_read){mock_short_read=0;n/=2;}return fread(p,1,n,(FILE*)h);}
LONG Write(BPTR h,const void*p,ULONG n){return fwrite(p,1,n,(FILE*)h);}
LONG Seek(BPTR h,LONG pos,LONG whence){mock_seek_calls++;FILE*f=(FILE*)h;long old=ftell(f);return fseek(f,pos,whence==OFFSET_BEGINNING?SEEK_SET:whence==OFFSET_CURRENT?SEEK_CUR:SEEK_END)?-1:old;}
LONG SetFileSize(BPTR h,LONG pos,LONG whence){FILE*f=(FILE*)h;assert(whence==OFFSET_CURRENT);LONG size=ftell(f)+pos;fflush(f);return ftruncate(fileno(f),size)?-1:size;}
LONG DeleteFile(STRPTR n){char p[1200];struct stat st;pathof(p,n);if(lstat(p,&st))return 0;return !(S_ISDIR(st.st_mode)?rmdir(p):unlink(p));}
LONG Rename(STRPTR a,STRPTR b){char x[1200],y[1200];pathof(x,a);pathof(y,b);return !rename(x,y);}
LONG Info(BPTR h,struct InfoData*i){(void)h;i->id_NumBlocks=100000;i->id_NumBlocksUsed=40000;i->id_BytesPerBlock=512;return 1;}
