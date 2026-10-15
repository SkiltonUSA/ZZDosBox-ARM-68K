#!/usr/bin/env python3
"""Independent FAT16 readback of the disk creator; synthetic files only."""
import pathlib,struct,subprocess,tempfile
ROOT=pathlib.Path(__file__).resolve().parents[1]
def u16(b,o):return struct.unpack_from('<H',b,o)[0]
def u32(b,o):return struct.unpack_from('<I',b,o)[0]
with tempfile.TemporaryDirectory() as t:
 p=pathlib.Path(t); exe=p/'create'; src=p/'dos';src.mkdir();(src/'DOS').mkdir();(src/'DOS'/'TOOLS').mkdir()
 files={'IO.SYS':bytes(range(256))*257,'MSDOS.SYS':b'M'*38138,'COMMAND.COM':b'C'*54645,'DOS/EDIT.COM':b'E'*33333,'DOS/EMPTY.TXT':b'','DOS/TOOLS/HELP.TXT':b'H'*20000}
 for name,data in files.items():(src/name).write_bytes(data)
 boot=bytearray(512);boot[510:]=b'\x55\xaa';struct.pack_into('<H',boot,11,512);boot[54:62]=b'FAT16   ';boot[480:502]=b'IO      SYSMSDOS   SYS'
 mbr=bytearray(512);mbr[510:]=b'\x55\xaa';mbr[450]=6;struct.pack_into('<II',mbr,454,32,65504)
 source=p/'source.img';source.write_bytes(mbr+bytes(16384-512)+boot)
 subprocess.run(['clang','-std=c99','-Wall','-Wextra','-Werror','-fsanitize=address,undefined','-I'+str(ROOT/'amiga/dosbox/launcher'),str(ROOT/'tests/amiga/dosbox/image_create.c'),'-o',str(exe)],check=True)
 for size in [32,64,128,256]:
  image=p/'new.img';subprocess.run([str(exe),str(image),str(source),str(src),str(size)],check=True)
  source_copy=p/'raw-source.img'
  if size==32:source_copy.write_bytes(image.read_bytes())
  subprocess.run([str(exe),str(image),str(source_copy),'-',str(size)],check=True)
  with image.open('rb') as f:
   def read(at,n):f.seek(at);b=f.read(n);assert len(b)==n;return b
   assert image.stat().st_size==size*1048576
   m=read(0,512);assert m[446]==128 and u32(m,454)==32 and u32(m,458)==size*2048-32
   b=read(16384,512);spc=b[13];spf=u16(b,22);total=u16(b,19) or u32(b,32);root=16384+(1+2*spf)*512;data=root+16384;clusters=(total-1-2*spf-32)//spc
   assert 4085<=clusters<65525 and spf*256>=clusters+2
   fat=read(16896,spf*512);assert fat==read(16896+spf*512,spf*512)
   seen=set()
   def chain(c):
    out=bytearray();local=set()
    while c<0xfff8:
     assert 2<=c<clusters+2 and c not in local and c not in seen
     local.add(c);seen.add(c);out+=read(data+(c-2)*spc*512,spc*512);c=u16(fat,c*2)
    return bytes(out)
   found={}
   def directory(buf,prefix=''):
    for i in range(0,len(buf),32):
     e=buf[i:i+32]
     if not e[0]:break
     name=e[:8].decode().strip();ext=e[8:11].decode().strip();name+=('.'+ext if ext else '')
     if name in ('.','..'):continue
     c=u16(e,26);n=u32(e,28);payload=chain(c) if c else b''
     if e[11]&16:directory(payload,prefix+name+'/')
     else:found[prefix+name]=payload[:n];assert len(payload)>=n
   entries=read(root,16384);assert entries[:11]==b'IO      SYS' and entries[32:43]==b'MSDOS   SYS'
   directory(entries)
   for name,content in files.items():assert found[name]==content,name
   assert b'PATH C:\\DOS\r\n' in found['AUTOEXEC.BAT']
   assert b'FILES=40\r\n' in found['CONFIG.SYS']
  print(f'{size} MB: boot geometry, FAT copies/chains, DOS files and startup text passed')
 for size in [32,64,128,256]:
  image=p/'dos5.img';subprocess.run([str(exe),str(image),'-','-',str(size)],check=True)
  with image.open('rb') as f:
   assert image.stat().st_size==size*1048576
   f.seek(446);assert f.read(1)==b'\0'
   f.seek(16384);b=f.read(512);spf=u16(b,22);f.seek(16384+(1+2*spf)*512);root=f.read(16384)
   assert b'IO      SYS' not in root and b'MSDOS   SYS' not in root
   assert b'AUTOEXECBAT' in root and b'CONFIG  SYS' in root
  print(f'{size} MB DOSBox DOS 5: independent data image, no Microsoft files')
 corrupt=p/'corrupt.img';bad=bytearray(source_copy.read_bytes());struct.pack_into('<H',bad,16896+4,2);corrupt.write_bytes(bad)
 assert subprocess.run([str(exe),str(p/'badchain.img'),str(corrupt),'-','32']).returncode==1
 assert subprocess.run([str(exe),str(p/'cancel.img'),str(source),str(src),'32','cancel']).returncode==1
 assert subprocess.run([str(exe),str(p/'bad.img'),str(source),str(src),'512']).returncode==1
 print('Cancellation and unsupported-size checks passed')
