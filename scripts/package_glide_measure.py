#!/usr/bin/env python3
"""Package fixed measurement firmware and a supplied FPGA bitstream offline.

No installation. Input hashes are retained; hardware compatibility and timing
review are separate prerequisites. A supplied stock bitstream is useful only
for checking this packager, not for running the GPU measurement service.
"""
import argparse
import json
from pathlib import Path
import subprocess
from glide_boot_image import BOOT_HEADER_BYTES, sha, partitions, elf_segments, bit_payload, wrap_bitstream

ROOT = Path(__file__).resolve().parents[1]
INPUTS = ROOT / 'amiga/dosbox/glide/validation/measure/boot-inputs.json'


def package(reference, fsbl, firmware, bitstream, output, bootgen, build_record=None):
    pin = json.loads(INPUTS.read_text())
    source, loader, arm, fpga = (p.read_bytes() for p in (reference, fsbl, firmware, bitstream))
    if build_record is not None:
        from build_glide_measure_firmware import PIN
        built=json.loads(build_record.read_text())
        if built['base']!=PIN or built['opcode']!='0x7a20' or built['remaining_bytes']<0:
            raise ValueError('Unrecognized firmware build provenance')
        for name,digest in built['sources'].items():
            if sha((ROOT/name).read_bytes())!=digest:
                raise ValueError('Source changed since firmware build: '+name)
        pin['firmware_elf_sha256']=built['elf_sha256']
    for data, key in [(source, 'reference_boot_sha256'), (loader, 'fsbl_elf_sha256'),
                      (arm, 'firmware_elf_sha256')]:
        if sha(data) != pin[key]:
            raise ValueError('Input differs from recorded measurement build: ' + key)
    original = partitions(source)
    if len(original) != 5 or original[0][:3] != (0, 0, 0x10) or original[1][:3] != (0, 0, 0x20):
        raise ValueError('Unexpected reference layout')
    expected = original[:1] + [(0, 0, 0x20, bit_payload(fpga))] + elf_segments(arm)
    out = output.resolve()
    out.mkdir(parents=True, exist_ok=False)
    (out / 'preserved-fsbl.elf').write_bytes(loader)
    (out / 'preserved.bit').write_bytes(wrap_bitstream(original[1][3]))
    prefix = 'the_ROM_image:\n{\n [bootloader] preserved-fsbl.elf\n'
    body = prefix + ' preserved.bit\n'
    for index, (load, entry, attributes, data) in enumerate(original[2:]):
        name = f'original-{index}.bin'
        (out / name).write_bytes(data)
        body += f' [load={load:#x}, startup={entry:#x}] {name}\n'

    def generate(name, bif):
        (out / (name + '.bif')).write_text(bif + '}\n')
        command = [str(bootgen), '-arch', 'zynq', '-image', name + '.bif', '-o', name + '.bin']
        run = subprocess.run(command, cwd=out, text=True, capture_output=True)
        (out / (name + '.log')).write_text(run.stdout + run.stderr)
        run.check_returncode()
        return (out / (name + '.bin')).read_bytes()

    roundtrip = generate('roundtrip', body)
    if partitions(roundtrip) != original or roundtrip[:BOOT_HEADER_BYTES] != source[:BOOT_HEADER_BYTES]:
        raise ValueError('Reference payload or boot header changed')
    (out / 'candidate.bit').write_bytes(fpga)
    (out / 'measurement.elf').write_bytes(arm)
    candidate = generate('BOOT_GLIDE_MEASURE_EXPERIMENTAL', prefix + ' candidate.bit\n measurement.elf\n')
    if partitions(candidate) != expected or candidate[:BOOT_HEADER_BYTES] != source[:BOOT_HEADER_BYTES]:
        raise ValueError('Packaged payload differs from input, or boot header changed')
    record = {
        'scope': 'Offline packaging only; not hardware validated or installed',
        'reference_boot_sha256': sha(source), 'fsbl_elf_sha256': sha(loader),
        'firmware_elf_sha256': sha(arm), 'fpga_bit_sha256': sha(fpga),
        'bootgen_sha256': sha(bootgen.read_bytes()),
        'build_record_sha256': sha(build_record.read_bytes()) if build_record else None,
        'boot_bytes': len(candidate), 'boot_sha256': sha(candidate),
        'reference_roundtrip_payloads_and_boot_header_identical': True,
        'candidate_payloads_match_inputs': True,
        'fpga_is_reference_payload': expected[1] == original[1],
        'partitions': [{'load': hex(p[0]), 'entry': hex(p[1]), 'attributes': hex(p[2]),
                        'bytes': len(p[3]), 'sha256': sha(p[3])} for p in expected],
    }
    (out / 'package.json').write_text(json.dumps(record, indent=2) + '\n')
    return record


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    for name in ('reference', 'fsbl', 'firmware', 'bitstream', 'output', 'bootgen'):
        parser.add_argument('--' + name, type=Path, required=True)
    parser.add_argument('--build-record',type=Path)
    args = parser.parse_args()
    args.bootgen = args.bootgen.resolve()
    result = package(**vars(args))
    print(json.dumps(result, indent=2))


if __name__ == '__main__':
    main()
