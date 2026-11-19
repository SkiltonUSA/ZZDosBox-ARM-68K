#!/usr/bin/env python3
"""Digital-only sampling test of a supplied upstream audio_clock.v; no hardware sign-off."""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
import tempfile


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('source', type=Path)
    parser.add_argument('--iverilog', default='iverilog')
    parser.add_argument('--vvp', default='vvp')
    parser.add_argument('--ivl-dir', type=Path)
    parser.add_argument('--output', type=Path)
    args = parser.parse_args()
    bench = Path(__file__).resolve().parents[1] / 'tests/amiga/dosbox/glide/audio_clock.v'
    source = args.source.read_text()
    compiler = [args.iverilog]
    if args.ivl_dir:
        compiler += ['-B', str(args.ivl_dir)]
    results = []
    with tempfile.TemporaryDirectory() as directory:
        directory = Path(directory)
        image = directory / 'clock.vvp'

        def run(path, half, phase):
            subprocess.run(compiler + ['-g2012', '-s', 'audio_clock_test',
                f'-Paudio_clock_test.HALF_PS={half}', f'-Paudio_clock_test.PHASE_PS={phase}',
                '-o', str(image), str(bench), str(path)], check=True, capture_output=True, text=True)
            return subprocess.run([args.vvp, str(image)], capture_output=True, text=True)

        for half in (40000, 40690, 41000):
            for phase in (1000, 3500, 9000):
                result = run(args.source, half, phase)
                if result.returncode or not result.stdout.startswith('PASS '):
                    raise RuntimeError(result.stdout + result.stderr)
                results.append(result.stdout.splitlines()[0])
        mutations = []
        for name, old, new in (
            ('missing-sampling-stage', 'assign mclk_out = clkbuf[2];', 'assign mclk_out = clkbuf[1];'),
            ('wrong-divider', 'assign bclk_out = clkgen[2];', 'assign bclk_out = clkgen[1];'),
        ):
            if source.count(old) != 1:
                raise ValueError(f'Unexpected source for mutation {name}')
            mutant = directory / 'mutant.v'
            mutant.write_text(source.replace(old, new))
            result = run(mutant, 40000, 1000)
            if result.returncode == 0 or 'FATAL:' not in result.stdout:
                raise RuntimeError(f'Mutation not detected: {name}')
            failure = result.stdout.splitlines()[0].replace(str(bench), bench.name)
            mutations.append({'name': name, 'detected': True, 'failure': failure})
    record = {'source_sha256': hashlib.sha256(args.source.read_bytes()).hexdigest(),
              'test_sha256': hashlib.sha256(bench.read_bytes()).hexdigest(),
              'cases': results, 'mutations': mutations,
              'initial_state': 'Explicit clkbuf=0, clkgen=0; no reset or startup validation',
              'scope': 'Ideal digital RTL simulation; no metastability, analog timing, or hardware performance claim'}
    text = json.dumps(record, indent=2) + '\n'
    if args.output:
        args.output.write_text(text)
    print(text, end='')


if __name__ == '__main__':
    main()
