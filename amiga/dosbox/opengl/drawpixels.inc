/* ZZDosBox patch: destination-driven nearest sampling, including source row/
 * column zero, minification and negative zoom. Original forward loops skipped
 * row/column zero and could not draw negative zoom. Blending is disabled in
 * this private build. Input uses TinyGL's packed 0x00RRGGBB GL_RGB convention. */
void glopDrawPixels(GLParam *p)
{
    extern unsigned zz_gl_format;
    extern const volatile unsigned char *zz_gl_stop;
    extern int zz_gl_cancelled;
    GLContext *c = gl_get_context();
    ZBuffer *zb = c->zb;
    const int sw=p[1].i, sh=p[2].i;
    const PIXEL *src=p[3].p;
    const float zx=c->pzoomx, zy=c->pzoomy;
    const float rx=c->rasterpos.v[0], ry=c->rasterpos.v[1];
    if(!c->rasterposvalid || !src || sw<=0 || sh<=0 || zx==0 || zy==0)
        return;
    /* Our private output is bounded to 640 pixels. Precompute horizontal
     * samples once, rather than repeating float division 480 times per row. */
    GLint xmap[640];
    if(zb->xsize>640)return;
    for(int x=0;x<zb->xsize;x++){
        float fx=(((float)x+0.5f)-rx)/zx;
        xmap[x]=(fx<0 || fx>=sw)?-1:(GLint)fx;
    }
    for(int y=0;y<zb->ysize;y++) {
        if(zz_gl_stop && (zz_gl_stop[0]|zz_gl_stop[1]|zz_gl_stop[2]|zz_gl_stop[3])){
            zz_gl_cancelled=1;return;
        }
        float fy=(ry-((float)y+0.5f))/zy;
        if(fy<0 || fy>=sh)continue;
        int sy=(int)fy;
        volatile PIXEL *row=(volatile PIXEL*)((unsigned char*)zb->pbuf+y*zb->linesize);
        for(int x=0;x<zb->xsize;x++) {
            int sx=xmap[x];if(sx<0)continue;
            int pos=y*zb->xsize+x;
            if(!zb->depth_test || c->rasterpos_zz>=zb->zbuf[pos]) {
                PIXEL pixel=src[sy*sw+sx];
                row[x]=zz_gl_format==4?__builtin_bswap32(pixel):pixel;
                if(zb->depth_write)zb->zbuf[pos]=c->rasterpos_zz;
            }
        }
    }
}
