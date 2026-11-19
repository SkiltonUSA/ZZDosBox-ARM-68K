#!/usr/bin/env python3
"""Test upstream RasterIX parser fence semantics; no FPGA or Amiga access."""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / '.context/amiga/glide/fence-model'


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--rtl', type=Path, required=True)
    p.add_argument('--iverilog', default='iverilog')
    p.add_argument('--vvp', default='vvp')
    p.add_argument('--ivl-dir', type=Path)
    p.add_argument('--axi', action='store_true', help='Also test bypass Coalescer write retirement')
    a = p.parse_args()
    OUT.mkdir(parents=True, exist_ok=True)
    source = a.rtl.resolve() / 'CommandParser.v'
    definitions = a.rtl.resolve() / 'RegisterAndDescriptorDefines.vh'
    bench = ROOT / 'tests/amiga/dosbox/glide/fence.v'
    compiler = [a.iverilog]
    runtime = [a.vvp]
    if a.ivl_dir:
        compiler += ['-B', str(a.ivl_dir.resolve())]
        runtime += ['-M', str(a.ivl_dir.resolve())]

    def simulate(path, name, testbench=bench, top='fence_test', extra=()):
        binary = OUT / (name + '.vvp')
        subprocess.run(compiler + ['-g2012', '-s', top, '-I', str(a.rtl.resolve()),
                                  '-o', str(binary), str(testbench), str(path)] +
                       [str(x) for x in extra], check=True)
        result = subprocess.run(runtime + [str(binary)], capture_output=True, text=True)
        text = (result.stdout + result.stderr).replace(str(ROOT) + '/', '')
        (OUT / (name + '.txt')).write_text(text)
        print(text, end='')
        return result.returncode, text

    rc, text = simulate(source, 'upstream')
    if rc or text.count('PASS parser fence:') != 4:
        raise SystemExit('Upstream fence scenarios failed')
    # Prove the test observes transaction retirement, not just command decoding.
    original = source.read_text()
    condition = 'if (!framebufferWritesPending && !textureMemoryTransactionsPending)'
    if original.count(condition) != 1:
        raise SystemExit('Unexpected upstream parser; review before mutation')
    mutated = OUT / 'without-wait.v'
    mutated.write_text(original.replace(condition, "if (1'b1)"))
    rc, text = simulate(mutated, 'without-wait')
    if not rc or 'Passed fence with transactions pending' not in text:
        raise SystemExit('Test did not detect removed retirement wait')
    print('PASS mutation: removing retirement wait is detected')
    record = {'scope': 'CommandParser simulation with injected pending signals; not full AXI or installed FPGA',
              'scenarios': 4, 'removed_wait_detected': True,
              'sources': {str(x.relative_to(ROOT)) if x.is_relative_to(ROOT) else x.name:
                          hashlib.sha256(x.read_bytes()).hexdigest()
                          for x in (source, definitions, bench, Path(__file__))}}
    if a.axi:
        coalescer = a.rtl.resolve() / 'Coalescer.v'
        tracker = a.rtl.resolve() / 'ValueTrack.v'
        axi_bench = ROOT / 'tests/amiga/dosbox/glide/fence_axi.v'
        rc, text = simulate(source, 'axi-upstream', axi_bench, 'fence_axi_test',
                            (coalescer, tracker))
        if rc or text.count('PASS AXI fence:') != 5:
            raise SystemExit('AXI write retirement scenarios failed')
        original = coalescer.read_text()
        condition = '| outstanding_write_active;'
        if original.count(condition) != 1:
            raise SystemExit('Unexpected bypass tracker; review before mutation')
        mutated = OUT / 'without-response-tracking.v'
        mutated.write_text(original.replace(condition, ';'))
        rc, text = simulate(source, 'without-response-tracking', axi_bench,
                            'fence_axi_test', (mutated, tracker))
        if not rc or 'Passed fence with transactions pending' not in text:
            raise SystemExit('Test did not detect removed response tracking')
        print('PASS mutation: removing AXI response tracking is detected')
        record['axi'] = {'scope': 'Parser plus bypass Coalescer and ValueTrack; simulated AXI slave',
                         'scenarios': 5, 'removed_response_tracking_detected': True,
                         'sources': {x.name: hashlib.sha256(x.read_bytes()).hexdigest()
                                     for x in (coalescer, tracker, axi_bench)}}
    (OUT / 'validation.json').write_text(json.dumps(record, indent=2) + '\n')


if __name__ == '__main__':
    main()
