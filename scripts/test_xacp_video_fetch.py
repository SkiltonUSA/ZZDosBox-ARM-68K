#!/usr/bin/env python3
"""Exercise the unchanged video formatter's fetch consumer, not complete scanout."""
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
    bench = Path(__file__).resolve().parents[1] / 'tests/amiga/dosbox/glide/video_fetch.v'
    compiler = [args.iverilog]
    if args.ivl_dir:
        compiler += ['-B', str(args.ivl_dir)]
    cases, stress, mutations = [], [], []
    with tempfile.TemporaryDirectory() as directory:
        directory = Path(directory)
        image = directory / 'fetch.vvp'

        def run(source, scale=0, skew=0, phase=1000):
            subprocess.run(compiler + ['-g2012', '-s', 'video_fetch_test',
                f'-Pvideo_fetch_test.SCALE={scale}', f'-Pvideo_fetch_test.SKEW={skew}',
                f'-Pvideo_fetch_test.PHASE={phase}', '-o', str(image), str(bench), str(source)],
                check=True, capture_output=True, text=True)
            result = subprocess.run([args.vvp, str(image)], capture_output=True, text=True)
            result.stdout = result.stdout.replace(str(bench), bench.name)
            return result

        def rejected(result):
            if result.returncode == 0 or 'FATAL:' not in result.stdout:
                raise RuntimeError('Expected runtime rejection: ' + result.stdout + result.stderr)
            return result.stdout.splitlines()[0]

        for scale in (0, 1):
            for skew in (0, 1, 3):
                for phase in (1000, 3500, 9000):
                    result = run(args.source, scale, skew, phase)
                    if result.returncode or not result.stdout.startswith('PASS '):
                        raise RuntimeError(result.stdout + result.stderr)
                    cases.append(result.stdout.splitlines()[0])
            stress.append({'scale': scale, 'inconsistent_sample_cycles': 24,
                           'failure': rejected(run(args.source, scale, 24))})
        source = args.source.read_text()
        for name, old, new in (
            ('ignore-line-request', 'need_line_fetch_reg2!=last_line_fetch', "1'b0"),
            ('ignore-frame-gate', 'need_frame_sync_reg==1', "1'b1"),
            ('corrupt-payload', 'line_buffer[inptr] <= pixin;', "line_buffer[inptr] <= pixin ^ 32'h1;"),
        ):
            if source.count(old) != 1:
                raise ValueError(f'Unexpected source for {name}')
            mutant = directory / 'mutant.v'
            mutant.write_text(source.replace(old, new))
            mutations.append({'name': name, 'failure': rejected(run(mutant))})
    record = {'source_sha256': hashlib.sha256(args.source.read_bytes()).hexdigest(),
              'test_sha256': hashlib.sha256(bench.read_bytes()).hexdigest(),
              'cases': cases, 'injected_long_skew': stress, 'mutations': mutations,
              'scope': 'Fetch consumer only; forced beam request and frame-sync stimuli, known initial state, 16-word lines; no reset, full scanout, physical skew or metastability validation'}
    text = json.dumps(record, indent=2) + '\n'
    if args.output:
        args.output.write_text(text)
    print(text, end='')


if __name__ == '__main__':
    main()
