"""Preprocess the real host layout check with complete and incomplete build headers."""
import argparse, shutil, subprocess, tempfile
from pathlib import Path
root=Path(__file__).resolve().parents[1]
p=argparse.ArgumentParser();p.add_argument('build',type=Path);args=p.parse_args()
host=(root/'amiga/dosbox/pc/host.c').read_text()
check='#include "mmu.h"\n'+host.split('#include "mmu.h"\n',1)[1].split('#endif',1)[0]+'#endif\n'
with tempfile.TemporaryDirectory() as tmp:
 d=Path(tmp);(d/'check.c').write_text(check)
 for name in ('mmu.h','mmu_plan.h'):shutil.copy2(args.build/name,d/name)
 command=['clang','-E','-DMB_MMU','-DZZ_CONTROL=0x180000','-DDB_DYNREC','-DDB_EXPANDED','-DDB_JIT_MIB=32','-I'+str(d),'-I'+str(root/'amiga/dosbox/pc'),'-I'+str(root/'amiga/common/arm'),str(d/'check.c')]
 good=subprocess.run(command,capture_output=True,text=True)
 assert good.returncode==0,good.stderr
 (d/'mmu.h').unlink()
 bad=subprocess.run(command,capture_output=True,text=True)
 assert bad.returncode and 'DOSBox host must use its generated MMU layout headers' in bad.stderr,bad.stderr
 print('PASS host layout: generated headers accepted; missing mmu.h cannot select fallback report offsets')
