#!/usr/bin/env python3
"""Pin CMSIS-DSP's matrix implementation for an isolated A9 experiment."""
import hashlib, io, json, subprocess, tarfile
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
REV = '83a2d7bc98c81b4bbe4a6f48b1f2ecf179868a0b'
URL = 'https://github.com/ARM-software/CMSIS-DSP.git'

def prepare(out):
    repo = ROOT / '.tools/cmsis-dsp-matrix'
    if not repo.exists():
        subprocess.run(['git', 'clone', '--filter=blob:none', '--no-checkout', URL, str(repo)], check=True)
    archive = subprocess.check_output(['git', '-C', str(repo), 'archive', REV,
        'Include', 'PrivateInclude', 'Source/MatrixFunctions', 'LICENSE'])
    stage = out / 'cmsis-dsp'
    stage.mkdir(parents=True, exist_ok=True)
    with tarfile.open(fileobj=io.BytesIO(archive)) as tf:
        tf.extractall(stage, filter='data')
    source = stage / 'Source/MatrixFunctions/arm_mat_mult_f32.c'
    (out / 'cmsis-provenance.json').write_text(json.dumps({
        'url': URL, 'revision': REV, 'license': 'Apache-2.0',
        'source_sha256': hashlib.sha256(source.read_bytes()).hexdigest(),
        'matrix_source_hashes': {p.name: hashlib.sha256(p.read_bytes()).hexdigest() for p in sorted(source.parent.glob('*.c'))},
        'patches': [], 'scope': 'arm_mat_mult_f32 and bounded packing buffers; ARM_MATH_NEON; wrappers supply valid shapes',
        'blocking_budgets_bytes': {'L1': 16384, 'L2': 32768, 'L3': 32768},
        'blocking_note': 'Software tile budgets, not detected hardware cache capacities; no shared cache configuration changed',
        'core_dependency': '__GNUC_PYTHON__ selects upstream standalone compiler definitions; no CMSIS startup or hardware configuration'
    }, indent=2) + '\n')
    return [source, source.parent / '_arm_mat_mult_neon_buffers.c']

def prepare_scene(out):
    """Keep the benchmark's rasterizer fix isolated from installed GL builds."""
    from prepare_dosbox_opengl import prepare as prepare_gl
    sources = prepare_gl(out)
    p = out / 'tinygl/src/ztriangle.h'
    text = p.read_text()
    # Screen-clipped dx is bounded by our 640-pixel viewport. Multiplication
    # preserves fixed-point arithmetic without shifting negative signed values.
    for name in ('dx1', 'dx2'):
        old = '(' + name + ' << 16)'
        assert text.count(old) == 1
        text = text.replace(old, '(' + name + ' * 65536)')
    p.write_text(text)
    provenance = out / 'tinygl-provenance.json'
    data = json.loads(provenance.read_text())
    data['matrix_benchmark_patch'] = 'Replace two negative signed edge shifts with bounded multiplication; only diagnostic scene build'
    data['matrix_benchmark_patch_sha256'] = hashlib.sha256(p.read_bytes()).hexdigest()
    provenance.write_text(json.dumps(data, indent=2) + '\n')
    return sources
