#!/usr/bin/env python3
"""Reproducible internal-transform replacement and exclusive stage hooks.
Only applies to the separate diagnostic TinyGL archive, never production GL.
"""
import hashlib,json
from prepare_dosbox_matrix import prepare_scene

def wrap(text,signature,name,stage):
    assert text.count(signature)==1,signature
    start=text.index(signature);brace=text.index('{',start);level=1;end=brace+1
    while level:
        if text[end]=='{':level+=1
        elif text[end]=='}':level-=1
        end+=1
    impl=signature.replace(name,name+'_unprofiled')
    if not impl.startswith('static '):impl='static '+impl
    args={'gl_vertex_transform':'c, v','gl_transform_to_viewport_vertex_c':'c, v','gl_M4_Mul':'c, a, b','gl_draw_triangle':'p0, p1, p2','gl_draw_triangle_fill':'p0, p1, p2'}[name]
    wrapper='\n'+signature+'\n{\n if(zz_tgl_profile)zz_tgl_enter('+stage+');\n '+name+'_unprofiled('+args+');\n if(zz_tgl_profile)zz_tgl_leave();\n}\n'
    return text[:start]+impl+text[start+len(signature):end]+wrapper+text[end:]

def prepare(out):
    sources=prepare_scene(out);root=out/'tinygl/src'
    p=root/'vertex.c';s=p.read_text();s='#include "experiment.h"\n#include <arm_neon.h>\nstatic float32x4x4_t zz_columns;\n'+s
    # Cache columns only at TinyGL's own combined-matrix invalidation point.
    old='            /* test to accelerate computation */'
    assert s.count(old)==1
    s=s.replace(old,'            zz_columns=vld4q_f32(m);\n'+old)
    old='    if (c->matrix_model_projection_updated) {'
    s=s.replace(old,old+'\n        if(zz_tgl_profile)zz_tgl_enter(ZZ_TRANSFORM);',1)
    s=s.replace('        c->matrix_model_projection_updated = 0;','        c->matrix_model_projection_updated = 0;\n        if(zz_tgl_profile)zz_tgl_leave();',1)
    old='        /* no eye coordinates needed, no normal */'
    s=s.replace(old,'''        if(zz_tgl_neon) {
            /* Preserve upstream W=1 and arithmetic order; A9 VMLA is non-fused. */
            float32x4_t r=vmulq_n_f32(zz_columns.val[0],v->coord.X);
            r=vmlaq_n_f32(r,zz_columns.val[1],v->coord.Y);
            r=vmlaq_n_f32(r,zz_columns.val[2],v->coord.Z);
            r=vaddq_f32(r,zz_columns.val[3]);
            vst1q_f32(&v->pc.X,r);
            if(c->matrix_model_projection_no_w_transform) v->pc.W=c->matrix_model_projection.m[3][3];
        } else {
'''+old,1)
    pos=s.index('    v->clip_code =',s.index('static void gl_vertex_transform'))
    s=s[:pos]+'    }\n'+s[pos:]
    for name in ('gl_vertex_transform','gl_transform_to_viewport_vertex_c'):
        s=wrap(s,'static void '+name+'(GLContext *c, GLVertex *v)',name,'ZZ_TRANSFORM')
    p.write_text(s)
    p=root/'zmath.c';s=p.read_text();s='#include "experiment.h"\n'+s
    # Signature formatting is pinned upstream; fail closed on changes.
    import re
    sig=re.search(r'void gl_M4_Mul\(M4 \*c, M4 \*a, M4 \*b\)',s).group(0)
    start=s.index('{',s.index(sig))+1
    s=s[:start]+'\n    if(zz_tgl_neon){zz_mat_neon(&a->m[0][0],&b->m[0][0],&c->m[0][0]);return;}\n'+s[start:]
    s=wrap(s,sig,'gl_M4_Mul','ZZ_TRANSFORM');p.write_text(s)
    p=root/'clip.c';s='#include "experiment.h"\n'+p.read_text()
    for name,stage in [('gl_draw_triangle','ZZ_CLIP'),('gl_draw_triangle_fill','ZZ_RASTER')]:
        s=wrap(s,'void '+name+'(GLVertex *p0, GLVertex *p1, GLVertex *p2)',name,stage)
    p.write_text(s)
    p=out/'tinygl-provenance.json';data=json.loads(p.read_text())
    data['internal_transform_experiment']={'scope':'unlit vertices and gl_M4_Mul; lighting keeps upstream implementation; cached matrix columns; exclusive stage profiling separately enabled','source_hashes':{name:hashlib.sha256((root/name).read_bytes()).hexdigest() for name in ('vertex.c','zmath.c','clip.c')}}
    p.write_text(json.dumps(data,indent=2)+'\n');return sources
