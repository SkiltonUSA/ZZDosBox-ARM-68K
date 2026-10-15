/* GPL-2.0-or-later. Read-only data CD adapter and real-DOS driver gateway. */
#include "cd_interface.h"
#include "cd_iso.h"
#include "pc_wire.h"
#include "pc.h"
#include "callback.h"
#include "regs.h"
#include <string.h>
extern "C" volatile uint8_t *db_shared();
extern bool db_cd_sector(unsigned,void*);
extern void MSCDEX_Init(Section*),MSCDEX_ShutDown(Section*);
static unsigned cd_sectors;
static bool mscdex_open;
/* Shared INT 2F chain: MSCDEX and IPX must coexist and unregister cleanly. */
static MultiplexHandler *multiplex[4];static unsigned multiplex_count;static bool multiplex_installed;
static Bitu cd_int2f(){for(unsigned i=multiplex_count;i;i--)if(multiplex[i-1]())break;return CBRET_NONE;}
void DOS_AddMultiplexHandler(MultiplexHandler*h){
 if(!h||multiplex_count==4)E_Exit("DOS multiplexer limit");
 for(unsigned i=0;i<multiplex_count;i++)if(multiplex[i]==h)E_Exit("Duplicate DOS multiplexer");
 if(!multiplex_installed){Bitu cb=CALLBACK_Allocate();CALLBACK_Setup(cb,cd_int2f,CB_IRET,"DOS multiplex");RealSetVec(0x2f,CALLBACK_RealPointer(cb));multiplex_installed=true;}
 multiplex[multiplex_count++]=h;
}
void DOS_DelMultiplexHandler(MultiplexHandler*h){for(unsigned i=0;i<multiplex_count;i++)if(multiplex[i]==h){for(unsigned j=i+1;j<multiplex_count;j++)multiplex[j-1]=multiplex[j];--multiplex_count;return;}}
CDROM_Interface_Image *CDROM_Interface_Image::images[8];
static TMSF msf(unsigned frames){TMSF p={(Bit8u)(frames/4500),(Bit8u)(frames/75%60),(Bit8u)(frames%75)};return p;}
bool CDROM_Interface_Image::SetDevice(char*,int){return cd_sectors!=0;}
bool CDROM_Interface_Image::GetAudioTracks(int &first,int &last,TMSF &end){first=last=1;end=msf(cd_sectors+150);return cd_sectors!=0;}
bool CDROM_Interface_Image::GetAudioTrackInfo(int track,TMSF &start,unsigned char &attr){start=msf(150);attr=0x40;return track==1&&cd_sectors;}
bool CDROM_Interface_Image::GetMediaTrayStatus(bool &present,bool &changed,bool &open){present=cd_sectors!=0;changed=false;open=!present;return true;}
bool CDROM_Interface_Image::ReadSector(Bit8u *data,bool raw,Bit32u sector){return !raw&&sector<cd_sectors&&db_cd_sector(sector,data);}
static bool guest_span(PhysPt at,unsigned n){return at<db_memory_mb*1048576u&&n<=db_memory_mb*1048576u-at;}
bool CDROM_Interface_Image::ReadSectors(PhysPt at,bool raw,unsigned long sector,unsigned long n){
 if(raw||!n||n>128||sector>=cd_sectors||n>cd_sectors-sector||!guest_span(at,n*2048))return false;
 Bit8u data[2048];for(unsigned i=0;i<n;i++){if(!ReadSector(data,false,sector+i))return false;MEM_BlockWrite(at+i*2048,data,2048);}return true;
}
static Bitu driver_request(){
 if(reg_ax!=0x5a43){CALLBACK_SCF(true);return CBRET_NONE;}
 PhysPt req=PhysMake(SegValue(es),reg_bx);unsigned error=0;
 if(!guest_span(req,13)){CALLBACK_SCF(true);return CBRET_NONE;}
 unsigned cmd=mem_readb(req+2),length=mem_readb(req),unit=mem_readb(req+1);
 if(!cd_sectors||unit)error=0x0c;
 else if(!guest_span(req,length))error=5;
 else if(cmd==0){if(length<23)error=5;else mem_writeb(req+13,1);}
 else if(cmd==3||cmd==12){
  if(length<20)error=5;
  else{
   PhysPt p=PhysMake(mem_readw(req+16),mem_readw(req+14));unsigned n=mem_readw(req+18);
   if(!n||!guest_span(p,n))error=5;
   else{
    unsigned f=mem_readb(p);
    if(cmd==12){if(f!=1&&f!=2)error=3;} /* lock/reset only; never eject host media */
    else switch(f){
     case 0:if(n<5)error=5;else mem_writed(p+1,RealMake(reg_cx,0));break;
     case 6:if(n<5)error=5;else mem_writed(p+1,0x200);break; /* HSG/MSF, data-only */
     case 7:if(n<4)error=5;else if(mem_readb(p+1))error=3;else mem_writew(p+2,2048);break;
     case 8:if(n<5)error=5;else mem_writed(p+1,cd_sectors);break;
     case 9:if(n<2)error=5;else mem_writeb(p+1,1);break; /* unchanged */
     case 10:{if(n<7){error=5;break;}TMSF end=msf(cd_sectors+150);mem_writeb(p+1,1);mem_writeb(p+2,1);mem_writeb(p+3,end.fr);mem_writeb(p+4,end.sec);mem_writeb(p+5,end.min);mem_writeb(p+6,0);break;}
     case 11:if(n<7)error=5;else if(mem_readb(p+1)!=1)error=3;else{mem_writed(p+2,0x00000200);mem_writeb(p+6,0x40);}break;
     case 15:if(n<11)error=5;else for(unsigned i=1;i<11;i++)mem_writeb(p+i,0);break;
     default:error=3;break;
    }
   }
  }
 }else if(cmd==0x80){
  if(length<27)error=5;
  else{
   unsigned mode=mem_readb(req+13),n=mem_readw(req+18),sector=mem_readd(req+20);
   PhysPt at=PhysMake(mem_readw(req+16),mem_readw(req+14));
   if(mode==1){unsigned frames=((sector>>16)&255)*4500+((sector>>8)&255)*75+(sector&255);if(frames<150)error=8;else sector=frames-150;}else if(mode)error=3;
   if(!error){CDROM_Interface_Image backend(7);if(!backend.ReadSectors(at,mem_readb(req+24)!=0,sector,n))error=8;}
  }
 }else if(cmd!=13&&cmd!=14&&cmd!=0x82&&cmd!=0x83)error=3;
 mem_writew(req+3,0x100|(error?0x8000|error:0));CALLBACK_SCF(false);return CBRET_NONE;
}
void db_cd_setup(){
 cd_sectors=ad_get(db_shared(),PC_VIDEO+944);mscdex_open=false;multiplex_count=0;multiplex_installed=false;
 if(!cd_sectors)return;
 if(cd_sectors<18||cd_sectors>0x7fffffffu/2048)E_Exit("Invalid CD sector count");
 Bitu cb=CALLBACK_Allocate();CALLBACK_Setup(cb,driver_request,CB_IRET,"ZZDosBox CD-ROM driver");RealSetVec(0x60,CALLBACK_RealPointer(cb));
}
void db_cd_mount(){
 if(!cd_sectors)return;
 MSCDEX_Init(nullptr);mscdex_open=true;int error=0;
 Drives[3]=new isoDrive('D',"ZZCD",0xf8,error);if(error)E_Exit("CD-ROM ISO mount error %d",error);
}
void db_cd_close(){if(mscdex_open)MSCDEX_ShutDown(nullptr);mscdex_open=false;cd_sectors=0;}
