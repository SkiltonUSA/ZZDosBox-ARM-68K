#!/usr/bin/env python3
"""Exercise the firmware completion reader against the real mailbox RTL."""
import hashlib
import json
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
EXP = ROOT / 'amiga/dosbox/glide/experimental'
OUT = ROOT / '.context/amiga/glide/completion-client'
TEST = ROOT / 'tests/amiga/dosbox/glide/completion_client.cpp'


def run(command, label):
    result = subprocess.run(list(map(str, command)), cwd=ROOT,
                            capture_output=True, text=True, timeout=300)
    log = (result.stdout + result.stderr).replace(str(ROOT) + '/', '')
    (OUT / (label + '.log')).write_text(log)
    if result.returncode:
        raise SystemExit(label + ' failed:\n' + log[-3000:])
    return log


def main():
    OUT.mkdir(parents=True, exist_ok=True)
    (OUT / 'results.json').unlink(missing_ok=True)
    client = EXP / 'completion_client.c'
    obj = OUT / 'client.o'
    flags = ['-std=c11', '-O2', '-Wall', '-Wextra', '-Werror']
    run(['clang', *flags, '-c', client, '-o', obj], 'compile')
    rtl = [EXP / name for name in ('completion.v', 'completion_axil.v')]
    binary = OUT / 'build/Vgrx_completion_axil'
    binary.unlink(missing_ok=True)  # Force relink even within make's timestamp resolution.
    run(['verilator', '--cc', '--exe', '--build', '-j', '2', '-Wno-fatal',
         '--top-module', 'grx_completion_axil', '--Mdir', OUT / 'build',
         '-CFLAGS', '-I' + str(ROOT), TEST, obj, *rtl], 'build')
    log = run([binary], 'test')
    if log.count('PASS completion client:') != 6:
        raise SystemExit('Missing completion-client test results')
    for abi in ('soft', 'softfp'):
        run(['arm-none-eabi-gcc', *flags, '-mcpu=cortex-a9', '-marm',
             '-mfpu=vfpv3', '-mfloat-abi=' + abi, '-ffreestanding',
             '-fno-builtin', '-c', client, '-o', OUT / (abi + '.o')], abi)
    # Removing the last status observation must lose the late-fault guarantee.
    mutant = OUT / 'no-final-status.c'
    text = client.read_text()
    marker = 'if (!io->read(io->context, STATUS, &status) || status != 0)'
    if text.count(marker) != 1:
        raise SystemExit('Final-observation mutation anchor changed')
    mutant.write_text(text.replace(marker, 'if (0)'))
    run(['clang', *flags, '-I' + str(EXP), '-c', mutant, '-o', obj], 'mutant-compile')
    binary.unlink()
    run(['make', '-C', OUT / 'build', '-f', 'Vgrx_completion_axil.mk'], 'mutant-link')
    result = subprocess.run([str(binary)],
                            capture_output=True, text=True, timeout=30)
    (OUT / 'mutant.log').write_text(result.stdout + result.stderr)
    if result.returncode != 1 or 'FAIL wrong completion state' not in result.stderr:
        raise SystemExit('Missing final status observation was not detected')
    binary.replace(OUT / 'completion-client-mutant')
    run(['clang', *flags, '-c', client, '-o', obj], 'restore-compile')
    run(['make', '-C', OUT / 'build', '-f', 'Vgrx_completion_axil.mk'], 'restore-link')
    run([binary], 'restored-test')
    print(log.strip())
    print('PASS mutation: missing final fault observation detected')
    sources = [client, EXP / 'completion_client.h', TEST, *rtl, Path(__file__)]
    record = {
        'scope': 'C completion reader against mailbox RTL; no hardware MMIO, submission, DDR or FPS',
        'scenarios': 20,
        'arm_compile_only': ['Cortex-A9 ARM32 soft', 'Cortex-A9 ARM32 softfp'],
        'missing_final_observation_mutation_detected': True,
        'sha256': {str(p.relative_to(ROOT)): hashlib.sha256(p.read_bytes()).hexdigest()
                   for p in sources},
    }
    (OUT / 'results.json').write_text(json.dumps(record, indent=2) + '\n')


if __name__ == '__main__':
    main()
