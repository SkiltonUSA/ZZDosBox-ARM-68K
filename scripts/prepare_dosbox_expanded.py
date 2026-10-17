#!/usr/bin/env python3
"""Pinned gameport and additional sound devices for the expanded PC runtime."""
from pathlib import Path
import difflib
ROOT=Path(__file__).resolve().parents[1]
UP=ROOT/'.context/research/dosbox-libretro'
def prepare(out):
 sources=[];patches=[]
 def unit(rel,edit=lambda s:s):
  old=(UP/rel).read_text();new=edit(old);p=out/rel;p.parent.mkdir(parents=True,exist_ok=True);p.write_text(new)
  if p.suffix=='.cpp':sources.append(p)
  patches.extend(difflib.unified_diff(old.splitlines(True),new.splitlines(True),fromfile='a/'+rel,tofile='b/'+rel))
 def lifecycle(s):
  return s.replace('sec->AddDestroyFunction(','if(sec)sec->AddDestroyFunction(').replace('delete test;','delete test;test=nullptr;')
 def joystick(s):
  s=s.replace('extern bool autofire;','bool autofire;')
  for name,value in {'joysticktype':'"2axis"','timed':'true','autofire':'false','swap34':'false','buttonwrap':'false','circularinput':'false','deadzone':'0'}.items():
   for kind in ['string','bool','int']:s=s.replace(f'section->Get_{kind}("{name}")',value)
  s=s.replace('Section_prop * section = static_cast<Section_prop *>(configuration);','Section_prop * section = static_cast<Section_prop *>(configuration);\nmemset(stick,0,sizeof(stick));write_active=false;last_write=0;')
  return lifecycle(s)
 def gus(s):
  s=s.replace("Bit8u adlib_commandreg;","extern Bit8u adlib_commandreg;")
  s=s.replace('#include <iomanip>','').replace('#include <sstream>','').replace('#include "shell.h"','').replace('\tAutoexecObject autoexecline[2];','')
  a=s.index('\t\tBitu portat =');b=s.index('\n\t}',a);s=s[:a]+s[b:]
  for k,v in {'gus':'true','gusbase':'0x240','gusdma':'3','gusirq':'5'}.items():
   for kind in ['bool','hex','int']:s=s.replace(f'section->Get_{kind}("{k}")',v)
  return lifecycle(s)
 def cms(s):
  return lifecycle(s.replace('section->Get_int( "oplrate" )','32000').replace('section->Get_hex("sbbase")','0x220').replace('section->Get_string("sbtype")','"gb"'))
 def tandy(s):
  s=s.replace('section->Get_string("tandy")','"on"').replace('section->Get_int("tandyrate")','32000')
  s=s.replace('bool enable_hw_tandy_dac=true;','memset(&tandy,0,sizeof(tandy));\n\t\tbool enable_hw_tandy_dac=true;')
  return lifecycle(s)
 def serial(s):
  s=s[:s.index('class SERIALPORTS:')]
  for h in ['directserial','serialdummy','softmodem','nullmodem']:s=s.replace('#include "'+h+'.h"','')
  # The upstream destructor releases the DOS device, but leaks its three FIFOs.
  s=s.replace('DOS_DelDevice(mydosdevice);','DOS_DelDevice(mydosdevice);delete errorfifo;delete rxfifo;delete txfifo;')
  return s
 unit('src/hardware/serialport/serialport.cpp',serial)
 unit('src/hardware/joystick.cpp',joystick)
 unit('src/hardware/gus.cpp',gus)
 unit('src/hardware/gameblaster.cpp',cms)
 unit('src/hardware/tandy_sound.cpp',tandy)
 for name in ['emu.h','saa1099.h','saa1099.cpp','sn76496.h','sn76496.cpp']:unit('src/hardware/mame/'+name)
 # Restore DOS AUX services removed by the small initial DOS-kernel port.
 p=out/'src/dos/dos.cpp';text=p.read_text();upstream=(UP/'src/dos/dos.cpp').read_text()
 a=upstream.index('\tcase 0x03:');b=upstream.index('\tcase 0x05:',a)
 text=text.replace('\tcase 0x03: case 0x04: E_Exit("Serial device not connected");\n',upstream[a:b])
 p.write_text('#include "serialport.h"\n'+text)
 (out/'expanded-devices.patch').write_text(''.join(patches))
 return sources
