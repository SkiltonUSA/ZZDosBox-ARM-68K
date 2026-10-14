#!/usr/bin/env python3
"""Explicit RAM-only staging / one physical A9 TinyGL raster measurement over the bridge.

Use the MCP Python environment. No installed executable/profile is replaced.
Read docs/amiga-session-memory.md before use. Model checks must precede run.
"""
import argparse, asyncio, json, re, time, zlib
from datetime import timedelta
from pathlib import Path
from mcp import ClientSession
from mcp.client.streamable_http import streamablehttp_client
ROOT=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser(description=__doc__)
p.add_argument('action',choices=['stage','run'])
p.add_argument('--build',required=True,type=Path)
p.add_argument('--label',default='raster-1')
a=p.parse_args();a.build=a.build.resolve()
assert re.fullmatch('[a-z0-9-]+',a.label)
OUT=ROOT/'.context/amiga/dosbox-tinygl-raster';OUT.mkdir(parents=True,exist_ok=True)
BASE='RAM:ZZRaster';binary=a.build/'ZZDosBox-Dynrec'
metadata=json.loads((a.build/'build.json').read_text())
assert metadata['tinygl_raster']
new_report=metadata['tinygl']['raster_experiment']['revision']>=3
report_marker='RASTERBENCH magic=52535432 errors=0 count=8 quick=0' if new_report else 'RASTERBENCH magic=52535431 errors=0 count=7 quick=0'

if metadata['tinygl']['raster_experiment']['revision']>=4:report_marker='RASTERBENCH magic=52535433 errors=0 count=8 quick=0'

async def main():
    async with streamablehttp_client('http://127.0.0.1:55010/mcp') as(r,w,_):
        async with ClientSession(r,w) as s:
            await s.initialize()
            async def call(n,args):
                v=await s.call_tool(n,args,read_timeout_seconds=timedelta(seconds=120))
                t='\n'.join(getattr(c,'text','') for c in v.content)
                with (OUT/(a.label+'-events.jsonl')).open('a') as f:f.write(json.dumps(dict(tool=n,args=args,result=t))+'\n')
                assert not v.isError and not any(x in t for x in ('[ERROR]','ERROR:','Not connected','Timeout')),t
                return t
            async def script(code):
                t=await call('amiga_run_script',dict(script=code,timeout=10));assert '[OK]' in t,t;return t
            async def idle():
                assert 'Sixies.ARM.Debug.Owner' not in await call('amiga_list_ports',{}),'Core1 busy'
                assert 'Software Failure' not in await call('amiga_list_screen_windows',{}),'Amiga recovery required'
            async def push(f,name):
                await call('amiga_push_file',dict(local_path=str(f),amiga_path=BASE+'/'+name))
            await idle()
            installed='SD032G:Dev/ZZDosBox/PC/ZZDosBox'
            before=await call('amiga_checksum',dict(path=installed))
            if a.action=='stage':
                await script('If NOT EXISTS '+BASE+'\nMakeDir '+BASE+'\nEndIf\nIf NOT EXISTS '+BASE+'/Drive\nMakeDir '+BASE+'/Drive\nEndIf')
                await push(binary,'ZZRasterBench')
                await push(a.build/'EGATEST.COM','Drive/EGATEST.COM')
                session=OUT/'fixture.session'
                session.write_text('ZZDOSSESSION5\n'+BASE+'/Drive\nEGATEST.COM\n8 0 3000 7 0 1 0 1 0 0 1\n0\n\n0\nout.0\n')
                await push(session,session.name)
                await script('Protect '+BASE+'/ZZRasterBench +e')
                assert f'{zlib.crc32(binary.read_bytes()):08X}' in (await call('amiga_checksum',dict(path=BASE+'/ZZRasterBench'))).upper()
                print('STAGED in RAM only; no launch',flush=True);return
            assert not (OUT/(a.label+'.log')).exists(),'Choose a fresh evidence label'
            assert 'EXISTS' not in await script('If EXISTS '+BASE+'/'+a.label+'.done\nEcho EXISTS\nEndIf')
            assert f'{zlib.crc32(binary.read_bytes()):08X}' in (await call('amiga_checksum',dict(path=BASE+'/ZZRasterBench'))).upper()
            run=OUT/('run-'+a.label)
            run.write_text('FailAt 21\nStack 65536\n'+BASE+'/ZZRasterBench --session '+BASE+'/fixture.session --presentation off >'+BASE+'/'+a.label+'.log\nEcho CLOSED >'+BASE+'/'+a.label+'.done\n')
            await push(run,run.name)
            await script('Run >NIL: Execute '+BASE+'/'+run.name)
            start=time.monotonic();print('START',a.label,flush=True)
            while time.monotonic()-start<120:
                await asyncio.sleep(3)
                if 'CLOSED' in await script('If EXISTS '+BASE+'/'+a.label+'.done\nEcho CLOSED\nEndIf'):break
            else:raise RuntimeError('Observation timeout: do not launch another workload')
            await idle()
            log=OUT/(a.label+'.log')
            await call('amiga_pull_file',dict(amiga_path=BASE+'/'+a.label+'.log',local_path=str(log)))
            text=log.read_text()
            for marker in ('SHELL PASS','EGA/PIT PASS','guest_exit=0','context_equal=1','register_mismatches=00000000','witness_bad=0','LAUNCHER exit=0; owned memory released',report_marker):
                assert marker in text,marker
            after=await call('amiga_checksum',dict(path=installed));assert before==after,'Installed runtime changed unexpectedly'
            print('PASS',a.label,'closed log, exact outputs, restoration, memory release; installed runtime unchanged',flush=True)
asyncio.run(main())
