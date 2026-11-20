#!/usr/bin/env python3
"""Test the actual Vivado-generated M09 clock converter with the completion endpoint."""
import argparse
import hashlib
import json
import subprocess
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
EXP = ROOT / 'amiga/dosbox/glide/experimental'


def one(paths):
    paths = list(paths)
    if len(paths) != 1:
        raise SystemExit('Expected one generated input: ' + str(paths))
    return paths[0]


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--generated', required=True, type=Path, help='Generated zz9000_ps directory')
    p.add_argument('--glbl', required=True, type=Path, help='Installed glbl.v')
    p.add_argument('--xpm', required=True, type=Path, help='Installed xpm_cdc.sv')
    p.add_argument('--output', type=Path, default=ROOT / '.context/amiga/glide/endpoint-cdc')
    a = p.parse_args()
    a.output.mkdir(parents=True, exist_ok=True)
    (a.output / 'results.json').unlink(missing_ok=True)
    wrapper = one(a.generated.glob('ip/*auto_cc_1/sim/*auto_cc_1.v'))
    cc = one(a.generated.glob('ipshared/*/hdl/axi_clock_converter*_vl_rfs.v'))
    infrastructure = one(a.generated.glob('ipshared/*/hdl/axi_infrastructure*_vl_rfs.v'))
    header = one(a.generated.glob('ipshared/*/hdl/axi_infrastructure*.vh'))
    inputs = [ROOT / 'tests/amiga/dosbox/glide/completion_cdc.v', wrapper, cc,
              infrastructure, a.xpm, a.glbl] + [EXP / name for name in (
                  'completion.v', 'completion_axil.v', 'completion_endpoint.v', 'response_token.v')]
    command = ['verilator', '--binary', '--timing', '-j', '4', '-Wno-fatal',
               '--top-module', 'completion_cdc_test', '--Mdir', str(a.output / 'obj'),
               '-I' + str(header.parent.resolve())] + [str(p.resolve()) for p in inputs]
    build = subprocess.run(command, capture_output=True, text=True)
    (a.output / 'build.log').write_text(build.stdout + build.stderr)
    if build.returncode:
        raise SystemExit('CDC compile failed: ' + str(a.output / 'build.log'))
    run = subprocess.run([str(a.output / 'obj/Vcompletion_cdc_test')],
                         capture_output=True, text=True, timeout=60)
    log = run.stdout + run.stderr
    (a.output / 'run.log').write_text(log)
    if run.returncode or log.count('PASS CDC:') != 6:
        raise SystemExit('CDC simulation failed: ' + log)
    result = {'scope': 'Generated M09 AXI CDC plus endpoint; no PS7, HP3, GPU or metastability model',
              'cases': 6, 'cpu_mhz': 25, 'gpu_mhz': 100, 'gpu_phase_ns': 3,
              'sha256': {p.name: hashlib.sha256(p.read_bytes()).hexdigest()
                         for p in [Path(__file__).resolve(), header, *inputs]}}
    (a.output / 'results.json').write_text(json.dumps(result, indent=2) + '\n')
    print(log)


if __name__ == '__main__':
    main()
