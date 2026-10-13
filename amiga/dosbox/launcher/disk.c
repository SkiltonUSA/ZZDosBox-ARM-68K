/* GPL-3.0-or-later. Explicit Workbench ownership of the ZZDosBox HDD image. */
#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <stdio.h>
#include <string.h>
#include "session.h"
int main(int argc,char **argv){
 static char base[DL_PATH],tool[DL_PATH],mountfile[DL_PATH],image[DL_PATH],qt[DL_PATH*2+3],qm[DL_PATH*2+3],qi[DL_PATH*2+3],cmd[DL_PATH*5+128];
 struct DosList *devices;BPTR lock;LONG rc;int mounting,mounted,inspect,readonly;
 struct Task *self=FindTask(NULL);
 if((ULONG)self->tc_SPUpper-(ULONG)self->tc_SPLower<8192){puts("ZZDosDisk requires Stack 8192 or greater.");return 20;}
 if(argc!=2||(strcmp(argv[1],"mount")&&strcmp(argv[1],"eject")&&strcmp(argv[1],"inspect")&&strcmp(argv[1],"mount-ro")))return 20;
 readonly=!strcmp(argv[1],"mount-ro");mounting=readonly||!strcmp(argv[1],"mount");inspect=!strcmp(argv[1],"inspect");
 if(FindPort("Sixies.ARM.Debug.Owner")){puts("Close the ARM application first.");return 5;}
 if(!NameFromLock(GetProgramDir(),base,sizeof(base)))return 20;
 strcpy(tool,base);strcpy(mountfile,base);strcpy(image,base);
 if(!AddPart(tool,"Disks/DiskImageCtrl",sizeof(tool))||!AddPart(mountfile,"Disks/Mounts/DOSHD",sizeof(mountfile))||!AddPart(image,"Disks/DOS622.IMG",sizeof(image)))return 20;
 if(!dl_quote(qt,sizeof(qt),tool)||!dl_quote(qm,sizeof(qm),mountfile)||!dl_quote(qi,sizeof(qi),image))return 20;
 if(mounting){lock=Lock(image,EXCLUSIVE_LOCK);if(!lock){puts("Hard disk is missing or already in use.");return 5;}UnLock(lock);}
 /* Address the raw unit while inhibited. DRIVE DOSHD: makes DiskImageCtrl
  * resolve the inhibited filesystem and deadlocks before we can release it. */
 /* No insertion/removal until the filesystem has flushed and inhibited. */
 devices=LockDosList(LDF_DEVICES|LDF_READ);
 if(!devices){puts("Cannot inspect device list.");return 20;}
 mounted=FindDosEntry(devices,"DOSHD",LDF_DEVICES)!=NULL;
 UnLockDosList(LDF_DEVICES|LDF_READ);
 printf("DOSHD device registered=%d; action=%s\n",mounted,argv[1]);Flush(Output());
 if(inspect)return 0;
 if(!mounted){
  if(!mounting)return 0;
  snprintf(cmd,sizeof(cmd),"Mount %s",qm);if(SystemTags((CONST_STRPTR)cmd,SYS_Asynch,FALSE,NP_StackSize,32768,TAG_DONE)!=0)return 20;
 }
 puts("Inhibiting DOSHD filesystem");Flush(Output());
 if(!Inhibit("DOSHD:",DOSTRUE)){puts("Cannot release Workbench disk; close its files first.");return 5;}
 if(mounting)snprintf(cmd,sizeof(cmd),"%s UNIT 5 %s INSERT %s",qt,readonly?"READONLY":"",qi);
 else snprintf(cmd,sizeof(cmd),"%s UNIT 5 EJECT",qt);
 puts("Changing diskimage.device raw unit 5");Flush(Output());
 rc=SystemTags((CONST_STRPTR)cmd,SYS_Asynch,FALSE,NP_StackSize,32768,TAG_DONE);
 puts("Releasing DOSHD filesystem");Flush(Output());
 if(!Inhibit("DOSHD:",DOSFALSE)&&!rc)rc=20;
 puts(rc?"Hard disk action failed.":mounting?"Hard disk mounted.":"Hard disk ejected.");return rc;
}
