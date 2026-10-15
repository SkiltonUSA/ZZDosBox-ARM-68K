/* GPL-3.0-or-later. Delete an unshared installation drawer, never an OS image. */
#include <dos/dosextens.h>
struct delete_item {struct delete_item *next;char path[DL_PATH];};
struct delete_scan {struct FileInfoBlock fib;char child[DL_PATH];};
static int delete_canonical(const char *name,char *out){
 struct Process *self=(struct Process*)FindTask(NULL);APTR old=self->pr_WindowPtr;BPTR lock;int ok=0;
 self->pr_WindowPtr=(APTR)-1;lock=Lock((STRPTR)name,ACCESS_READ);
 if(lock){ok=NameFromLock(lock,out,DL_PATH);UnLock(lock);}self->pr_WindowPtr=old;return ok;
}
/* Canonical Amiga names use ':' at the volume and '/' between drawers. */
static int delete_contains(const char *parent,const char *child){
 size_t n=strlen(parent);
 return !strnicmp(parent,child,n)&&(!child[n]||child[n]=='/'||(n&&parent[n-1]==':'));
}
static int delete_plan(const char *name,struct delete_item ***tail,unsigned depth){
 struct delete_scan *scan;struct delete_item *item;BPTR lock;int ok=0;LONG type;
 if(depth>64)return 0;
 scan=AllocVec(sizeof(*scan),MEMF_PUBLIC|MEMF_CLEAR);if(!scan)return 0;
 lock=Lock((STRPTR)name,ACCESS_READ);if(!lock)goto done;
 if(!Examine(lock,&scan->fib)||(scan->fib.fib_Protection&FIBF_DELETE))goto unlocked;
 type=scan->fib.fib_DirEntryType;
 if(type!=ST_FILE&&type!=ST_USERDIR)goto unlocked;
 if(type==ST_USERDIR){
  while(ExNext(lock,&scan->fib)){
   /* Do not follow links out of the confirmed installation. */
   if(scan->fib.fib_DirEntryType!=ST_FILE&&scan->fib.fib_DirEntryType!=ST_USERDIR)goto unlocked;
   strcpy(scan->child,name);
   if(!AddPart(scan->child,scan->fib.fib_FileName,DL_PATH)||!delete_plan(scan->child,tail,depth+1))goto unlocked;
  }
  if(IoErr()!=ERROR_NO_MORE_ENTRIES)goto unlocked;
 }
 item=AllocVec(sizeof(*item),MEMF_PUBLIC|MEMF_CLEAR);if(!item)goto unlocked;
 strcpy(item->path,name);**tail=item;*tail=&item->next;ok=1;
unlocked:UnLock(lock);
done:FreeVec(scan);return ok;
}
static void delete_free(struct delete_item *item){while(item){struct delete_item *next=item->next;FreeVec(item);item=next;}}
static int delete_target(char *target){
 struct dl_profile *p=&library.p[selected];char other[DL_PATH],candidate[DL_PATH];
 BPTR lock,parent;struct FileInfoBlock fib;unsigned i;int valid;
 if(p->category==DL_OS){note("OS entries and disks are protected.");return 0;}
 if(p->system){note("OS/shared image protected. Use the application's own uninstaller.");return 0;}
 if(!delete_canonical(p->drive,target)){note("Installation drawer unavailable; nothing deleted.");return 0;}
 lock=Lock(target,ACCESS_READ);if(!lock)return 0;
 parent=ParentDir(lock);valid=parent&&Examine(lock,&fib)&&fib.fib_DirEntryType==ST_USERDIR;
 if(parent)UnLock(parent);UnLock(lock);
 if(!valid){note("A volume root cannot be deleted.");return 0;}
 if(!delete_canonical(appdir,other)||delete_contains(target,other)){note("The launcher installation is protected.");return 0;}
 for(i=0;i<library.count;i++){
  if((int)i==selected)continue;
  p=&library.p[i];
  if(p->system==1)path(candidate,"Disks/DOS622.IMG");
  else if(p->system>=2){strcpy(candidate,p->drive);if(!AddPart(candidate,p->image,sizeof(candidate)))return 0;}
  else strcpy(candidate,p->drive);
  if(delete_canonical(candidate,other)&&(delete_contains(target,other)||delete_contains(other,target))){
   note("Shared installation protected. Use the application's own uninstaller.");return 0;
  }
 }
 /* Guard obvious DOS/Windows/Amiga system drawers even if no OS profile exists. */
 {static const char *markers[]={"IO.SYS","MSDOS.SYS","WINDOWS/WIN.COM","WIN.COM","S/Startup-Sequence"};
  for(i=0;i<sizeof(markers)/sizeof(markers[0]);i++){
   strcpy(candidate,target);if(!AddPart(candidate,(STRPTR)markers[i],sizeof(candidate)))return 0;
   if(exists(candidate)){note("This drawer contains OS files and is protected.");return 0;}
  }
 }
 return 1;
}
static void delete_profile(void){
 unsigned n;APTR args[2];char target[DL_PATH],picture[DL_PATH],extra[DL_PATH+4];struct delete_item *items=NULL,**tail=&items,*item;int ok=1;
 if(selected<0||!store_selection())return;
 if(FindPort("Sixies.ARM.Debug.Owner")||FindPort("ZZDOS_DISK_CREATE")){note("Close DOSBox before deleting files.");return;}
 if(!delete_target(target))return;
 if(!snapshot_path(&library.p[selected],picture)){note("Screenshot path is too long; nothing deleted.");return;}
 args[0]=library.p[selected].title;args[1]=target;
 if(MUI_RequestA(app,win,0,"Delete installation","_Yes|*_No",
  "Delete \"%s\" and its files?\n%s\n\nSaved games in this folder will also be deleted.\nThis cannot be undone.",args)!=1)return;
 set(app,MUIA_Application_Sleep,TRUE);
 if(!delete_plan(target,&tail,0)){delete_free(items);set(app,MUIA_Application_Sleep,FALSE);note("Cannot inspect every file (in use, protected, or linked). Nothing deleted.");return;}
 {unsigned i;static const char *suffix[]={"",".tmp",".bak"};
  for(i=0;i<3;i++){snprintf(extra,sizeof(extra),"%s%s",picture,suffix[i]);
   if(exists(extra)&&!delete_plan(extra,&tail,0)){delete_free(items);set(app,MUIA_Application_Sleep,FALSE);note("Screenshot is protected; nothing deleted.");return;}
  }
 }
 for(item=items;item;item=item->next)if(!DeleteFile(item->path)){ok=0;break;}
 delete_free(items);set(app,MUIA_Application_Sleep,FALSE);
 if(!ok){note("Deletion incomplete. Entry retained; check the installation drawer.");return;}
 n=selected;memmove(&library.p[n],&library.p[n+1],(--library.count-n)*sizeof(library.p[0]));
 library.last=n<library.count?n:0;selected=-1;refresh();
 note(save()?"Installation deleted.":"Files deleted; save the updated library with Save settings.");
}
