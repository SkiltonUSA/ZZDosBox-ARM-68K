#!/usr/bin/env python3
"""Compile unmodified pinned DOSBox relay with a test-only POSIX adapter."""
import subprocess,shutil
from pathlib import Path
R=Path(__file__).resolve().parents[1];O=R/'.context/amiga/dosbox-ipx/upstream-relay';O.mkdir(parents=True,exist_ok=True)
U=R/'.context/research/dosbox-libretro';T=R/'tests/amiga/dosbox/network/upstream_relay'
for n in ['ipx.h','ipxserver.h']:shutil.copy2(U/'include'/n,O/n)
shutil.copy2(R/'amiga/dosbox/network/ipx_sdl.h',O/'SDL_net.h')
subprocess.run(['clang++','-std=c++17','-O1','-g','-fsanitize=address,undefined','-I'+str(T),'-I'+str(O),str(U/'src/hardware/ipxserver.cpp'),str(T/'host.cpp'),'-o',str(O/'ipxrelay')],check=True)
print(O/'ipxrelay')
