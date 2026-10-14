#!/usr/bin/env python3
"""Stage an immutable TinyGL subset for the optional ARM software renderer."""
import hashlib,json,shutil,subprocess
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
REV='c2e48591a6bfba1a85f1b87a78dcf1abf7dff57a'
URL='https://github.com/jserv/tinygl.git'
def prepare(out):
 repo=ROOT/'.tools/tinygl-zzdosbox'
 if not repo.exists():subprocess.run(['git','clone','--no-checkout',URL,str(repo)],check=True)
 subprocess.run(['git','-C',str(repo),'cat-file','-e',REV+'^{commit}'],check=True)
 # Archive the pinned tree; ignore the clone's worktree and local edits.
 stage=out/'tinygl';stage.mkdir(parents=True,exist_ok=True)
 archive=subprocess.check_output(['git','-C',str(repo),'archive',REV,'src','include','LICENSE'])
 import io,tarfile
 with tarfile.open(fileobj=io.BytesIO(archive)) as tf:tf.extractall(stage,filter='data')
 p=stage/'include/zfeatures.h';s=p.read_text()
 for feature,value in {'CUSTOM_MALLOC':1,'BLEND':0,'DIRTY_RECTANGLE':0,'MULTITHREADED_DRAWPIXELS':0,'MULTITHREADED_COPY_TEXIMAGE_2D':0}.items():
  import re
  s,n=re.subn(r'(#define TGL_FEATURE_'+feature+r') \d+',r'\g<1> '+str(value),s);assert n==1
 p.write_text(s)
 p=stage/'src/list.c';p.write_text(p.read_text().replace('exit(1);','abort();'))
 p=stage/'src/init.c';s=p.read_text().replace('GLint t2 = 1 << 31;', 'GLuint t2 = 1u << 31;').replace('glInitTextures(c);','glInitTextures();');p.write_text(s)
 p=stage/'src/zraster.c';s=p.read_text();a=s.index('void glopDrawPixels(');b=s.index('\nvoid glPixelZoom(',a)
 patch=ROOT/'amiga/dosbox/opengl/drawpixels.inc';p.write_text(s[:a]+patch.read_text()+s[b:])
 (out/'tinygl-provenance.json').write_text(json.dumps({'url':URL,'revision':REV,'patch_sha256':hashlib.sha256(patch.read_bytes()).hexdigest(),'scope':'partial fixed-function API, software only; no GLSL'},indent=2)+'\n')
 return sorted(p for p in (stage/'src').glob('*.c') if p.name not in ('msghandling.c','memory.c'))
if __name__=='__main__':prepare(ROOT/'.context/amiga/dosbox-opengl')
