#!/usr/bin/env python3
"""Sanitize TinyGL's private presentation path against an independent image oracle."""
import subprocess
from pathlib import Path
from prepare_dosbox_opengl import ROOT,prepare
out=ROOT/'.context/amiga/dosbox-opengl';sources=prepare(out)
subprocess.run(['clang','-std=c99','-O2','-g','-fsanitize=address,undefined','-fno-sanitize-recover=all','-I'+str(out/'tinygl/include'),'-I'+str(ROOT/'amiga/dosbox/opengl'),*map(str,sources),str(ROOT/'amiga/dosbox/opengl/messages.c'),str(ROOT/'amiga/dosbox/opengl/renderer.c'),str(ROOT/'tests/amiga/dosbox/opengl.c'),'-lm','-o',str(out/'test-native')],check=True)
subprocess.run([str(out/'test-native')],check=True)
