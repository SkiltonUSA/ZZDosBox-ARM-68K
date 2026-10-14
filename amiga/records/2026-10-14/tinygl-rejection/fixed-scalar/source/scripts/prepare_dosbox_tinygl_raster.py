#!/usr/bin/env python3
"""Patch only the separate diagnostic: four smooth untextured span variants."""
import hashlib,json
from prepare_dosbox_tinygl_bench import prepare as prepare_internal
from prepare_dosbox_matrix import ROOT

def prepare(out,fixed_mode="comparison"):
    assert fixed_mode in ("comparison","scalar","guarded","hybrid")
    sources=prepare_internal(out);root=out/'tinygl/src'
    p=root/'ztriangle.c';s='#include "span.h"\n'+p.read_text()
    for dt in range(2):
        for dw in range(2):
            name=f'ZB_fillTriangleSmoothNOBLEND_DT{dt}_DW{dw}'
            begin=s.index('void '+name+'(');end=s.index('#include "ztriangle.h"',begin)
            s=s[:end]+f'#define ZZ_SPAN_DT {dt}\n#define ZZ_SPAN_DW {dw}\n'+s[end:]
    if fixed_mode!="comparison":
        # Only this translation unit sees a constant selector. Read declarations
        # first; the include guard prevents replacing the extern declaration.
        mode={"scalar":0,"guarded":2,"hybrid":3}[fixed_mode]
        s='#include "raster.h"\n#define zz_tgl_raster '+str(mode)+'u\n'+s
    p.write_text(s)
    p=root/'ztriangle.h' ;s=p.read_text();marker='                while (n >= 3) {';assert s.count(marker)==1
    patch='''#ifdef ZZ_SPAN_DT
#if TGL_HAS(POLYGON_STIPPLE)
                int zz_allowed=!zbdostipple;
#else
                int zz_allowed=1;
#endif
                /* Setup overhead is avoided for short spans. Full groups only. */
                if(zz_tgl_raster && zz_allowed && n>=31) {
                    while(((uintptr_t)pp&15u) && n>=0){
                        PUT_PIXEL(0);pp++;pz++;n--;
                    }
                    unsigned done=zz_span4((uint32_t*)pp,pz,(unsigned)n+1,z,or1,og1,ob1,
                        dzdx,drdx,dgdx,dbdx,ZZ_SPAN_DT,ZZ_SPAN_DW);
                    z+=(uint32_t)dzdx*done;
                    or1=(int)((uint32_t)or1+(uint32_t)drdx*done);
                    og1=(int)((uint32_t)og1+(uint32_t)dgdx*done);
                    ob1=(int)((uint32_t)ob1+(uint32_t)dbdx*done);
                    pp+=done;pz+=done;n-=(int)done;
                }
#endif
'''
    s=s.replace(marker,patch+marker);s+='\n#undef ZZ_SPAN_DT\n#undef ZZ_SPAN_DW\n';p.write_text(s)
    p=root/'zbuffer.c';s='#include "raster.h"\n'+p.read_text();start=s.index('{',s.index('void ZB_clear('))+1
    s=s[:start]+'\n    if(zz_tgl_clear && zz_clear_buffers(zb,clear_z,z,clear_color,r,g,b))return;\n'+s[start:];p.write_text(s)
    p=out/'raster-scene.c';p.write_text((ROOT/'amiga/dosbox/tinygl_bench/scene.c').read_text().replace('v[2]=.5f-(float)(t%32)/64;', 'v[2]=kind==3?(float)(t%32)/64:kind==7?((t&1)?-.25f:.25f):.5f-(float)(t%32)/64;if(kind==7){v[0]+=(float)((int)(t%7)-3)/16;v[1]+=(float)((int)(t%5)-2)/16;}')+'''
void zz_tgl_clear_benchmark(unsigned kind) {
    if(kind==4)ZB_clear(buffer,1,0,1,0,0,0);
    else if(kind==5)ZB_clear(buffer,1,0x5678,1,0x120000,0x340000,0x560000);
    else ZB_clear(buffer,0,0,1,0xab0000,0xcd0000,0xef0000);
}
''')
    data=json.loads((out/'tinygl-provenance.json').read_text())
    data['raster_experiment']={'scope':'32-bit smooth untextured no-blend spans, all four depth variants; scalar tails/short spans/stipple fallback; separate row-aware NEON clears','minimum_span':32,'revision':3,'zero_clear_only':True,'span_modes':['scalar','revision2-neon','guarded-neon','scalar-prefix-neon'],'mixed_scene':7,'fixed_mode':fixed_mode,'patched_source_sha256':{n:hashlib.sha256((root/n).read_bytes()).hexdigest() for n in ('ztriangle.c','ztriangle.h','zbuffer.c')}}
    (out/'tinygl-provenance.json').write_text(json.dumps(data,indent=2)+'\n');return sources+[p]
