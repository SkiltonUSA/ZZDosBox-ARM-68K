/* Original recovery tests: preserve user files across partial replacement. */
#include <assert.h>
#include <unistd.h>
#include <sys/stat.h>
#include "startup.h"
static char root[DL_PATH],p[DL_PATH],q[DL_PATH];
static void put(const char*name,const char*text){assert(dl_join(p,sizeof(p),root,name));FILE*f=fopen(p,"wb");assert(f);assert(fwrite(text,1,strlen(text),f)==strlen(text));assert(!fclose(f));}
static void check(const char*name,const char*expected){char data[DL_TEXT];assert(dl_join(p,sizeof(p),root,name));assert(dl_read_text(p,data));assert(!strcmp(data,expected));}
static void remove_file(const char*name){assert(dl_join(p,sizeof(p),root,name));assert(!remove(p));}
static void move(const char*a,const char*b){assert(dl_join(p,sizeof(p),root,a)&&dl_join(q,sizeof(q),root,b));assert(!rename(p,q));}
int main(void){
 strcpy(root,"/tmp/zzdos-startup-XXXXXX");assert(mkdtemp(root));
 for(unsigned originals=0;originals<4;originals++){
  if(originals&1)put("AUTOEXEC.BAT","@echo original\r\n");
  if(originals&2)put("CONFIG.SYS","FILES=30\r\n");
  assert(dl_startup_apply(root,"echo edited\n","FILES=40\n"));
  check("AUTOEXEC.BAT","echo edited\r\n");check("CONFIG.SYS","FILES=40\r\n");
  assert(dl_startup_restore(root)&&dl_startup_restore(root));
  if(originals&1){check("AUTOEXEC.BAT","@echo original\r\n");remove_file("AUTOEXEC.BAT");}
  else{dl_join(p,sizeof(p),root,"AUTOEXEC.BAT");assert(!dl_file_exists(p));}
  if(originals&2){check("CONFIG.SYS","FILES=30\r\n");remove_file("CONFIG.SYS");}
  else{dl_join(p,sizeof(p),root,"CONFIG.SYS");assert(!dl_file_exists(p));}
 }
 put("AUTOEXEC.BAT","original\n");put("CONFIG.SYS","config\n");
 /* Crash after first original rename, before replacement or second rename. */
 put("ZZDSTART.TXN","ZZSTART1\n3\n");put("ZZDAUTO.TMP","new\n");put("ZZDCONF.TMP","new config\n");move("AUTOEXEC.BAT","ZZDAUTO.BAK");
 assert(dl_startup_restore(root));check("AUTOEXEC.BAT","original\n");check("CONFIG.SYS","config\n");
 /* An unrelated backup or malformed marker must never be consumed. */
 put("ZZDAUTO.BAK","unrelated\n");assert(!dl_startup_apply(root,"new","new"));check("ZZDAUTO.BAK","unrelated\n");remove_file("ZZDAUTO.BAK");
 put("ZZDSTART.TXN","invalid\n");assert(!dl_startup_restore(root));check("AUTOEXEC.BAT","original\n");remove_file("ZZDSTART.TXN");
 /* Recovery can itself be interrupted after one backup was restored. */
 assert(dl_startup_apply(root,"new","new"));remove_file("AUTOEXEC.BAT");move("ZZDAUTO.BAK","AUTOEXEC.BAT");assert(dl_startup_restore(root));check("AUTOEXEC.BAT","original\n");check("CONFIG.SYS","config\n");
 remove_file("AUTOEXEC.BAT");remove_file("CONFIG.SYS");assert(!rmdir(root));puts("PASS startup replacements, original/absent files, partial recovery and collision refusal");
}
