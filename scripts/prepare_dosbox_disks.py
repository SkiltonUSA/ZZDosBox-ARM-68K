#!/usr/bin/env python3
"""Generate read-only FAT12 Workbench mounts, without bundling disk images.

Requires Thore Boeckelmann's diskimage.device 37.7 and fat95 on the Amiga.
https://aminet.net/package/disk/misc/diskimage
"""
import argparse
from pathlib import Path


def prepare(out, base):
    if any(c in base for c in '\n\r"*'):
        raise ValueError('Use a plain Amiga drawer path')
    out.mkdir(parents=True, exist_ok=True)
    devices = [('DOSD1', 'Disk1.img'), ('DOSD2', 'Disk2.img'),
               ('DOSD3', 'Disk3.img'), ('DOSSUP', 'Suppdisk.img'), ('DOSBOOT', 'DosBoot.img')]
    mount, eject = ["FailAt 21"], []
    for unit, (device, image) in enumerate(devices):
        (out/device).write_text(f'''FileSystem = L:fat95
Device = diskimage.device
Unit = {unit}
Flags = 0
Surfaces = 2
SectorsPerTrack = 18
SectorSize = 512
Reserved = 0
Interleave = 0
LowCyl = 0
HighCyl = 79
Buffers = 10
BufMemType = 1
StackSize = 8192
Priority = 5
GlobVec = -1
DosType = 0x46415400
Activate = 1
Mask = 0x7ffffffe
MaxTransfer = 0x1fe00
''')
        mount.extend([f'Mount "{base}/Mounts/{device}"',
                      f'"{base}/DiskImageCtrl" DRIVE {device}: READONLY INSERT "{base}/{image}"',
                      'IF WARN', ' Quit 20', 'ENDIF'])
        eject.extend([f'"{base}/DiskImageCtrl" DRIVE {device}: EJECT', 'IF WARN', ' Quit 20', 'ENDIF'])
    # Unit 5 is reserved for the fixed-geometry 32 MiB BIOS HDD.
    # FAT95 discovers the primary partition from the MBR (DosType 0x46415401).
    (out/'DOSHD').write_text("""FileSystem = L:fat95
Device = diskimage.device
Unit = 5
Flags = 0
Surfaces = 16
SectorsPerTrack = 32
SectorSize = 512
Reserved = 0
Interleave = 0
LowCyl = 0
HighCyl = 127
Buffers = 20
BufMemType = 1
StackSize = 8192
Priority = 5
GlobVec = -1
DosType = 0x46415401
Activate = 1
Mask = 0x7ffffffe
MaxTransfer = 0x1fe00
""")
    (out/'Mount-DOS-Disks').write_text('\n'.join(mount)+'\n')
    (out/'Eject-DOS-Disks').write_text('\n'.join(eject)+'\n')


if __name__ == '__main__':
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--output', type=Path, default=Path('.context/amiga/dosbox-launcher/disks'))
    p.add_argument('--drawer', default='SD032G:Dev/ZZDosBox/PC/Disks')
    a = p.parse_args()
    prepare(a.output, a.drawer)
