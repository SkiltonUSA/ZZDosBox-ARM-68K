/* GPL-3.0-or-later. Offscreen TinyGL scene, NOT a DOS game or scanout FPS test. */
#include "kernels.h"
#include <TGL/gl.h>
#include <zbuffer.h>
static ZBuffer *buffer;
/* Required by the existing draw-pixels extension; unused by this scene. */
unsigned zz_gl_format;
const volatile unsigned char *zz_gl_stop;
int zz_gl_cancelled;
int zz_matrix_scene_open(void) {
    buffer=ZB_open(640,480,ZB_MODE_RGBA,0);
    if(!buffer) return 0;
    glInit(buffer);
    glViewport(0,0,640,480);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glMatrixMode(GL_MODELVIEW); glLoadIdentity();
    glDisable(GL_CULL_FACE); glDisable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST); glDepthMask(GL_TRUE);
    glShadeModel(GL_SMOOTH);
    return glGetError()==GL_NO_ERROR;
}
void zz_matrix_scene_draw(const float *v,unsigned triangles) {
    glClearColor(0,0,0,1);
    glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    glBegin(GL_TRIANGLES);
    for(unsigned i=0;i<triangles*3;i++) {
        glColor3f((i%3)==0, (i%3)==1, (i%3)==2);
        glVertex4f(v[i*4],v[i*4+1],v[i*4+2],v[i*4+3]);
    }
    glEnd();
}
unsigned zz_matrix_scene_hash(void) {
    unsigned h=2166136261u;
    const unsigned char *p=(const unsigned char*)buffer->pbuf;
    for(unsigned y=0;y<480;y++) for(unsigned x=0;x<640*4;x++) h=(h^p[y*buffer->linesize+x])*16777619u;
    return h;
}
void zz_matrix_scene_close(void) {
    if(buffer) {glClose(); ZB_close(buffer); buffer=0;}
}
