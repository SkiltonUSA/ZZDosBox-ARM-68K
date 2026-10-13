/* GPL-3.0-or-later. Read-only driver/mode discovery; no sound allocation. */
#include <devices/ahi.h>
#include <proto/ahi.h>
#include <proto/camd.h>
struct Library *AHIBase,*CamdBase;
static Object *audio_camd,*audio_ahi,*audio_ax;
static char camd_status[128],ahi_status[128],ax_status[128];
static void audio_detect(void){
 struct MsgPort *port=NULL;struct AHIRequest *request=NULL;int opened=0,ax=0,cluster_found=0;ULONG id=AHI_INVALID_ID;
 const char *wanted=(const char*)val(midi_port,MUIA_String_Contents);
 strcpy(camd_status,"CAMD: not installed");strcpy(ahi_status,"AHI: unavailable");strcpy(ax_status,"ZZ9000AX: no AHI mode found");
 CamdBase=OpenLibrary("camd.library",37);
 if(CamdBase){APTR lock=LockCAMD(CD_Linkages);struct MidiCluster*c;
  if(lock){for(c=NextCluster(NULL);c;c=NextCluster(c))if(!strcmp(c->mcl_Node.ln_Name,wanted))cluster_found=1;UnlockCAMD(lock);}
  snprintf(camd_status,sizeof(camd_status),"CAMD %u.%u - %s %s",CamdBase->lib_Version,CamdBase->lib_Revision,wanted,cluster_found?"available":"not found");CloseLibrary(CamdBase);CamdBase=NULL;
 }
 port=CreateMsgPort();if(!port)goto done;request=(struct AHIRequest*)CreateIORequest(port,sizeof(*request));if(!request)goto done;
 request->ahir_Version=4;if(OpenDevice(AHINAME,AHI_NO_UNIT,(struct IORequest*)request,0))goto done;
 opened=1;AHIBase=(struct Library*)request->ahir_Std.io_Device;snprintf(ahi_status,sizeof(ahi_status),"AHI %u.%u installed",AHIBase->lib_Version,AHIBase->lib_Revision);
 while((id=AHI_NextAudioID(id))!=AHI_INVALID_ID){char name[128]={0};if(AHI_GetAudioAttrs(id,NULL,AHIDB_BufferLen,sizeof(name),AHIDB_Name,(ULONG)name,TAG_DONE)&&strstr(name,"ZZ9000AX"))ax++;}
 if(ax)snprintf(ax_status,sizeof(ax_status),"ZZ9000AX: %d AHI modes available",ax);
 done:if(opened)CloseDevice((struct IORequest*)request);AHIBase=NULL;if(request)DeleteIORequest((struct IORequest*)request);if(port)DeleteMsgPort(port);
 set(audio_camd,MUIA_Text_Contents,(ULONG)camd_status);set(audio_ahi,MUIA_Text_Contents,(ULONG)ahi_status);set(audio_ax,MUIA_Text_Contents,(ULONG)ax_status);
}
