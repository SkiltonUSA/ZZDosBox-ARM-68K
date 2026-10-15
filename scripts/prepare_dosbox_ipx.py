#!/usr/bin/env python3
"""Adapt the pinned DOSBox IPX driver to bounded Roadshow UDP transport."""
from pathlib import Path
import difflib
ROOT=Path(__file__).resolve().parents[1]
BASE=ROOT/'amiga/dosbox/network'
def prepare(out):
    up=ROOT/'.context/research/dosbox-libretro'
    old=(up/'src/hardware/ipx.cpp').read_text();s=old
    s=s.replace('#include "SDL_net.h"','#include "SDL_net.h"\n#include "ipx_packet.h"\nextern bool db_ipx_enabled();\nextern void db_ipx_connected(unsigned);')
    s=s.replace('GetTicks()', 'PIC_Ticks')
    # Shell dispatch avoids adding a virtual Z: drive or external DOS driver.
    a=s.index('class IPXNET :');b=s.index('Bitu IPX_ESRHandler',a)
    s=s[:a]+(BASE/'ipx_commands.inc').read_text()+'\n'+s[b:]
    a=s.index('\t\tSection_prop * section',s.index('IPX(Section*'));b=s.index('\n\t\tECBList',a)
    s=s[:a]+s[b:]
    s=s.replace('\t\tPROGRAMS_MakeFile("IPXNET.COM",IPXNET_ProgramStart);','')
    s=s.replace('\t\tSection_prop * section = static_cast<Section_prop *>(m_configuration);','')
    s=s.replace('\t\tif(!section->Get_bool("ipx")) return;','')
    s=s.replace('\t\t\tIPX_StopServer();','')
    s=s.replace('\t\tVFILE_Remove("IPXNET.COM");','\t\twhile(ECBList)delete ECBList;while(ESRList)delete ESRList;dospage=0;')
    a=s.index('void IPX_ShutDown(');b=s.index('//Initialize static',a)
    s=s[:a]+'''void db_ipx_open(){if(db_ipx_enabled()&&!test)test=new IPX(nullptr);}
void db_ipx_close(){delete test;test=nullptr;SDLNet_UDP_Close(ipxClientSocket);}
'''+s[b:]
    # Enforce header bounds before reading guest fragments or received packets.
    s=s.replace('fragCount = sendecb->getFragCount();', '''fragCount = sendecb->getFragCount();
 if(!fragCount||fragCount>64){sendecb->setCompletionFlag(COMP_MALFORMED);sendecb->NotifyESR();return;}
 sendecb->getFragDesc(0,&tmpFrag);
 if(tmpFrag.size<30){sendecb->setCompletionFlag(COMP_MALFORMED);sendecb->NotifyESR();return;}''')
    s=s.replace('Bitu fragCount = getFragCount();','Bitu fragCount = getFragCount();\n if(!fragCount||fragCount>64){setCompletionFlag(COMP_MALFORMED);return false;}')
    s=s.replace('static void receivePacket(Bit8u *buffer, Bit16s bufSize) {','static void receivePacket(Bit8u *buffer, Bit16s bufSize) {\n if(bufSize<0||!zz_ipx_valid(buffer,bufSize))return;')
    s=s.replace('IPXHeader regHeader;','IPXHeader regHeader = {};')
    s=s.replace('if (result != 0) {\n\t\t\t\t\t\tmemcpy(localIpxAddr', 'if (result != 0 && zz_ipx_registration(regPacket.data,regPacket.len)) {\n\t\t\t\t\t\tmemcpy(localIpxAddr')
    # Keep ECB storage bounded even when a guest repeatedly queues listeners.
    s=s.replace('ECBClass *ECBList;', 'static unsigned zz_ecbs;\nECBClass *ECBList;')
    s=s.replace('ECBClass::ECBClass(Bit16u segment, Bit16u offset) {','ECBClass::ECBClass(Bit16u segment, Bit16u offset) {\n if(++zz_ecbs>256)E_Exit("IPX ECB limit");')
    s=s.replace('ECBClass::~ECBClass() {','ECBClass::~ECBClass() {\n if(zz_ecbs)--zz_ecbs;')
    s=s.replace('LOG_IPX("SEND crc:%2x",packetCRC(&outbuffer[0], packetsize));','')
    (out/'ipx.cpp').write_text(s)
    (out/'ipx-integration.patch').write_text(''.join(difflib.unified_diff(old.splitlines(True),s.splitlines(True),fromfile='a/src/hardware/ipx.cpp',tofile='b/src/hardware/ipx.cpp')))
    (out/'SDL_net.h').write_bytes((BASE/'ipx_sdl.h').read_bytes())
    p=out/'include/config.h';p.write_text(p.read_text().replace('/* #undef C_IPX */','#define C_IPX 1'))
    p=out/'src/shell/shell_cmds.cpp';s=p.read_text().replace('void DOS_Shell::DoCommand(char * line) {','void DOS_Shell::DoCommand(char * line) {\n extern bool db_ipx_command(char*);if(db_ipx_command(line))return;');p.write_text(s)
    return [out/'ipx.cpp',BASE/'ipx_transport.cpp']
