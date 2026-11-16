from pathlib import Path
import shutil,subprocess,json,hashlib
root=Path.cwd();repo=root/'.context/ZZDosBox-ARM-68K';old=root/'.context/amiga/dosbox-dynrec-4m-neon-pipe-scan-present-adapters-pic-vfp-compat-x87-read-ahead';out=repo/'.context/handoff-diagnostics-1115';out.mkdir(parents=True,exist_ok=True)
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
diagnostic = r'''static UBYTE *test_target;
static ULONG test_span,test_nonce,test_sequence,test_checked,test_changed;
static int video_failed(unsigned line){
 printf("VIDEO_FAIL line=%u active=%u complete=%u request=%u done=%u error=%u front=%u slot=%u pending=%ld render=%u\n",line,active,complete,ad_get(shared,PC_VIDEO),ad_get(shared,PC_VIDEO+36),ad_get(shared,PC_VIDEO+40),full_front(),full_slots.front,(long)full_slots.pending,full_slots.render);
 if(test_target){syncio->pull(test_target,4,NULL);syncio->pull(test_target+test_span-4,4,NULL);
 printf("NONCE_FAILURE seq=%u target=%08x span=%u expected=%08x/%08x observed=%08x/%08x checked=%u changed=%u\n",test_sequence,(ULONG)test_target,test_span,test_nonce,test_nonce^0x31415926u,ad_get(test_target,0),ad_get(test_target,test_span-4),test_checked,test_changed);}
 return 0;
}
'''
s=s[:a]+diagnostic+s[a:b].replace('return 0;','return video_failed(__LINE__);')+s[b:]
marker=' copy_start=us();publish(PC_VIDEO+16,seq);active=seq;return 1;'
assert marker in s
s=s.replace(marker,r''' test_target=target;test_span=span;test_nonce=nonce;test_sequence=seq;
 syncio->pull(target,4,NULL);syncio->pull(target+span-4,4,NULL);test_checked++;
 if(ad_get(target,0)!=nonce||ad_get(target,span-4)!=(nonce^0x31415926u)){test_changed++;return video_failed(__LINE__);}
'''+marker)
s=s.replace(' printf("MOUSE events=', ' printf("NONCE_CHECK grants=%u changed=%u\\n",test_checked,test_changed);\n printf("MOUSE events=')
# Exercise actual Intuition depth changes and production mode-switch code.
if __import__('os').environ.get('DOS_VIEW_STRESS'):
 marker='int pc_display_service(void){'
 helper=r'''static unsigned test_depth_phase,test_switches;
static ULONG test_depth_due,test_recover_due;
static unsigned test_recovered;
static void test_views(void){
 /* Private test explicitly brings its screen forward if an unrelated depth
  * change occurs before the scheduled frame-80 test. Production only waits. */
 if(fullscreen&&full_screen&&test_depth_phase==0&&!full_front()&&!test_recovered){
  if(!test_recover_due)test_recover_due=us()+500000u;
  else if((LONG)(us()-test_recover_due)>=0){
   ScreenToFront(full_screen);test_recovered=1;
   printf("VIEW_TEST initial foreground requested frame=%u\n",frames);
  }
 }

 if(test_depth_phase==1 && (LONG)(us()-test_depth_due)>=0){
  ScreenToFront(full_screen);test_depth_phase=2;
  printf("VIEW_TEST foreground requested frame=%u\n",frames);
 }
 if(direct_locked||pc_display_pending()||!window)return;
 if(test_depth_phase==0 && fullscreen && frames>=80){
  ScreenToBack(full_screen);test_depth_due=us()+500000u;test_depth_phase=1;
  printf("VIEW_TEST background requested frame=%u\n",frames);
 }else if(test_depth_phase==2 && test_switches<6 && frames>=200+test_switches*140){
  switch_requested=!fullscreen;test_switches++;
 }
}
'''
 assert marker in s
 s=s.replace(marker,helper+marker+'\n test_views();')
if __import__('os').environ.get('DOS_BAD_NONCE'):
 marker=' copy_start=us();publish(PC_VIDEO+16,seq);active=seq;return 1;'
 assert marker in s
 s=s.replace(marker,' if(seq==100){ad_put(target,span-4,(nonce^0x31415926u)^1u);syncio->push(target+span-4,4,NULL);syncio->barrier(NULL);puts("NONCE_TEST corrupted final marker at frame 100");}\n'+marker)
(out/'display.c').write_text(s)
(out/'windowed.h').write_bytes((repo/'amiga/dosbox/pc/windowed.h').read_bytes())
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
(out/'build.json').write_text(json.dumps({'scope':'Archived release ARM payload; host diagnostics, current support sources; not a performance comparison','sha256':{f.name:hashlib.sha256(f.read_bytes()).hexdigest() for f in [old/'ZZDosBox-Compat',old/'zz_payload.h',out/'ZZDosBox-Diagnostic']},'flags':flags,'injected_foreground_checks':5 if os.environ.get('DOS_FRONT_FAULT') else 0,'injected_bad_nonce':bool(os.environ.get('DOS_BAD_NONCE')),'view_stress':bool(os.environ.get('DOS_VIEW_STRESS'))},indent=2)+'\n')
