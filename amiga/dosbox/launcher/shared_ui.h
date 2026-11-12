/* Existing AmigaOS filesystems handle USB/SD media; no raw-device writes. */
#include "shared_drive.h"
static Object *shared_path,*shared_name,*shared_browse,*shared_create,*shared_use,*shared_open;
enum {ID_SHARED_BROWSE=820,ID_SHARED_CREATE,ID_SHARED_USE,ID_SHARED_OPEN};
static int shared_directory(const char*folder){
 struct Process*self=(struct Process*)FindTask(NULL);APTR old=self->pr_WindowPtr;
 struct FileInfoBlock *fib=AllocDosObject(DOS_FIB,NULL);BPTR lock;int ok=0;
 if(!fib)return 0;self->pr_WindowPtr=(APTR)-1;lock=Lock((STRPTR)folder,ACCESS_READ);
 if(lock){ok=Examine(lock,fib)&&fib->fib_DirEntryType>0;UnLock(lock);}
 self->pr_WindowPtr=old;FreeDosObject(DOS_FIB,fib);return ok;
}
static Object* shared_page(void){return VGroup,UI_PANEL("USB / SD and shared folders"),
 Child,ColGroup(2),
  Child,fieldlabel("DOS C:"),Child,HGroup,Child,shared_path=str(DL_PATH),Child,shared_browse=ui_button("Browse..."),End,
  Child,fieldlabel("Name"),Child,shared_name=str(64),End,
 Child,HGroup,Child,shared_create=ui_button("Create DOSSHARE"),Child,shared_use=ui_button("Add DOS drive"),Child,shared_open=ui_button("Open in Workbench"),End,
 Child,label("Choose a mounted USB/SD volume or folder. Files are shared read/write.\nCopy files in Workbench between DOS sessions; keep the card connected."),End;}
static void shared_wire(void){char folder[DL_PATH];path(folder,"");
 set(shared_path,MUIA_String_Contents,(ULONG)folder);set(shared_name,MUIA_String_Contents,(ULONG)"Shared Drive");
 set(shared_path,MUIA_ShortHelp,(ULONG)"An existing AmigaOS folder becomes C: in built-in DOS.\nUSB needs Poseidon, a storage driver and the card's filesystem mounted in Workbench.\nThis does not add a drive to a booted MS-DOS/Windows image.");
 set(shared_create,MUIA_ShortHelp,(ULONG)"Create a DOSSHARE subfolder inside the selected volume or drawer. Existing files are kept.");
 set(shared_use,MUIA_ShortHelp,(ULONG)"Adds a DOS prompt under OS. Launch it to use the selected folder as C:.\nWritable media required. Use DOS-compatible filenames (8.3).\nLaunch afresh after copying files; do not remove a card during a DOS session.");
 notify(shared_browse,MUIA_Pressed,FALSE,ID_SHARED_BROWSE);notify(shared_create,MUIA_Pressed,FALSE,ID_SHARED_CREATE);
 notify(shared_use,MUIA_Pressed,FALSE,ID_SHARED_USE);notify(shared_open,MUIA_Pressed,FALSE,ID_SHARED_OPEN);
}
static void shared_action(unsigned id){char folder[DL_PATH],name[64];unsigned i;BPTR lock;static struct dl_profile candidate;
 if(FindPort("Sixies.ARM.Debug.Owner")){note("Close DOSBox before changing shared files or drives.");return;}
 if(id==ID_SHARED_BROWSE){if(browse(folder,1,0,"Choose a mounted USB/SD volume or shared folder"))set(shared_path,MUIA_String_Contents,(ULONG)folder);return;}
 snprintf(folder,sizeof(folder),"%s",(char*)val(shared_path,MUIA_String_Contents));
 if(id==ID_SHARED_CREATE){
  if(!shared_directory(folder)){note("Choose an existing parent folder or mounted volume first.");return;}
  if(!AddPart(folder,"DOSSHARE",sizeof(folder))){note("Folder path is too long.");return;}
  if(!shared_directory(folder)){lock=CreateDir(folder);if(!lock){note("Cannot create DOSSHARE. Check write protection and free space.");return;}UnLock(lock);}
  set(shared_path,MUIA_String_Contents,(ULONG)folder);note("DOSSHARE ready. Choose Add DOS drive.");return;
 }
 if(!shared_directory(folder)){note("Folder unavailable. Insert and mount the card, or choose another folder.");return;}
 if(id==ID_SHARED_OPEN){disk_open_drawer(folder);return;}
 if(id!=ID_SHARED_USE||!store_selection())return;
 snprintf(name,sizeof(name),"%s",(char*)val(shared_name,MUIA_String_Contents));
 if(!dl_shared_profile(&candidate,folder,name)){note("Enter a drive name and a valid folder.");return;}
 /* Re-select an existing prompt; never replace a game's settings or add duplicates. */
 for(i=0;i<library.count;i++)if(!library.p[i].system&&!*library.p[i].program&&library.p[i].category==DL_OS&&same_dir(library.p[i].drive,folder))break;
 if(i==library.count){if(i==DL_MAX){note("Program list is full.");return;}library.p[i]=candidate;library.count++;}
 library.last=i;refresh();
 note(save()?"DOS drive added under OS. Launch it for C:; open its folder to transfer files.":"Drive added for this session; settings could not be saved.");
}
