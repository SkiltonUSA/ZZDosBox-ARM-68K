import asyncio,json,re,sys,zlib,subprocess
from pathlib import Path
from datetime import timedelta
from mcp import ClientSession
from mcp.client.streamable_http import streamablehttp_client
out=Path('.context/amiga/dosbox-window').resolve();base='SD032G:Dev/ZZDosBox/PC';events=[]
async def main():
 async with streamablehttp_client('http://127.0.0.1:55010/mcp') as (r,w,_):
  async with ClientSession(r,w) as s:
   await s.initialize()
   async def call(n,a=None):
    result=await s.call_tool(n,a or {},read_timeout_seconds=timedelta(seconds=65));t='\n'.join(getattr(c,'text','') for c in result.content);events.append({'tool':n,'args':a,'text':t});(out/(sys.argv[1]+'-events.json')).write_text(json.dumps(events,indent=2));assert not result.isError,t;return t
   async def key(k):
    await call('amiga_input_key',{'rawkey':k,'direction':'down'});await asyncio.sleep(.12);await call('amiga_input_key',{'rawkey':k,'direction':'up'})
   async def snapshot(label):
    t=await call('amiga_screenshot',{'window':'ZZDosBox - F10 closes'});m=re.search(r'Screenshot saved: (.*?) \(',t);assert m,t
    subprocess.run(['sips','-s','format','png',m[1],'--out',str(out/(label+'.png'))],check=True,stdout=subprocess.DEVNULL)
   mode=sys.argv[1]
   assert 'Sixies.ARM.Debug.Owner' not in await call('amiga_list_ports')
   if mode.startswith('deploy'):
    for src,name in [('.context/amiga/dosbox-pc/ZZDosBox','ZZDosBox-Window'),('.context/amiga/dosbox-launcher/ZZDosBox-Launcher','Launcher-Window')]:
     if mode=='deploy-ui' and name!='Launcher-Window':continue
     p=Path(src).resolve();crc=f'{zlib.crc32(p.read_bytes()):08X}';t=await call('amiga_push_file',{'local_path':str(p),'amiga_path':base+'/'+name});print(t,flush=True)
     t=await call('amiga_checksum',{'path':base+'/'+name});assert crc in t,t;print(t,flush=True)
    return
   system=1 if mode.startswith('msdos') else 0;full=int('full' in mode);fps=int('fps' in mode)
   drive=base+'/Disks' if system else 'SD032G:Dev/ZZDosBox/Shell/Games/KEEN6' if mode.startswith('keen') else 'SD032G:Dev/ZZDosBox/Drives/C'
   command='ver' if system else 'KEEN6C' if mode.startswith('keen') else 'ver'
   path=out/'test.session';path.write_text(f'ZZDOSSESSION2\n{drive}\n{command}\n8 0 3000 7 0 1 1 {full} {fps} {system}\n')
   await call('amiga_push_file',{'local_path':str(path),'amiga_path':'RAM:ZZDWindow.session'})
   t=await call('amiga_run_script',{'script':f'Stack 65536\nRun >NIL: {base}/ZZDosBox-Window --session RAM:ZZDWindow.session >RAM:ZZDWindow.log','timeout':10});assert '[OK]' in t and 'STILL RUNNING' not in t,t
   for _ in range(35):
    windows=await call('amiga_list_screen_windows')
    if 'ZZDosBox - F10 closes' in windows:break
    await asyncio.sleep(1)
   else:raise AssertionError(windows)
   print(windows,flush=True)
   await asyncio.sleep(22 if system else 8)
   if mode.startswith('keen'):
    await key('44');await asyncio.sleep(6)
   else:
    # Exercise physical raw keys too, independently of the startup command.
    for k in ['34','12','13','44']:await key(k)
    await asyncio.sleep(2)
   await snapshot(mode)
   await key('59')
   for _ in range(30):
    if 'Sixies.ARM.Debug.Owner' not in await call('amiga_list_ports'):break
    await asyncio.sleep(1)
   else:raise AssertionError('Core1 owner remains after F10; stop here')
   log=out/(mode+'.log');log.unlink(missing_ok=True);await call('amiga_pull_file',{'amiga_path':'RAM:ZZDWindow.log','local_path':str(log)});text=log.read_text()
   assert all(x in text for x in ['SHELL PASS','LAUNCHER exit=0; owned memory released','register_mismatches=00000000','witness_bad=0']),text
   if system:assert 'MS-DOS Version 6.22' in text,text
   if not full:assert 'WINDOW native_format=' in text,text
   if fps:assert int(re.search(r'ARM_FPS tenths=(\d+)',text)[1])>0,text
   print(text[-5000:],flush=True);print(await call('amiga_ping'),flush=True)
asyncio.run(main())
