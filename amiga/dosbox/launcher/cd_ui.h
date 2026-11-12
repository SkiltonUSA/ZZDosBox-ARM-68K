/* Resolve a Workbench volume to the device accepted by the existing CD player. */
#include <dos/dosextens.h>
#include "../pc/cd_host.h"
enum {ID_CD_WORKBENCH=830,ID_CD_SETTINGS};
static int cd_volume_device(const char*path,char*out,size_t cap){
 struct Process*self=(struct Process*)FindTask(NULL);APTR old=self->pr_WindowPtr;
 struct DevProc*proc;struct DosList*list,*entry;int ok=0;
 self->pr_WindowPtr=(APTR)-1;proc=GetDeviceProc((STRPTR)path,NULL);
 if(proc){
  list=LockDosList(LDF_DEVICES|LDF_READ);
  if(list){entry=list;while((entry=NextDosEntry(entry,LDF_DEVICES))){
   if(entry->dol_Task==proc->dvp_Port){
    const UBYTE*b=(const UBYTE*)BADDR(entry->dol_Name);unsigned n=b?b[0]:0;
    if(n&&n+2<=cap){memcpy(out,b+1,n);out[n]=':';out[n+1]=0;ok=1;}break;
   }
  }UnLockDosList(LDF_DEVICES|LDF_READ);}
  FreeDeviceProc(proc);
 }
 self->pr_WindowPtr=old;return ok;
}
static void cd_controls(void){unsigned n=val(cd_mode,MUIA_Cycle_Active);
 set(cd_path,MUIA_Disabled,!n);set(cd_browse,MUIA_Disabled,!n);
}
static void cd_workbench(void){char folder[DL_PATH],device[108];ULONG count;
 if(FindPort("Sixies.ARM.Debug.Owner")){note("Close DOSBox before changing the CD-ROM.");return;}
 if(selected<0){note("Select a program or DOS drive first.");return;}
 if(!browse(folder,1,0,"Choose the CD visible in Workbench"))return;
 if(!cd_volume_device(folder,device,sizeof(device))){note("Cannot resolve that mounted volume to a CD device.");return;}
 set(app,MUIA_Application_Sleep,TRUE);count=db_cd_host_open(2,device);db_cd_host_close();set(app,MUIA_Application_Sleep,FALSE);
 if(!count){note("No readable ISO9660 data CD in that volume. Audio-only discs are not supported.");return;}
 SetAttrs(cd_mode,MUIA_NoNotify,TRUE,MUIA_Cycle_Active,2,TAG_DONE);
 set(cd_path,MUIA_String_Contents,(ULONG)device);cd_controls();
 if(!store_selection()||!save()){note("CD selected; settings could not be saved.");return;}
 note("Workbench CD ready as DOS D:. Launch the selected program or terminal.");
}
