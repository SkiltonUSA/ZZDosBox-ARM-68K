#!/usr/bin/env python3
"""Pinned SB Pro 2, SB16, PC speaker and DBOPL, with deterministic tick mixing into owned PCM ring."""
from pathlib import Path
import difflib
ROOT=Path(__file__).resolve().parents[1];UP=ROOT/'.context/research/dosbox-libretro'
def prepare(out):
 sources=[];patches=[]
 def unit(name,edit):
  rel='src/hardware/'+name+'.cpp';old=(UP/rel).read_text();new=edit(old);p=out/rel;p.write_text(new);sources.append(p)
  patches.extend(difflib.unified_diff(old.splitlines(True),new.splitlines(True),fromfile='a/'+rel,tofile='b/'+rel))
 def sb(s):
  s=s.replace('#include <iomanip>','').replace('#include <sstream>','').replace('#include "shell.h"','#include "audio.h"').replace('\tAutoexecObject autoexecline;','')
  a=s.index('\t\t// Create set blaster line');b=s.index('\t\t/* Soundblaster midi interface */',a);s=s[:a]+s[b:]
  defaults={'sbtype':'(db_sound_card==2?"sb16":"sbpro2")','oplmode':'"opl3"'}
  for k,v in defaults.items():s=s.replace(f'config->Get_string("{k}")',v)
  for k,v in {'sbbase':'0x220','irq':'7','dma':'1','hdma':'5','sbmixer':'true'}.items():
   for t in ['hex','int','bool']:s=s.replace(f'section->Get_{t}("{k}")',v)
  s=s.replace('CMS_Init(section);','/* CMS excluded: this build exposes SB Pro 2, SB16 and OPL3. */').replace('CMS_ShutDown(m_configuration);','')
  s=s.replace('sec->AddDestroyFunction(', 'if(sec)sec->AddDestroyFunction(').replace('delete test;', 'delete test;test=nullptr;')
  return s
 def opl(s):
  a=s.index('#include "mame/emu.h"');b=s.index('namespace Adlib {',a)
  s=s[:a]+s[b:]
  a=s.index('/* Raw DRO capture stuff */');b=s.index('Chip::Chip()',a);s=s[:a]+s[b:]
  s=s.replace('\tif ( capture ) {\n\t\tcapture->DoWrite( reg, val );\n\t}', '')
  a=s.index('static void OPL_SaveRawEvent');b=s.index('namespace Adlib {',a);s=s[:a]+s[b:]
  a=s.index('\tif (oplemu == "compat")');b=s.index('\thandler->Init( rate );',a)
  s=s[:a]+'\thandler = new DBOPL::Handler(oplmode >= OPL_opl3);\n'+s[b:]
  s=s.replace('section->Get_hex("sbbase")','0x220').replace('section->Get_int("oplrate")','32000').replace('section->Get_bool("sbmixer")','true').replace('std::string oplemu( section->Get_string( "oplemu" ) );','')
  s=s.replace('\tMAPPER_AddHandler(OPL_SaveRawEvent,MK_f7,MMOD1|MMOD2,"caprawopl","Cap OPL");','').replace('\tif ( capture ) {\n\t\tdelete capture;\n\t}', '')
  return s
 def mixer(s):
  a=s.index('#ifdef __LIBRETRO__');b=s.index('#include <string.h>',a);s=s[:a]+s[b:]
  s=s.replace('#include "SDL.h"','#include "audio.h"\n#define SDL_LockAudio()\n#define SDL_UnlockAudio()')
  a=s.index('extern bool ticksLocked;');b=s.index('static Bit32u calc_tickadd',a);s=s[:a]+'static bool Mixer_irq_important(){return true;}\n'+s[b:]
  a=s.index('#ifndef __LIBRETRO__',s.index('static void MIXER_MixData'));b=s.index('#endif',a);s=s[:a]+s[b+6:]
  a=s.index('static void MIXER_Mix(void)');b=s.index('static void MIXER_Mix_NoSound',a);s=s[:a]+s[b:]
  s=s.replace('/* Clear piece we\'ve just generated */','''Bit16s pcm[32*2];
 if(mixer.needed>32)E_Exit("PCM tick bound");
 for(Bitu i=0;i<mixer.needed;i++){
  Bitu p=(mixer.pos+i)&MIXER_BUFMASK;
  pcm[i*2]=MIXER_CLIP(mixer.work[p][0]>>MIXER_VOLSHIFT);
  pcm[i*2+1]=MIXER_CLIP(mixer.work[p][1]>>MIXER_VOLSHIFT);
 }
 db_audio_samples(pcm,(unsigned)mixer.needed);
 /* Clear piece we've just generated */''')
  a=s.index('#define INDEX_SHIFT_LOCAL');b=s.index('MixerChannel* MixerObject::Install',a);s=s[:a]+s[b:]
  a=s.index('void MIXER_Init(Section* sec)');s=s[:a]+'''
void MIXER_Init(Section*) {
 memset(&mixer,0,sizeof(mixer));mixer.freq=32000;
 mixer.mastervol[0]=mixer.mastervol[1]=1.0f;
 mixer.tick_add=calc_tickadd(32000);mixer.needed=32;
 TIMER_AddTickHandler(MIXER_Mix_NoSound);
}
void db_mixer_close(){TIMER_DelTickHandler(MIXER_Mix_NoSound);}
'''
  return s
 def speaker(s):
  return s.replace('#include "mixer.h"','#include "mixer.h"\n#include "audio.h"').replace('section->Get_bool("pcspeaker")','db_pc_speaker').replace('section->Get_int("pcrate")','32000').replace('sec->AddDestroyFunction(', 'if(sec)sec->AddDestroyFunction(').replace('delete test;', 'delete test;test=nullptr;')
 unit('mixer',mixer);unit('sblaster',sb);unit('adlib',opl);unit('dbopl',lambda s:s);unit('pcspeaker',speaker)
 (out/'audio-integration.patch').write_text(''.join(patches))
 return sources
