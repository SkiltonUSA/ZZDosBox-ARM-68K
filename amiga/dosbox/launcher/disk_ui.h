/* GPL-3.0-or-later. Workbench browsing of installed raw PC disk images. */
#include <workbench/workbench.h>
#include "startup.h"
#include <dos/dosextens.h>
static Object *disk_choice,*disk_path,*disk_browse,*disk_open,*disk_mount,*disk_eject,*disk_files;
static Object *disk_size,*disk_create,*disk_os;
static const char *disk_os_all[]={"DOSBox DOS 5","MS-DOS 6.22 (installed)",NULL};
static const char *disk_os_builtin[]={"DOSBox DOS 5",NULL};
static const char *disk_sizes[]={"32 MB","64 MB","128 MB","256 MB",NULL};
static char disk_paths[DL_MAX+1][DL_PATH],disk_labels[DL_MAX+1][64];
static const char *disk_names[DL_MAX+3];static unsigned disk_count;
static void disk_choices_init(void){unsigned i,j;char file[DL_PATH];
 disk_count=1;path(disk_paths[0],"Disks/DOS622.IMG");strcpy(disk_labels[0],"MS-DOS 6.22");disk_names[0]=disk_labels[0];
 for(i=0;i<library.count;i++){struct dl_profile*p=&library.p[i];if(p->system<2||!dl_join(file,sizeof(file),p->drive,p->image))continue;
  for(j=0;j<disk_count;j++)if(!strcmp(file,disk_paths[j]))break;if(j<disk_count)continue;
  strcpy(disk_paths[disk_count],file);snprintf(disk_labels[disk_count],64,"%s",p->title);disk_names[disk_count]=disk_labels[disk_count];disk_count++;
 }
 disk_names[disk_count]="Other image...";disk_names[disk_count+1]=NULL;
}
/* A missing/ejected volume must report an error, never an Insert Disk requester. */
static int disk_exists(const char*name){struct Process*self=(struct Process*)FindTask(NULL);APTR old=self->pr_WindowPtr;int ok;
 self->pr_WindowPtr=(APTR)-1;ok=exists(name);self->pr_WindowPtr=old;return ok;
}
static int disk_open_drawer(const char*name){
 struct TagItem tags[]={{WBOPENA_Show,DDFLAGS_SHOWALL},{WBOPENA_ViewBy,DDVM_BYNAME},{TAG_DONE,0}};char err[160];
 if(!disk_exists(name)){note("Drawer unavailable; mount the disk first.");return 0;}
 if(!OpenWorkbenchObjectA((STRPTR)name,WorkbenchBase->lib_Version>=45?tags:NULL)){snprintf(err,sizeof(err),"Workbench could not open the drawer (error %ld).",(long)IoErr());note(err);return 0;}
 note("Opened in Workbench.");return 1;
}
static Object* disk_image_page(void){return VGroup,
 Child,ColGroup(2),UI_PANEL("Hard disk image"),
  Child,fieldlabel("Image"),Child,disk_choice=cycle(disk_names),
  Child,fieldlabel("File"),Child,HGroup,Child,disk_path=str(DL_PATH),Child,disk_browse=ui_button("Browse..."),End,End,
 Child,HGroup,Child,disk_mount=ui_button("Mount"),Child,disk_open=ui_button("Open contents"),Child,disk_eject=ui_button("Eject"),Child,disk_files=ui_button("Show image files"),End,
 Child,VGroup,UI_PANEL("New hard disk"),
  Child,ColGroup(2),Child,fieldlabel("DOS"),Child,disk_os=cycle(exists(disk_paths[0])?disk_os_all:disk_os_builtin),
  Child,fieldlabel("Size"),Child,HGroup,Child,disk_size=cycle(disk_sizes),Child,disk_create=ui_button("Create hard disk..."),End,End,
  Child,label("Raw .IMG  |  Added to OS  |  DOSBox DOS 5 needs no install disks"),End,
 End;}
