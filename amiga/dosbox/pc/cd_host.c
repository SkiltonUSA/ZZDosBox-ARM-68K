/* GPL-3.0-or-later. No writes, eject, configuration changes or raw SCSI. */
#include "cd_host.h"
#include <stdio.h>
#include <string.h>
static uint32_t sectors;
#ifdef DB_NATIVE
static FILE *image;
static int raw_read(uint32_t at,unsigned n,void *data){return image&&fseek(image,(long)at*2048,SEEK_SET)==0&&fread(data,2048,n,image)==n;}
void db_cd_host_close(void){if(image)fclose(image);image=NULL;sectors=0;}
#else
#include <dos/dosextens.h>
#include <dos/filehandler.h>
#include <devices/timer.h>
#include <exec/io.h>
#include <proto/exec.h>
#include <proto/dos.h>
static BPTR image;
static struct MsgPort *port,*timer_port;
static struct IOStdReq *request;
static struct timerequest *timer;
static int device_open,timer_open;
static int raw_read(uint32_t at,unsigned n,void *data){
 if(image)return Seek(image,at*2048,OFFSET_BEGINNING)>=0&&Read(image,data,n*2048)==(LONG)(n*2048);
 if(!device_open||!timer_open)return 0;
 request->io_Command=CMD_READ;request->io_Data=data;request->io_Length=n*2048;request->io_Offset=at*2048;
 timer->tr_node.io_Command=TR_ADDREQUEST;timer->tr_time.tv_secs=5;timer->tr_time.tv_micro=0;
 SendIO((struct IORequest*)request);SendIO((struct IORequest*)timer);
 while(!CheckIO((struct IORequest*)request)&&!CheckIO((struct IORequest*)timer))Wait((1UL<<port->mp_SigBit)|(1UL<<timer_port->mp_SigBit));
 if(!CheckIO((struct IORequest*)request)){AbortIO((struct IORequest*)request);WaitIO((struct IORequest*)request);if(!CheckIO((struct IORequest*)timer))AbortIO((struct IORequest*)timer);WaitIO((struct IORequest*)timer);return 0;}
 WaitIO((struct IORequest*)request);if(!CheckIO((struct IORequest*)timer))AbortIO((struct IORequest*)timer);WaitIO((struct IORequest*)timer);
 return !request->io_Error&&request->io_Actual==n*2048;
}
void db_cd_host_close(void){
 if(image)Close(image);image=0;
 if(device_open)CloseDevice((struct IORequest*)request);device_open=0;
 if(timer_open)CloseDevice((struct IORequest*)timer);timer_open=0;
 if(request)DeleteIORequest((struct IORequest*)request);request=NULL;
 if(timer)DeleteIORequest((struct IORequest*)timer);timer=NULL;
 if(port)DeleteMsgPort(port);port=NULL;
 if(timer_port)DeleteMsgPort(timer_port);timer_port=NULL;sectors=0;
}
static int physical_open(const char *path){
 struct DosList *list,*entry;struct FileSysStartupMsg *startup;UBYTE *b;
 char drive[108],device[108];ULONG unit,flags;size_t n=strlen(path);
 if(n<2||n>=sizeof(drive)||path[n-1]!=':'||strchr(path,'/'))return 0;
 memcpy(drive,path,n-1);drive[n-1]=0;
 list=LockDosList(LDF_DEVICES|LDF_READ);if(!list)return 0;
 entry=FindDosEntry(list,drive,LDF_DEVICES);
 if(!entry||(ULONG)entry->dol_misc.dol_handler.dol_Startup<4096){UnLockDosList(LDF_DEVICES|LDF_READ);return 0;}
 startup=(struct FileSysStartupMsg*)BADDR(entry->dol_misc.dol_handler.dol_Startup);
 b=(UBYTE*)BADDR(startup->fssm_Device);
 if(!b||!b[0]||b[0]>=sizeof(device)){UnLockDosList(LDF_DEVICES|LDF_READ);return 0;}
 memcpy(device,b+1,b[0]);device[b[0]]=0;unit=startup->fssm_Unit;flags=startup->fssm_Flags;
 UnLockDosList(LDF_DEVICES|LDF_READ);
 port=CreateMsgPort();timer_port=CreateMsgPort();if(!port||!timer_port)return 0;
 request=(struct IOStdReq*)CreateIORequest(port,sizeof(*request));timer=(struct timerequest*)CreateIORequest(timer_port,sizeof(*timer));if(!request||!timer)return 0;
 if(OpenDevice(device,unit,(struct IORequest*)request,flags))return 0;
 device_open=1;
 if(OpenDevice(TIMERNAME,UNIT_MICROHZ,(struct IORequest*)timer,0))return 0;
 timer_open=1;
 printf("CD source=%s device=%s unit=%lu read-only\n",path,device,(unsigned long)unit);return 1;
}
#endif
static uint32_t little(const unsigned char *p){return p[0]|((uint32_t)p[1]<<8)|((uint32_t)p[2]<<16)|((uint32_t)p[3]<<24);}
uint32_t db_cd_host_open(unsigned mode,const char *path){
 unsigned char pvd[2048];uint32_t size;db_cd_host_close();if(!mode)return 0;
#ifdef DB_NATIVE
 if(mode!=1||(image=fopen(path,"rb"))==NULL)return 0;
#else
 if(mode==1){image=Open((STRPTR)path,MODE_OLDFILE);if(!image)return 0;}
 else if(mode!=2||!physical_open(path)){db_cd_host_close();return 0;}
#endif
 if(!raw_read(16,1,pvd)||pvd[0]!=1||memcmp(pvd+1,"CD001",5)||pvd[6]!=1||pvd[128]!=0||pvd[129]!=8)goto bad;
 size=little(pvd+80);if(size<18||size>0x7fffffffu/2048)goto bad;
 if(mode==1){
#ifdef DB_NATIVE
  if(fseek(image,0,SEEK_END)||ftell(image)<(long)size*2048)goto bad;
#else
  if(Seek(image,0,OFFSET_END)<0||Seek(image,0,OFFSET_CURRENT)<(LONG)(size*2048))goto bad;
#endif
 }
 sectors=size;printf("CD ISO9660 sectors=%lu read-only\n",(unsigned long)sectors);return sectors;
bad:db_cd_host_close();return 0;
}
int db_cd_host_read(uint32_t sector,unsigned count,void *data){return count&&count<=2&&sector<sectors&&count<=sectors-sector&&raw_read(sector,count,data);}
