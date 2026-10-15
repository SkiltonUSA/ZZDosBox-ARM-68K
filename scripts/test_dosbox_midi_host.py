#!/usr/bin/env python3
"""Exercise the production CAMD service with a fake clock and MIDI endpoint."""
from pathlib import Path
import subprocess
root=Path(__file__).resolve().parents[1];out=root/'.context/amiga/optimisation-round2/midi-test';out.mkdir(parents=True,exist_ok=True)
s=(root/'amiga/dosbox/pc/midi_host.c').read_text();s='\n'.join(line for line in s.splitlines() if not line.startswith('#include'))+'\n'
(out/'midi_host_body.inc').write_text(s)
exe=out/'test'
subprocess.run(['clang','-std=c99','-O1','-g','-Wall','-Wextra','-Werror','-Wno-pointer-to-int-cast','-fsanitize=address,undefined','-fno-sanitize-recover=all',*['-I'+str(root/p) for p in ['amiga/dosbox/pc','amiga/dosbox/console','amiga/dosbox/dos','amiga/common/arm','amiga/arm_debug','amiga/arm_debug/zz9000']],'-I'+str(out),str(root/'tests/amiga/dosbox/midi_host_test.c'),'-o',str(exe)],check=True)
subprocess.run([str(exe)],check=True)
