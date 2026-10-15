#!/usr/bin/env python3
"""Validate production host packet queues without touching real hardware."""
from pathlib import Path
import subprocess
root=Path(__file__).resolve().parents[1]
out=root/'.context/amiga/dosbox-network/tests';out.mkdir(parents=True,exist_ok=True)
inc=['amiga/dosbox/network','amiga/dosbox/pc','amiga/dosbox/console','amiga/dosbox/dos','amiga/common/arm','amiga/arm_debug','amiga/arm_debug/zz9000']
exe=out/'queue-test'
subprocess.run(['clang','-std=c99','-O1','-g','-Wall','-Wextra','-Werror','-Wno-pointer-to-int-cast','-fsanitize=address,undefined','-fno-sanitize-recover=all',*['-I'+str(root/p) for p in inc],str(root/'tests/amiga/dosbox/network/queue_test.c'),'-o',str(exe)],check=True)
subprocess.run([str(exe)],check=True)
exe=out/'control-test'
subprocess.run(['clang','-std=c99','-g','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-fno-sanitize-recover=all','-I'+str(root/'tests/amiga/dosbox/network/mock'),'-I'+str(root/'amiga/dosbox/network'),str(root/'tests/amiga/dosbox/network/control_test.c'),'-o',str(exe)],check=True)
subprocess.run([str(exe)],check=True)
exe=out/'config-test'
subprocess.run(['clang','-std=c99','-g','-fsanitize=address,undefined','-fno-sanitize-recover=all','-I'+str(root/'amiga/dosbox/launcher'),str(root/'tests/amiga/dosbox/network/config_test.c'),'-o',str(exe)],check=True)
subprocess.run([str(exe)],check=True)
exe=out/'ipx-packet-test'
subprocess.run(['clang','-std=c99','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-fno-sanitize-recover=all','-I'+str(root/'amiga/dosbox/network'),str(root/'tests/amiga/dosbox/network/ipx_packet_test.c'),'-o',str(exe)],check=True)
subprocess.run([str(exe)],check=True)
