#!/usr/bin/env python3
"""Build a read-only PS DMA survey, separate from the ZZDosBox player."""
import hashlib
import json
from pathlib import Path
import subprocess
from build_zz9000_debug import unpack_elf
from amiga import PIN

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / '.context/amiga/dosbox/dma-probe'


def run(args):
    subprocess.run([str(x) for x in args], cwd=ROOT, check=True)


def array(name, data):
    return 'static const unsigned char ' + name + '[]={' + ','.join(map(str, data)) + '};\n'


def main():
    OUT.mkdir(parents=True, exist_ok=True)
    includes = ['-Iamiga/arm_debug', '-Iamiga/arm_debug/zz9000', '-Iamiga/dosbox/dma']
    flags = ['--target=arm-none-eabi', '-mcpu=cortex-a9', '-marm', '-mfloat-abi=soft',
             '-ffreestanding', '-fno-builtin', '-fPIC', '-fvisibility=hidden',
             '-O2', '-g', '-Wall', '-Wextra', '-Werror', *includes]
    ld = '/opt/homebrew/opt/lld/bin/ld.lld'
    link = [ld, '-shared', '-Bsymbolic', '--no-undefined', '-T',
            'amiga/arm_debug/zz9000/payload.ld']
    sources = ['amiga/arm_debug/zz9000/entry.S', 'amiga/dosbox/dma/probe.c']
    objects = []
    for source in sources:
        obj = OUT / (Path(source).stem + '.o')
        run(['clang', *flags, '-c', source, '-o', obj])
        objects.append(obj)
    run([*link, *objects, '-o', OUT / 'worker.elf'])
    payload, relocations, entry = unpack_elf((OUT / 'worker.elf').read_bytes())
    mapping_source = 'amiga/arm_debug/zz9000/mapping_probe.S'
    run(['clang', *flags, '-c', mapping_source, '-o', OUT / 'mapping.o'])
    run([*link, OUT / 'mapping.o', '-o', OUT / 'mapping.elf'])
    mapping, reloc, point = unpack_elf((OUT / 'mapping.elf').read_bytes())
    assert not reloc and not point and len(mapping) <= 1024
    (OUT / 'zz_payload.h').write_text(
        f'#define ZZ_ENTRY {entry}u\n' + array('zz_image', payload) +
        array('zz_mapping_probe', mapping) +
        'static const unsigned long zz_relocations[]={' + ','.join(map(str, relocations)) + '};\n')
    relative = OUT.relative_to(ROOT)
    host = ['amiga/arm_debug/zz9000/launcher.c', 'amiga/dosbox/dma/probe_host.c']
    run(['podman', '--connection', 'nuflix-converter-root', 'run', '--rm', '--platform',
         'linux/amd64', '-v', f'{ROOT}:/work', '-w', '/work', PIN['image'],
         'm68k-amigaos-gcc', '-std=c99', '-noixemul', '-m68020', '-O2', '-g',
         '-Wall', '-Wextra', '-Werror', '-DZZ_HEADLESS_BENCH', '-DZZ_RELEASE',
         '-DZZ_APP_NAME="ZZDosBox-DMA-Probe"', '-DZZ_APP_VERSION="0.1"',
         '-DZZ_MIN_STACK=32768', *includes, '-I' + str(relative), *host,
         '-lamiga', '-o', relative / 'ZZDosBox-DMA-Probe'])
    binary = OUT / 'ZZDosBox-DMA-Probe'
    assert binary.read_bytes()[:4] == b'\x00\x00\x03\xf3'
    sources += host + [mapping_source, 'amiga/dosbox/dma/probe_wire.h',
                       'amiga/arm_debug/zz9000/layout.h', 'scripts/build_dosbox_dma_probe.py']
    manifest = dict(kind='read-only-register-survey', dma_submissions=0,
                    executable_sha256=hashlib.sha256(binary.read_bytes()).hexdigest(),
                    sources={p: hashlib.sha256((ROOT/p).read_bytes()).hexdigest() for p in sources})
    (OUT / 'build.json').write_text(json.dumps(manifest, indent=2) + '\n')
    print(binary)


if __name__ == '__main__':
    main()
