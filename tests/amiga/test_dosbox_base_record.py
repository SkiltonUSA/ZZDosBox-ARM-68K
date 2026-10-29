"""A recorded base must reject version/core/artifact drift before building."""
import hashlib,json,re,subprocess,tempfile
from pathlib import Path
root=Path(__file__).resolve().parents[2]
version=re.search(r'#define ZZ_DOSBOX_VERSION "([^"]+)"',(root/'amiga/dosbox/version.h').read_text())[1]
with tempfile.TemporaryDirectory(dir=root/'.context') as d:
 p=Path(d);artifact=p/'player';artifact.write_bytes(b'fixture')
 record={'version':version,'upstream':json.loads((root/'amiga/dosbox/upstream.json').read_text()),'outputs':{'player':hashlib.sha256(artifact.read_bytes()).hexdigest()},'build_options':{}}
 def rejected(changed,phrase):
  (p/'build.json').write_text(json.dumps(changed))
  r=subprocess.run(['python3',str(root/'scripts/build_dosbox_dos_arm.py'),'--base',str(p),'--output',str(p/'out')],capture_output=True,text=True)
  assert r.returncode and phrase in r.stderr,(r.returncode,r.stderr)
 rejected(dict(record,version='old-version'),'Base product version differs')
 rejected(dict(record,outputs={'player':'0'*64}),'Base artifact changed')
 rejected(dict(record,upstream={'revision':'different'}),'Base core pin differs')
 changed=dict(record);del changed['build_options'];rejected(changed,'recorded options')
 assert not (p/'out').exists()
print('PASS base manifest: reject version/core/artifact drift and unrecorded flags before build')
