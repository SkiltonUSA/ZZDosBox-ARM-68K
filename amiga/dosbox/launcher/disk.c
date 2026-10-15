/* GPL-3.0-or-later. Explicit Workbench ownership of the ZZDosBox HDD image. */
#include <dos/dosextens.h>
#include <dos/dostags.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include <stdio.h>
#include <string.h>
#include "session.h"
#include "image_mount.h"
int main(int argc,char **argv){
 static char base[DL_PATH],tool[DL_PATH],mountfile[DL_PATH],image[DL_PATH],qt[DL_PATH*2+3],qm[DL_PATH*2+3],qi[DL_PATH*2+3],cmd[DL_PATH*5+128];
 struct DosList *devices;BPTR lock;LONG rc;int mounting,mounted,inspect,readonly,custom,unit;const char *device;
 struct Task *self=FindTask(NULL);
 ((struct Process*)self)->pr_WindowPtr=(APTR)-1;
 if((ULONG)self->tc_SPUpper-(ULONG)self->tc_SPLower<8192){puts("ZZDosDisk requires Stack 8192 or greater.");return 20;}
 if((argc!=2&&argc!=3)||(strcmp(argv[1],"mount")&&strcmp(argv[1],"eject")&&strcmp(argv[1],"inspect")&&strcmp(argv[1],"mount-ro")))return 20;
 custom=argc==3;unit=custom?6:5;device=custom?"ZZIMAGE:":"DOSHD:";
 readonly=!strcmp(argv[1],"mount-ro");mounting=readonly||!strcmp(argv[1],"mount");inspect=!strcmp(argv[1],"inspect");
 if(FindPort("Sixies.ARM.Debug.Owner")||FindPort("ZZDOS_DISK_CREATE")){puts("Close the ARM application first.");return 5;}
 if(!NameFromLock(GetProgramDir(),base,sizeof(base)))return 20;
 strcpy(tool,base);strcpy(mountfile,base);strcpy(image,base);
 if(!AddPart(tool,"Disks/DiskImageCtrl",sizeof(tool))||!AddPart(mountfile,custom?"Disks/Mounts/ZZIMAGE":"Disks/Mounts/DOSHD",sizeof(mountfile))||!AddPart(image,"Disks/DOS622.IMG",sizeof(image)))return 20;
 if(custom){if(strlen(argv[2])>=sizeof(image))return 20;strcpy(image,argv[2]);}
 if(custom&&mounting){unsigned char mbr[512];FILE*f=fopen(image,"rb");long bytes;int valid=0;
  if(f){if(!fseek(f,0,SEEK_END)&&(bytes=ftell(f))>=0&&!fseek(f,0,SEEK_SET)&&fread(mbr,1,512,f)==512)valid=di_raw_fat_image(mbr,(ULONG)bytes);fclose(f);}
  if(!valid){puts("Select a raw FAT .IMG disk, 32 to 256 MiB. VHD containers are not supported.");return 10;}
 }
 if(!dl_quote(qt,sizeof(qt),tool)||!dl_quote(qm,sizeof(qm),mountfile)||!dl_quote(qi,sizeof(qi),image))return 20;
 if(mounting&&!custom){lock=Lock(image,EXCLUSIVE_LOCK);if(!lock){puts("Hard disk is missing or already in use.");return 5;}UnLock(lock);}
 /* Address the raw unit while inhibited. DRIVE DOSHD: makes DiskImageCtrl
  * resolve the inhibited filesystem and deadlocks before we can release it. */
 /* No insertion/removal until the filesystem has flushed and inhibited. */
 devices=LockDosList(LDF_DEVICES|LDF_READ);
 if(!devices){puts("Cannot inspect device list.");return 20;}
 mounted=FindDosEntry(devices,custom?"ZZIMAGE":"DOSHD",LDF_DEVICES)!=NULL;
 UnLockDosList(LDF_DEVICES|LDF_READ);
 printf("%s device registered=%d; action=%s\n",device,mounted,argv[1]);Flush(Output());
 if(inspect)return 0;
 if(!mounted){
  if(!mounting)return 0;
  snprintf(cmd,sizeof(cmd),"Mount %s",qm);if(SystemTags((CONST_STRPTR)cmd,SYS_Asynch,FALSE,NP_StackSize,32768,TAG_DONE)!=0)return 20;
 }
 printf("Inhibiting %s filesystem\n",device);Flush(Output());
 if(!Inhibit((STRPTR)device,DOSTRUE)){puts("Cannot release Workbench disk; close its files first.");return 5;}
 if(custom&&mounting){
  /* Flush/eject the viewer's previous image before acquiring the new one.
   * Never use DRIVE while inhibited: DOS name resolution would deadlock. */
  snprintf(cmd,sizeof(cmd),"%s UNIT %d EJECT",qt,unit);
  rc=SystemTags((CONST_STRPTR)cmd,SYS_Asynch,FALSE,NP_StackSize,32768,TAG_DONE);
  if(rc){Inhibit((STRPTR)device,DOSFALSE);return rc;}
  lock=Lock(image,EXCLUSIVE_LOCK);
  if(!lock){Inhibit((STRPTR)device,DOSFALSE);puts("Image is in use; close its other mount first.");return 5;}
  UnLock(lock);
 }
 if(mounting)snprintf(cmd,sizeof(cmd),"%s UNIT %d %s INSERT %s",qt,unit,readonly?"READONLY":"",qi);
 else snprintf(cmd,sizeof(cmd),"%s UNIT %d EJECT",qt,unit);
 printf("Changing diskimage.device raw unit %d\n",unit);Flush(Output());
 rc=SystemTags((CONST_STRPTR)cmd,SYS_Asynch,FALSE,NP_StackSize,32768,TAG_DONE);
 printf("Releasing %s filesystem\n",device);Flush(Output());
 if(!Inhibit((STRPTR)device,DOSFALSE)&&!rc)rc=20;
 puts(rc?"Hard disk action failed.":mounting?"Hard disk mounted.":"Hard disk ejected.");return rc;
}
