#!/usr/bin/env python3
"""Pinned, diagnostic-only hlsl++ comparison. Never changes installed TinyGL."""
import difflib, hashlib, io, json, re, subprocess, tarfile
from pathlib import Path
from prepare_dosbox_matrix import ROOT, prepare_scene
from prepare_dosbox_tinygl_bench import prepare as prepare_internal

REV = '3d9af2e77a89ddd645928f1600a0688dc898dedc'
URL = 'https://github.com/redorav/hlslpp.git'
BASE = ROOT/'amiga/dosbox'

def prepare(out, mode):
    assert mode in ('micro', 'scalar', 'neon')
    repo = ROOT/'.tools/hlslpp-bench'
    if not repo.exists():
        subprocess.run(['git','clone','--filter=blob:none','--no-checkout',URL,str(repo)],check=True)
    subprocess.run(['git','-C',str(repo),'cat-file','-e',REV],check=True)
    archive = subprocess.check_output(['git','-C',str(repo),'archive',REV,'include','LICENSE'])
    stage = out/'hlslpp'; stage.mkdir(parents=True,exist_ok=True)
    with tarfile.open(fileobj=io.BytesIO(archive)) as tf: tf.extractall(stage,filter='data')
    # ARM32 helpers use implicit cross-element vector conversions rejected by
    # GCC 15. Make the intended bit reinterpretation explicit, with no math change.
    header=stage/'include/hlsl++/platforms/neon.h'; original=header.read_text(); fixed=original
    for kind in ('f32','s32','u32'):
        fixed=re.sub(r'uint8x8x([24])_t tbl = \{ \{ (.*?) \} \};',
            lambda m: 'uint8x8x'+m[1]+'_t tbl = { { '+re.sub(r'vget_(low|high)_'+kind+r'\(([xy])\)',lambda n:'vreinterpret_u8_'+kind+'('+n[0]+')',m[2])+' } };',fixed)
        fixed=fixed.replace('vcombine_'+kind+'(rL, rH)', 'vcombine_'+kind+'(vreinterpret_'+kind+'_u8(rL), vreinterpret_'+kind+'_u8(rH))')
    fixed=fixed.replace('vshufq_u32(const uint32x4_t x, const int32x4_t y', 'vshufq_u32(const uint32x4_t x, const uint32x4_t y')
    fixed=fixed.replace('float32x4_t tgtx = vcgtq_f32','uint32x4_t tgtx = vcgtq_f32').replace('float32x4_t tgtx = vcltq_f32','uint32x4_t tgtx = vcltq_f32')
    fixed=fixed.replace('vandq_u32(tgtx, ones)','vreinterpretq_f32_u32(vandq_u32(tgtx, vreinterpretq_u32_f32(ones)))')
    fixed=fixed.replace('vreinterpretq_u32_f32(vmovq_n_s32(0x80000000u))','vreinterpretq_f32_u32(vmovq_n_u32(0x80000000u))')
    fixed=fixed.replace('veorq_u32(vreinterpretq_u32_f32((x)), vmovq_n_u32(0x80000000u))','vreinterpretq_f32_u32(veorq_u32(vreinterpretq_u32_f32((x)), vmovq_n_u32(0x80000000u)))')
    fixed=fixed.replace('int32x2_t t = vld1_u32(src)', 'uint32x2_t t = vld1_u32(src)')
    for op in ('eq','gt','ge','lt','le'):
        pattern=r'(#define _hlslpp_cmp'+op+r'_ps\(x, y\)\s+)(vc'+op+r'q_f32\(\(x\), \(y\)\))'
        fixed,count=re.subn(pattern,r'\1vreinterpretq_f32_u32(\2)',fixed);assert count==1
    fixed=fixed.replace('veorq_u32(vreinterpretq_u32_f32(vceqq_f32((x), (y))), vmovq_n_u32(0xffffffffu))','vreinterpretq_f32_u32(vmvnq_u32(vceqq_f32((x), (y))))')
    fixed=fixed.replace('vbslq_f32((mask), (y), (x))','vbslq_f32(vreinterpretq_u32_f32(mask), (y), (x))')
    fixed=fixed.replace('vqmovun_s32(v255i)','vqmovun_s32(vreinterpretq_s32_u32(v255i))')
    fixed=fixed.replace('uint8x16_t packed8 = vmovq_n_u32(p)','uint8x16_t packed8 = vreinterpretq_u8_u32(vmovq_n_u32(p))')
    fixed=fixed.replace('int8x16_t packed8 = vmovq_n_s32(p)','int8x16_t packed8 = vreinterpretq_s8_s32(vmovq_n_s32(p))')
    fixed=fixed.replace('vcvtq_f32_s32(packed32);\n\treturn vmulq_f32(t, vmovq_n_f32(1.0f / 255.0f))','vcvtq_f32_s32(vreinterpretq_s32_u32(packed32));\n\treturn vmulq_f32(t, vmovq_n_f32(1.0f / 255.0f))')
    for name in ('any','all'):
        for n in range(1,5):
            fixed=fixed.replace('_hlslpp_'+name+str(n)+'_epi32(vreinterpretq_u32_f32(x))','_hlslpp_'+name+str(n)+'_epi32(vreinterpretq_s32_f32(x))')
            pattern=r'(hlslpp_inline bool _hlslpp_'+name+str(n)+r'_epi32\(n128i x\)\n\{)(.*?)(\n\})'
            def convert(m):
                body=m[2].replace('vget_low_u32(x)','vget_low_u32(ux)').replace('vget_high_u32(x)','vget_high_u32(ux)').replace(', x, 3)',', ux, 3)').replace('vq_u32(x)','vq_u32(ux)')
                return m[1]+'\n\tuint32x4_t ux=vreinterpretq_u32_s32(x);'+body+m[3]
            fixed,count=re.subn(pattern,convert,fixed,flags=re.S);assert count==1
    assert fixed!=original
    header.write_text(fixed)
    (out/'hlslpp-arm32.patch').write_text(''.join(difflib.unified_diff(original.splitlines(True),fixed.splitlines(True),fromfile='a/include/hlsl++/platforms/neon.h',tofile='b/include/hlsl++/platforms/neon.h')))
    if mode == 'micro':
        sources = prepare_scene(out)+[BASE/'matrix/scene.c']
        text = (BASE/'matrix/bench.cpp').read_text().replace('"kernels.h"','"kernels.h"\n#include "../hlslpp/kernels.h"')
        # Use the same wire layout: method 1 is explicitly labelled hlsl now.
        text = text.replace('zz_mat_cmsis','zz_mat_hlsl').replace('zz_vec_cmsis','zz_vec_hlsl')
    else:
        sources = prepare_internal(out)+[BASE/'tinygl_bench/scene.c',BASE/'tinygl_bench/profile.c']
        p=out/'tinygl/src/vertex.c';s=p.read_text()
        s='#include "hlsl_kernels.h"\n'+s
        marker='        if(zz_tgl_neon) {'
        assert s.count(marker)==1
        s=s.replace(marker,'''        if(zz_tgl_neon==2) {
            zz_vertex_hlsl((const float *)&zz_columns, &v->coord.X, &v->pc.X);
            if(c->matrix_model_projection_no_w_transform) v->pc.W=c->matrix_model_projection.m[3][3];
        } else if(zz_tgl_neon==1) {''')
        p.write_text(s)
        p=out/'tinygl/src/zmath.c';s=p.read_text()
        s='#include "hlsl_kernels.h"\n'+s
        marker='    if(zz_tgl_neon){'
        assert s.count(marker)==1
        s=s.replace(marker,'    if(zz_tgl_neon==2){zz_mat_hlsl(&a->m[0][0],&b->m[0][0],&c->m[0][0]);return;}\n'+marker)
        p.write_text(s)
        text=(BASE/'tinygl_bench/bench.cpp').read_text()
        assert text.count('zz_tgl_neon=m;')==2
        text=text.replace('zz_tgl_neon=m;', 'zz_tgl_neon=m?2:'+('1' if mode=='neon' else '0')+';')
    (out/'hlsl_kernels.h').write_bytes((BASE/'hlslpp/kernels.h').read_bytes())
    text=text.replace('#include "../hlslpp/kernels.h"','#include "hlsl_kernels.h"')
    (out/'hlsl-bench.cpp').write_text(text)
    sources += [out/'hlsl-bench.cpp', BASE/'hlslpp/kernels.cpp', BASE/'matrix/neon.c',BASE/'matrix/scalar.c',BASE/'opengl/messages.c']
    (out/'hlslpp-provenance.json').write_text(json.dumps(dict(url=URL,revision=REV,license='MIT',mode=mode,patches=['hlslpp-arm32.patch: explicit intrinsic bit reinterpretation for GCC ARM32'],prepare_script_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),archive_sha256=hashlib.sha256(archive).hexdigest(),source_hashes={str(p.relative_to(out)):hashlib.sha256(p.read_bytes()).hexdigest() for p in sources if p.is_relative_to(out)},header_hashes={str(p.relative_to(stage)):hashlib.sha256(p.read_bytes()).hexdigest() for p in sorted(stage.rglob('*.h'))}),indent=2)+'\n')
    return sources
