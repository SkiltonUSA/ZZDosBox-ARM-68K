from pathlib import Path
import shutil,subprocess,json,hashlib
root=Path.cwd();repo=root/'.context/ZZDosBox-ARM-68K';old=root/'.context/amiga/dosbox-dynrec-4m-neon-pipe-scan-present-adapters-pic-vfp-compat-x87-read-ahead';out=repo/'.context/host-diagnostics-1115';out.mkdir(parents=True,exist_ok=True)
for f in [*old.glob('*.h'),old/'launcher.c',old/'host.c']:shutil.copy2(f,out/f.name)
f=out/'host.c';s=f.read_text()
s=s.replace('static int run(struct DbWindow *console){','static int service_failed(const char *name,int *ok){if(*ok)printf("HOST_SERVICE_FAIL %s\\n",name);*ok=0;return 1;}\nstatic int run(struct DbWindow *console){')
for name,pattern in [('audio','if(!db_audio_host_service()){transport_ok=0;want_stop=1;}'),('files','if(!timed_fs_service()){transport_ok=0;want_stop=1;}'),('console','if(!drain(pc_display_locked()?NULL:console)){transport_ok=0;want_stop=1;}')]:
 assert pattern in s,pattern;s=s.replace(pattern,pattern.split('{')[0]+'{want_stop=service_failed("'+name+'",&transport_ok);}')
s=s.replace('if(!display_result)transport_ok=0;', 'if(!display_result)service_failed("display",&transport_ok);')
s=s.replace('if(!(midi_result=db_midi_host_service())){transport_ok=0;want_stop=1;}', 'if(!(midi_result=db_midi_host_service())){want_stop=service_failed("midi",&transport_ok);}')
s=s.replace('   printf("SHELL result=', '   printf("HOST_END transport=%d stopping=%d stop_word=%u\\n",transport_ok,stopping,receive(ZZ_STOP));\n   printf("SHELL result=')
f.write_text(s)
s=(repo/'amiga/dosbox/pc/display.c').read_text();a=s.index('int pc_display_service(void)');b=s.index('extern void pc_console_cleanup',a)
s=s[:a]+'static int video_failed(unsigned line){printf("VIDEO_FAIL line=%u active=%u complete=%u request=%u done=%u error=%u\\n",line,active,complete,ad_get(shared,PC_VIDEO),ad_get(shared,PC_VIDEO+36),ad_get(shared,PC_VIDEO+40));return 0;}\n'+s[a:b].replace('return 0;','return video_failed(__LINE__);')+s[b:]
(out/'display.c').write_text(s)
import os
full=(repo/'amiga/dosbox/pc/fullscreen.h').read_text()
if os.environ.get('DOS_FRONT_FAULT'):
 old_front='static int full_front(void){return full_screen && IntuitionBase->FirstScreen==full_screen;}'
 assert old_front in full
 full=full.replace(old_front,'static unsigned test_front_waits;\nstatic int full_front(void){if(frames==100 && test_front_waits<5){test_front_waits++;return 0;}return full_screen && IntuitionBase->FirstScreen==full_screen;}')
(out/'fullscreen.h').write_text(full)

folder=str(out.relative_to(repo));pin=json.loads((root/'amiga/c64/toolchain.json').read_text())['image']
flags=['-DDB_COMPAT','-DDB_SCANLINE_TEST','-DDB_PRESENT_TEST','-DDB_PIPELINE_DEFAULT=1','-DFF_DIRECT','-DZZ_PC_CD','-DZZ_HEADLESS_BENCH','-DZZ_RELEASE','-DZZ_BLOCK_SIZE=0x2000000','-DZZ_CONTROL=0x180000','-DMB_ALLOCATION=0x2000000','-DDB_JIT_MIB=4','-DZZ_APP_NAME="ZZDosBox-Diagnostic"','-DZZ_APP_VERSION="0.22.7-host-diagnostic"','-DZZ_MIN_STACK=65536','-DMB_MMU','-DMB_DCACHE','-DZZ_HEADLESS_MMU','-DZZ_HEADLESS_ICACHE']
incs=[folder,'amiga/dosbox/pc','amiga/arm_debug','amiga/arm_debug/zz9000','amiga/common/arm','amiga/dosbox/dos','amiga/dosbox/console','amiga/dosbox/shell']
sources=[folder+'/launcher.c',folder+'/host.c',folder+'/display.c','amiga/dosbox/console/window.c','amiga/dosbox/shell/fs_host.c','amiga/dosbox/pc/cd_host.c','amiga/dosbox/pc/midi_host.c','amiga/dosbox/pc/audio_host.c']
subprocess.run(['podman','--connection','podman-machine-default-root','run','--rm','--platform','linux/amd64','-v',str(repo)+':/work','-w','/work',pin,'m68k-amigaos-gcc','-std=c99','-noixemul','-m68020','-O2','-g','-Wall','-Wextra','-Werror',*flags,*['-I'+x for x in incs],*sources,'-lamiga','-o',folder+'/ZZDosBox-Diagnostic'],check=True)
(out/'build.json').write_text(json.dumps({'scope':'Archived release ARM payload; host diagnostics, current support sources; not a performance comparison','sha256':{f.name:hashlib.sha256(f.read_bytes()).hexdigest() for f in [old/'ZZDosBox-Compat',old/'zz_payload.h',out/'ZZDosBox-Diagnostic']},'flags':flags,'injected_foreground_checks':5 if os.environ.get('DOS_FRONT_FAULT') else 0},indent=2)+'\n')
