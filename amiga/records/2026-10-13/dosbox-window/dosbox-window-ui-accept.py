import asyncio,json,re,subprocess
from pathlib import Path
from datetime import timedelta
from mcp import ClientSession
from mcp.client.streamable_http import streamablehttp_client
out=Path('.context/amiga/dosbox-window').resolve();events=[]
async def main():
 async with streamablehttp_client('http://127.0.0.1:55010/mcp') as(r,w,_):
  async with ClientSession(r,w) as s:
   await s.initialize()
   async def call(n,a=None):
    res=await s.call_tool(n,a or {},read_timeout_seconds=timedelta(seconds=65));t='\n'.join(getattr(c,'text','') for c in res.content);events.append({'tool':n,'args':a,'text':t});(out/'ui-accept-events.json').write_text(json.dumps(events,indent=2));assert not res.isError,t;return t
   async def click(x,y):
    for n,a in [('amiga_input_mouse_move',{'dx':-10000,'dy':-10000}),('amiga_input_mouse_move',{'dx':x,'dy':y}),('amiga_input_click',{'button':'left','direction':'down'}),('amiga_input_click',{'button':'left','direction':'up'})]:await call(n,a)
    await asyncio.sleep(.3)
   async def snap(label,window):
    t=await call('amiga_screenshot',{'window':window});m=re.search(r'Screenshot saved: (.*?) \(',t);assert m,t;subprocess.run(['sips','-s','format','png',m[1],'--out',str(out/(label+'.png'))],check=True,stdout=subprocess.DEVNULL)
   assert 'Sixies.ARM.Debug.Owner' not in await call('amiga_list_ports')
   w=await call('amiga_list_screen_windows');m=re.search(r'ZZDosBox Launcher  pos=\((\d+),(\d+)\)',w);assert m,w;x,y=map(int,m.groups())
   await click(x+520,y+22);await snap('launcher-settings-final','ZZDosBox Launcher')
   await click(x+405,y+22);await snap('launcher-terminal-final','ZZDosBox Launcher')
   await click(x+135,y+239)
   for _ in range(30):
    w=await call('amiga_list_screen_windows')
    if 'ZZDosBox - F10 closes' in w:break
    await asyncio.sleep(1)
   else:raise AssertionError(w)
   await asyncio.sleep(20)
   await snap('msdos-launcher-window','ZZDosBox - F10 closes')
   m=re.search(r'ZZDosBox - F10 closes  pos=\((\d+),(\d+)\)',w);assert m,w;wx,wy=map(int,m.groups())
   # Application close gadget only: never a Workbench drag/size gesture.
   await click(wx+8,wy+5)
   for _ in range(30):
    if 'Sixies.ARM.Debug.Owner' not in await call('amiga_list_ports'):break
    await asyncio.sleep(1)
   else:raise AssertionError('Close gadget did not release Core1; stop testing')
   log=out/'ui-msdos.log';log.unlink(missing_ok=True);await call('amiga_pull_file',{'amiga_path':'SD032G:Dev/ZZDosBox/PC/LastRun.log','local_path':str(log)});t=log.read_text()
   assert all(v in t for v in ['MS-DOS Version 6.22','DISPLAY_CONFIG mode=window fps=1','SHELL PASS','register_mismatches=00000000','witness_bad=0','LAUNCHER exit=0; owned memory released']),t
   cfg=out/'Launcher-final.cfg';cfg.unlink(missing_ok=True);await call('amiga_pull_file',{'amiga_path':'SD032G:Dev/ZZDosBox/PC/Launcher.cfg','local_path':str(cfg)});t=cfg.read_text();assert t.startswith('ZZDOSLIB2\n') and t.strip().endswith('0 1 1'),t
   print('PASS MUI saved DOS/window/FPS settings after restart; Open terminal booted 6.22; window close gadget returned cleanly.',flush=True)
   print(await call('amiga_list_screen_windows'),flush=True);print(await call('amiga_last_crash'),flush=True)
asyncio.run(main())
