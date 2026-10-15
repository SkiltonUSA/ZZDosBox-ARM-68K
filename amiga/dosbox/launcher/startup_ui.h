/* GPL-3.0-or-later. Workbench ownership surrounds all startup-file changes. */
#include "startup.h"
#include "../pc/cd_host.h"
static int welcome_support(const char*root){
 char source[DL_PATH],dest[DL_PATH],tmp[DL_PATH];FILE*a,*b;unsigned char data[512],check[512];size_t n;int ok=1;
 path(source,"ZZHELLO2.COM");if(!dl_join(dest,sizeof(dest),root,"ZZHELLO2.COM")||!dl_join(tmp,sizeof(tmp),root,"ZZHELLO2.TMP"))return 0;
 a=fopen(source,"rb");if(!a)return 0;
 if(exists(dest)){
  b=fopen(dest,"rb");if(!b){fclose(a);return 0;}
  while((n=fread(data,1,sizeof(data),a))>0)if(fread(check,1,n,b)!=n||memcmp(data,check,n)){ok=0;break;}
  if(ferror(a)||ferror(b)||fgetc(b)!=EOF)ok=0;fclose(a);fclose(b);return ok;
 }
 if(exists(tmp)){fclose(a);return 0;}b=fopen(tmp,"wb");if(!b){fclose(a);return 0;}
 while((n=fread(data,1,sizeof(data),a))>0)if(fwrite(data,1,n,b)!=n){ok=0;break;}
 if(ferror(a))ok=0;fclose(a);if(fclose(b))ok=0;
 if(ok)ok=Rename(tmp,dest);if(!ok)DeleteFile(tmp);return ok;
}
static int startup_disk(int mount){char helper_path[DL_PATH],quoted[DL_PATH*2+3],cmd[DL_PATH*2+40];LONG rc;
 path(helper_path,"ZZDosDisk");if(!dl_quote(quoted,sizeof(quoted),helper_path))return 0;
 snprintf(cmd,sizeof(cmd),"%s %s",quoted,mount?"mount":"eject");
 set(app,MUIA_Application_Sleep,TRUE);rc=SystemTags(cmd,SYS_Asynch,FALSE,NP_StackSize,32768,TAG_DONE);set(app,MUIA_Application_Sleep,FALSE);return rc==0;
}
static int startup_read_default(unsigned n,char *out){
 char file[DL_PATH];int ok;
 if(!val(system_choice,MUIA_Cycle_Active)){strcpy(out,n?dl_default_config:dl_default_autoexec);return 1;}
 if(val(system_choice,MUIA_Cycle_Active)==3){char image[DL_PATH];
  if(selected<0||!dl_join(image,sizeof(image),library.p[selected].drive,(char*)val(disk_image,MUIA_String_Contents))||!disk_image_control("mount",image))return 0;
  ok=dl_join(file,sizeof(file),"ZZIMAGE:",dl_start_names[n])&&dl_read_text(file,out);if(!disk_image_control("eject",image))ok=0;return ok;
 }
 if(FindPort("Sixies.ARM.Debug.Owner")||!hdd_available()||!startup_disk(1))return 0;
 ok=dl_startup_restore("DOSHD:")&&dl_join(file,sizeof(file),"DOSHD:",dl_start_names[n]);
 if(ok){if(exists(file))ok=dl_read_text(file,out);else *out=0;}
 if(!startup_disk(0))ok=0;return ok;
}
static int startup_support(void){
 char source[DL_PATH];FILE*a,*b;unsigned char data[512];size_t n;int ok=1;
 if(!exists("DOSHD:DOS/MSCDEX.EXE")){note("MSCDEX.EXE is missing from C:\\DOS.");return 0;}
 if(exists("DOSHD:ZZDOSCD.SYS"))return 1;
 path(source,"ZZDOSCD.SYS");a=fopen(source,"rb");if(!a)return 0;
 if(exists("DOSHD:ZZDCD.TMP")){fclose(a);return 0;}
 b=fopen("DOSHD:ZZDCD.TMP","wb");if(!b){fclose(a);return 0;}
 while((n=fread(data,1,sizeof(data),a))>0)if(fwrite(data,1,n,b)!=n){ok=0;break;}
 if(ferror(a))ok=0;fclose(a);if(fclose(b))ok=0;
 if(ok)ok=Rename("DOSHD:ZZDCD.TMP","DOSHD:ZZDOSCD.SYS");if(!ok)DeleteFile("DOSHD:ZZDCD.TMP");return ok;
}
static int startup_prepare(const struct dl_profile*p,int *changed){
 static char text[2][DL_TEXT],combined[DL_TEXT];char file[DL_PATH];unsigned i;int ok=1;
 *changed=0;if(!hdd_available()||!startup_disk(1)){note("Cannot mount the DOS hard disk for startup files.");return 0;}
 if(!dl_startup_restore("DOSHD:")){note("Startup recovery needed: keep ZZDSTART.TXN and its backups.");ok=0;goto done;}
 if(!welcome_support("DOSHD:")){note("Cannot install ZZHELLO2.COM; check for a conflicting file.");ok=0;goto done;}

 for(i=0;i<2;i++){
  unsigned mode=i?p->config_mode:p->edited;const char*src=i?p->configsys:p->autoexec;
  if(mode==1)strcpy(text[i],src);
  else if(mode==2){if(!dl_read_text(i?p->config_file:p->auto_file,text[i])){ok=0;note("Cannot read selected startup file.");goto done;}}
  else{dl_join(file,sizeof(file),"DOSHD:",dl_start_names[i]);if(exists(file)){if(!dl_read_text(file,text[i])){ok=0;note("Disk startup file is too large or not plain text.");goto done;}}else text[i][0]=0;}
 }
 if(p->cd_mode){
  int n;if(!startup_support()){ok=0;note("Cannot install CD driver; check ZZDOSCD.SYS and C:\\DOS\\MSCDEX.EXE.");goto done;}
  n=snprintf(combined,sizeof(combined),"%s\nLASTDRIVE=Z\nDEVICE=C:\\ZZDOSCD.SYS /D:ZZDOSCD\n",text[1]);if(n<0||n>=(int)sizeof(combined)||!dl_text_valid(combined)){ok=0;goto done;}strcpy(text[1],combined);
 }
 {int n=snprintf(combined,sizeof(combined),"@echo off\n%sC:\\ZZHELLO2.COM\n%s",p->cd_mode?"C:\\DOS\\MSCDEX.EXE /D:ZZDOSCD /L:D\n":"",text[0]);if(n<0||n>=(int)sizeof(combined)||!dl_text_valid(combined)){ok=0;goto done;}strcpy(text[0],combined);}
 ok=dl_startup_apply("DOSHD:",text[0],text[1]);*changed=ok;
 if(!ok)note("Could not prepare startup files; originals retained for recovery.");
done:if(!startup_disk(0)){note("Cannot eject DOS hard disk; session was not started.");ok=0;}return ok;
}
static int startup_finish(void){int ok;if(!hdd_available()||!startup_disk(1))return 0;ok=dl_startup_restore("DOSHD:");if(!startup_disk(0))ok=0;return ok;}
