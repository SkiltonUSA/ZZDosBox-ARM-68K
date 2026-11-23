#!/usr/bin/env python3
"""Runtime timer calibration and presentation regressions (native, not hardware)."""
from pathlib import Path
import subprocess,tempfile
ROOT=Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory() as directory:
 for name in ("clock_rate","presentation"):
  exe=Path(directory)/name
  subprocess.run(["clang++","-std=c++11","-O1","-g","-fsanitize=address,undefined","-I"+str(ROOT/"amiga/dosbox/pc"),str(ROOT/"tests/amiga/dosbox"/(name+".cpp")),"-o",str(exe)],check=True)
  subprocess.run([str(exe)],check=True)
