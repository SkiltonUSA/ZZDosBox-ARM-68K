/* RasterIX S.5 edge setup derived from Xanxi's librixgpu rixdesc.c (0BSD).
 * https://github.com/Xanxi-Amiga/XACP-ZZ9000/tree/main/applications/games/ZZQuakeGL/source/ZZQuakeGL/librixgpu
 * Adaptation: explicit geometry, no global state, Glide preweighted ST input,
 * no depth test. Retains the reference's unbiased edge rule (see README).
 * SPDX-License-Identifier: 0BSD
 */
#include "descriptor.h"

static int32_t edge(const int32_t *a,const int32_t *b,const int32_t *p)
{
    return (int32_t)((int64_t)(p[0]-a[0])*(b[1]-a[1])-
                     (int64_t)(p[1]-a[1])*(b[0]-a[0]));
}
static uint32_t bits(float f) {uint32_t u;__builtin_memcpy(&u,&f,4);return u;}

int grx_descriptor(const grx_config *c,const grx_vertex *v,uint32_t out[39])
{
    int32_t p[3][2],lo[2],hi[2],start[2],weights[3],dx[3],dy[3];
    float attributes[3][9];
    unsigned i,j,k;
    for(i=0;i<3;i++) {
        float y=c->origin==GRX_UPPER_LEFT?(float)(c->height-1)-v[i].y:v[i].y;
        p[i][0]=(int32_t)(v[i].x*32.0f+0.5f);
        p[i][1]=(int32_t)(y*32.0f+0.5f);
        attributes[i][0]=v[i].r/255.0f;attributes[i][1]=v[i].g/255.0f;
        attributes[i][2]=v[i].b/255.0f;attributes[i][3]=v[i].a/255.0f;
        attributes[i][4]=0;attributes[i][5]=v[i].oow;
        /* Glide's longer texture axis is 256, regardless of mip dimensions. */
        unsigned longest=c->texture_width>c->texture_height?c->texture_width:c->texture_height;
        attributes[i][6]=longest?v[i].sow*((float)longest/(256.0f*c->texture_width)):0;
        attributes[i][7]=longest?v[i].tow*((float)longest/(256.0f*c->texture_height)):0;
        attributes[i][8]=v[i].oow;
    }
    int32_t area=edge(p[0],p[1],p[2]);
    if(!area)return 0;
    int sign=area<0?-1:1;area*=sign;
    for(j=0;j<2;j++) {
        lo[j]=hi[j]=p[0][j];
        for(i=1;i<3;i++) {
            if(p[i][j]<lo[j])lo[j]=p[i][j];
            if(p[i][j]>hi[j])hi[j]=p[i][j];
        }
        lo[j]=(lo[j]+16)>>5;hi[j]=(hi[j]+48)>>5;start[j]=lo[j]<<5;
    }
    out[0]=0;out[1]=(uint32_t)lo[0]|((uint32_t)lo[1]<<16);
    out[2]=(uint32_t)hi[0]|((uint32_t)hi[1]<<16);
    for(i=0;i<3;i++) {
        unsigned a=(i+1)%3,b=(i+2)%3;
        weights[i]=edge(p[a],p[b],start)*sign;
        dx[i]=(p[b][1]-p[a][1])*32*sign;
        dy[i]=(p[a][0]-p[b][0])*32*sign;
        out[3+i]=(uint32_t)weights[i];out[6+i]=(uint32_t)dx[i];out[9+i]=(uint32_t)dy[i];
    }
    /* Descriptor groups: RGBA, Z/W, S/W T/W 1/W; each origin, dx, dy. */
    const unsigned sizes[3]={4,2,3};unsigned field=0,pos=12;
    float invarea=1.0f/(float)area;
    for(i=0;i<3;i++) {
        for(j=0;j<3;j++) {
            const int32_t *w=j==0?weights:j==1?dx:dy;
            for(k=0;k<sizes[i];k++) {
                float f=0;
                for(unsigned n=0;n<3;n++)f+=attributes[n][field+k]*((float)w[n]*invarea);
                out[pos++]=bits(f);
            }
        }
        field+=sizes[i];
    }
    return 1;
}
