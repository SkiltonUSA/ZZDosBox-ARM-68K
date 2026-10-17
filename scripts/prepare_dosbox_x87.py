"""Enable pinned DOSBox portable x87 (binary64), without host x86 assembly."""
import shutil
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
def prepare(out):
    config=out/'include/config.h'
    text=config.read_text()
    assert '#define C_FPU 0' in text
    config.write_text(text.replace('#define C_FPU 0','#define C_FPU 1'))
    shutil.copytree(ROOT/'.context/research/dosbox-libretro/src/fpu',out/'src/fpu',dirs_exist_ok=True)
    return [out/'src/fpu/fpu.cpp']
