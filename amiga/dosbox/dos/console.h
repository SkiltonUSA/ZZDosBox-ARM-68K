/* GPL-2.0-or-later. Headless DOS console frontend. */
#include "drive.h"
#include "console_io.h"
class device_CON:public DOS_Device {
public:
 device_CON(){open=true;flags=OPEN_READWRITE;SetName("CON");}
 bool Read(Bit8u *data,Bit16u *n)override{*n=db_console_read(data,*n,dos.echo);return true;}
 bool Write(Bit8u *data,Bit16u *n)override{db_console(data,*n);return true;}
 bool Seek(Bit32u *p,Bit32u)override{*p=0;return true;}
 bool Close()override{return true;}
 Bit16u GetInformation()override{return db_console_ready()?0x8093:0x80d3;}
 bool ReadFromControlChannel(PhysPt,Bit16u,Bit16u*)override{return false;}
 bool WriteToControlChannel(PhysPt,Bit16u,Bit16u*)override{return false;}
};
