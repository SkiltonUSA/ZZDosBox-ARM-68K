/* GPL-3.0-or-later. Create a DOSBox data disk or bootable MS-DOS raw disk on AmigaOS. */
#include <dos/dosextens.h>
#include <exec/memory.h>
#include <proto/dos.h>
#include <proto/exec.h>
#include "image_create.h"
void __chkabort(void){} /* Progress callback handles Ctrl-C with cleanup. */
static unsigned last_percent=999;
static int progress(unsigned percent,const char*phase,void*opaque){(void)opaque;
 if(SetSignal(0,SIGBREAKF_CTRL_C)&SIGBREAKF_CTRL_C)return 0;
 if(percent/5!=last_percent/5){printf("%3u%%  %s\n",percent,phase);fflush(stdout);last_percent=percent;}
 return 1;
}
int main(int argc,char**argv){FILE*out;int rc=20;unsigned mb;char*end;const char*error;BPTR parent;struct InfoData*info;struct MsgPort*owner;BPTR worklock=0;static char workdir[512],tempfile[512];struct Task*self=FindTask(NULL);
 if((ULONG)self->tc_SPUpper-(ULONG)self->tc_SPLower<32768){puts("ZZDosCreate requires Stack 32768.");return 20;}
 ((struct Process*)self)->pr_WindowPtr=(APTR)-1;
 if(argc!=3&&argc!=4){puts("ZZDosCreate destination.IMG 32|64|128|256 [source.IMG]");return 20;}
 mb=strtoul(argv[2],&end,10);if(*end||(mb!=32&&mb!=64&&mb!=128&&mb!=256))return 20;
 if(FindPort("Sixies.ARM.Debug.Owner")||FindPort("ZZDOS_DISK_CREATE")){puts("Close the ARM application or wait for disk creation.");return 5;}
 owner=CreateMsgPort();if(!owner)return 20;owner->mp_Node.ln_Name="ZZDOS_DISK_CREATE";AddPort(owner);
 /* Native DOS operations protect existing files; libnix O_EXCL is not
  * reliable on the target. Build in a newly-created private directory and
  * publish with Rename(), which refuses an existing destination. */
 {static char dir[512];if(strlen(argv[1])>=sizeof(dir))goto done;strcpy(dir,argv[1]);*PathPart(dir)=0;parent=Lock(*dir?dir:"",SHARED_LOCK);}
 if(!parent){puts("Destination drawer is unavailable.");goto done;}
 info=AllocVec(sizeof(*info),MEMF_PUBLIC|MEMF_CLEAR);
 if(!info||!Info(parent,info)||info->id_BytesPerBlock<=0||info->id_NumBlocks<info->id_NumBlocksUsed||(ULONG)(info->id_NumBlocks-info->id_NumBlocksUsed)<((mb+1)*1048576u/(ULONG)info->id_BytesPerBlock+1)){puts("Not enough free space (allow an extra 1 MB).");if(info)FreeVec(info);UnLock(parent);goto done;}
 FreeVec(info);UnLock(parent);
 parent=Lock(argv[1],SHARED_LOCK);if(parent){UnLock(parent);puts("File already exists; choose a new name.");goto done;}
 if(IoErr()!=ERROR_OBJECT_NOT_FOUND){puts("Cannot check destination file.");goto done;}
 if(snprintf(workdir,sizeof(workdir),"%s.build",argv[1])>=(int)sizeof(workdir)||!di_path(tempfile,sizeof(tempfile),workdir,"NEW.IMG"))goto done;
 worklock=CreateDir(workdir);if(!worklock){puts("Temporary drawer exists or cannot be created.");goto done;}
 out=fopen(tempfile,"wb");if(!out)goto done;
 puts("Creating DOS hard disk. Ctrl-C cancels.\n");fflush(stdout);
 error=di_create(out,argc==4?argv[3]:NULL,NULL,mb,progress,NULL);if(fclose(out)&&!error)error="Cannot close the completed image.";
 if(error)puts(error);else if(!Rename(tempfile,argv[1]))puts("Cannot publish image; destination exists or is unavailable.");else{printf("Ready: %s (%u MB)\n",argv[1],mb);rc=0;}
 done:if(worklock){DeleteFile(tempfile);UnLock(worklock);DeleteFile(workdir);}RemPort(owner);DeleteMsgPort(owner);return rc;
}