static void disk_ui_wire(void){
 notify(disk_create,MUIA_Pressed,FALSE,ID_DISK_CREATE);
 set(disk_size,MUIA_Cycle_Active,1);
 set(disk_path,MUIA_String_Contents,(ULONG)disk_paths[0]);
 set(disk_path,MUIA_ShortHelp,(ULONG)"Raw FAT .IMG disks, 32-256 MB. Eject before running DOSBox.");
 notify(disk_choice,MUIA_Cycle_Active,MUIV_EveryTime,ID_DISK_SELECT);notify(disk_browse,MUIA_Pressed,FALSE,ID_DISK_BROWSE);
 notify(disk_mount,MUIA_Pressed,FALSE,ID_MOUNT_HD);notify(disk_eject,MUIA_Pressed,FALSE,ID_EJECT_HD);notify(disk_open,MUIA_Pressed,FALSE,ID_OPEN_HD);notify(disk_files,MUIA_Pressed,FALSE,ID_DISKS);
}
static int disk_image_control(const char*action,const char*image){
 char tool[DL_PATH],qt[DL_PATH*2+3],qi[DL_PATH*2+3],cmd[DL_PATH*4+80];LONG rc;
 if(FindPort("Sixies.ARM.Debug.Owner")||FindPort("ZZDOS_DISK_CREATE"))return 0;
 path(tool,"ZZDosDisk");if(!dl_quote(qt,sizeof(qt),tool)||!dl_quote(qi,sizeof(qi),image))return 0;
 snprintf(cmd,sizeof(cmd),"%s %s %s",qt,action,qi);set(app,MUIA_Application_Sleep,TRUE);
 rc=SystemTags(cmd,SYS_Asynch,FALSE,NP_StackSize,32768,TAG_DONE);set(app,MUIA_Application_Sleep,FALSE);return rc==0;
}
static void disk_create_image(void){
 static char dest[DL_PATH],tool[DL_PATH],source[DL_PATH],qt[DL_PATH*2+3],qd[DL_PATH*2+3],qs[DL_PATH*2+3],cmd[DL_PATH*6+128];
 struct dl_profile*p;unsigned mb=32u<<val(disk_size,MUIA_Cycle_Active),i,msdos=val(disk_os,MUIA_Cycle_Active);LONG rc;BPTR console;
 if(FindPort("Sixies.ARM.Debug.Owner")||FindPort("ZZDOS_DISK_CREATE")){note("Close DOSBox before creating a disk.");return;}
 if(library.count==DL_MAX){note("Program list is full.");return;}
 if(!store_selection()||!browse(dest,0,1,"New hard disk - choose an 8.3 name ending .IMG"))return;
 {char*n=FilePart(dest);for(i=0;n[i];i++)if(n[i]>='a'&&n[i]<='z')n[i]-=32;
 if(!dl_image_valid(n)||strlen(n)<5||strcmp(n+strlen(n)-4,".IMG")){note("Use a short image name, such as DOSNEW.IMG (up to 8 letters).");return;}}
 if(disk_exists(dest)){note("That file already exists. Choose a new name.");return;}
 path(source,"Disks/DOS622.IMG");path(tool,"ZZDosCreate");if((msdos&&!disk_exists(source))||!disk_exists(tool)){note("Disk creation helper or selected DOS source is missing.");return;}
 if(!dl_quote(qt,sizeof(qt),tool)||!dl_quote(qd,sizeof(qd),dest)||!dl_quote(qs,sizeof(qs),source))return;
 snprintf(cmd,sizeof(cmd),"%s %s %u%s%s%s",qt,qd,mb,msdos?" ":"",msdos?qs:"","");
 console=Open("CON:180/150/620/240/Creating hard disk - Ctrl-C cancels/AUTO/CLOSE",MODE_NEWFILE);
 if(!console){note("Cannot open progress window.");rc=20;}else{
  note("Creating hard disk...");set(app,MUIA_Application_Sleep,TRUE);
  rc=SystemTags(cmd,SYS_Asynch,FALSE,SYS_Input,console,SYS_Output,console,NP_StackSize,65536,TAG_DONE);
  Close(console);set(app,MUIA_Application_Sleep,FALSE);
 }
 if(rc){note("Disk not created. Check free space and the selected DOS source.");return;}
 p=&library.p[library.count];dl_profile_default(p);snprintf(p->title,sizeof(p->title),"%s - %s",msdos?"MS-DOS 6.22":"DOSBox DOS 5",FilePart(dest));strcpy(p->image,FilePart(dest));strcpy(p->drive,dest);*PathPart(p->drive)=0;p->system=msdos?2:3;p->category=DL_OS;p->memory=8;p->display=0;
 library.last=library.count++;refresh();
 disk_choices_init();SetAttrs(disk_choice,MUIA_NoNotify,TRUE,MUIA_Cycle_Entries,(ULONG)disk_names,TAG_DONE);
 for(i=0;i<disk_count;i++)if(!strcmp(disk_paths[i],dest))break;
 SetAttrs(disk_choice,MUIA_NoNotify,TRUE,MUIA_Cycle_Active,i<disk_count?i:disk_count,TAG_DONE);set(disk_path,MUIA_String_Contents,(ULONG)dest);
 note(save()?"Hard disk ready. Launch it from OS, or open its contents here.":"Disk created; profile could not be saved. Use Save settings.");
}
static void disk_action(unsigned id){
 char image[DL_PATH],tool[DL_PATH],qt[DL_PATH*2+3],qi[DL_PATH*2+3],cmd[DL_PATH*4+64];const char*root;LONG rc;int normal;
 if(id==ID_DISK_CREATE){disk_create_image();return;}
 if(id==ID_DISK_SELECT){unsigned n=val(disk_choice,MUIA_Cycle_Active);if(n<disk_count){set(disk_path,MUIA_String_Contents,(ULONG)disk_paths[n]);return;}id=ID_DISK_BROWSE;}
 if(id==ID_DISK_BROWSE){if(browse(image,0,0,"Choose a raw hard disk image")){set(disk_path,MUIA_String_Contents,(ULONG)image);SetAttrs(disk_choice,MUIA_NoNotify,TRUE,MUIA_Cycle_Active,disk_count,TAG_DONE);}return;}
 snprintf(image,sizeof(image),"%s",(char*)val(disk_path,MUIA_String_Contents));if(!*image){note("Choose a disk image.");return;}
 if(id==ID_DISKS){*PathPart(image)=0;disk_open_drawer(image);return;}
 if(FindPort("Sixies.ARM.Debug.Owner")){note("Close DOSBox before mounting or ejecting an image.");return;}
 {struct Process*self=(struct Process*)FindTask(NULL);APTR old=self->pr_WindowPtr;self->pr_WindowPtr=(APTR)-1;normal=same_dir(image,disk_paths[0]);self->pr_WindowPtr=old;}root=normal?"DOSHD:":"ZZIMAGE:";
 if(normal&&id!=ID_EJECT_HD&&disk_exists(root)){if(id==ID_OPEN_HD)disk_open_drawer(root);else note("MS-DOS disk is already mounted.");return;}
 path(tool,"ZZDosDisk");if(!dl_quote(qt,sizeof(qt),tool)||!dl_quote(qi,sizeof(qi),image))return;
 snprintf(cmd,sizeof(cmd),"%s %s%s%s",qt,id==ID_EJECT_HD?"eject":"mount",normal?"":" ",normal?"":qi);
 set(app,MUIA_Application_Sleep,TRUE);rc=SystemTags(cmd,SYS_Asynch,FALSE,NP_StackSize,32768,TAG_DONE);set(app,MUIA_Application_Sleep,FALSE);
 if(rc){note(rc==10?"Use a raw FAT .IMG disk (32-256 MB), not a VHD container.":"Disk unavailable; close DOSBox and any open disk files.");return;}
 if(id==ID_OPEN_HD)disk_open_drawer(root);else note(id==ID_EJECT_HD?"Disk ejected.":"Disk mounted in Workbench.");
}
