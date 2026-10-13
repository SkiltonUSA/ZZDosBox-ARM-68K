/* GPL-3.0-or-later. Recoverable, session-only replacement of DOS startup files. */
#ifndef DL_STARTUP_H
#define DL_STARTUP_H
#include "model.h"
static const char *dl_start_names[]={"AUTOEXEC.BAT","CONFIG.SYS"};
static const char *dl_start_backup[]={"ZZDAUTO.BAK","ZZDCONF.BAK"};
static const char *dl_start_temp[]={"ZZDAUTO.TMP","ZZDCONF.TMP"};
static int dl_join(char *out,size_t cap,const char *root,const char *name){size_t n=strlen(root);int r=snprintf(out,cap,"%s%s%s",root,n&&root[n-1]!=':'&&root[n-1]!='/'?"/":"",name);return r>=0&&(size_t)r<cap;}
static int dl_file_exists(const char *p){FILE*f=fopen(p,"rb");if(!f)return 0;fclose(f);return 1;}
static int dl_read_text(const char *path,char *text){FILE*f=fopen(path,"rb");size_t n;int ok;if(!f)return 0;n=fread(text,1,DL_TEXT-1,f);ok=fgetc(f)==EOF&&!ferror(f);fclose(f);text[n]=0;return ok&&!memchr(text,0,n)&&dl_text_valid(text);}
static int dl_write_text(const char *path,const char *text){FILE*f=fopen(path,"wb");int ok;if(!f)return 0;ok=dl_dos_text(f,text);return fclose(f)==0&&ok;}
static int dl_startup_restore(const char *root){
 char marker[DL_PATH],path[DL_PATH],backup[DL_PATH],temp[DL_PATH],line[64],extra;FILE*f;unsigned mask,i;int ok=1;
 if(!dl_join(marker,sizeof(marker),root,"ZZDSTART.TXN"))return 0;
 if(!dl_file_exists(marker))return 1;
 f=fopen(marker,"r");if(!f)return 0;
 if(!dl_line(f,line,sizeof(line))||strcmp(line,"ZZSTART1")||!dl_line(f,line,sizeof(line))||sscanf(line,"%u %c",&mask,&extra)!=1||mask>3||fgetc(f)!=EOF){fclose(f);return 0;}fclose(f);
 for(i=0;i<2;i++){
  if(!dl_join(path,sizeof(path),root,dl_start_names[i])||!dl_join(backup,sizeof(backup),root,dl_start_backup[i])||!dl_join(temp,sizeof(temp),root,dl_start_temp[i]))return 0;
  if(mask&(1u<<i)){
   if(dl_file_exists(backup)){if(dl_file_exists(path)&&remove(path))ok=0;else if(rename(backup,path))ok=0;}
   else if(!dl_file_exists(path))ok=0;
  }else if(dl_file_exists(path)&&remove(path))ok=0;
  if(dl_file_exists(temp)&&remove(temp))ok=0;
 }
 return ok&&remove(marker)==0;
}
static int dl_startup_apply(const char *root,const char *autoexec,const char *configsys){
 const char*text[]={autoexec,configsys};char marker[DL_PATH],path[DL_PATH],backup[DL_PATH],temp[DL_PATH];unsigned mask=0,i;FILE*f;int ok;
 if(!dl_text_valid(autoexec)||!dl_text_valid(configsys)||!dl_startup_restore(root)||!dl_join(marker,sizeof(marker),root,"ZZDSTART.TXN"))return 0;
 for(i=0;i<2;i++){
  if(!dl_join(path,sizeof(path),root,dl_start_names[i])||!dl_join(backup,sizeof(backup),root,dl_start_backup[i])||!dl_join(temp,sizeof(temp),root,dl_start_temp[i]))return 0;
  if(dl_file_exists(backup)||dl_file_exists(temp))return 0;
  if(dl_file_exists(path))mask|=1u<<i;
 }
 /* Stage both complete replacements before renaming any original. */
 for(i=0;i<2;i++){
  dl_join(temp,sizeof(temp),root,dl_start_temp[i]);
  if(!dl_write_text(temp,text[i])){remove(temp);if(i){dl_join(temp,sizeof(temp),root,dl_start_temp[0]);remove(temp);}return 0;}
 }
 f=fopen(marker,"w");if(!f)goto unstage;ok=fprintf(f,"ZZSTART1\n%u\n",mask)>0;if(fclose(f))ok=0;if(!ok){remove(marker);goto unstage;}
 for(i=0;i<2;i++){
  dl_join(path,sizeof(path),root,dl_start_names[i]);dl_join(backup,sizeof(backup),root,dl_start_backup[i]);dl_join(temp,sizeof(temp),root,dl_start_temp[i]);
  if(((mask&(1u<<i))&&rename(path,backup))||rename(temp,path)){dl_startup_restore(root);return 0;}
 }
 return 1;
unstage:
 for(i=0;i<2;i++){dl_join(temp,sizeof(temp),root,dl_start_temp[i]);remove(temp);}return 0;
}
#endif
