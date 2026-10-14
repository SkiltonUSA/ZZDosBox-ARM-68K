/* GPL-3.0-or-later. Private 640x480 software renderer; no display/guest offload. */
#include "experiment.h"
#include <TGL/gl.h>
#include <zbuffer.h>
static ZBuffer *buffer;
static unsigned texture;
static unsigned char texels[256*256*3];
unsigned zz_gl_format;
const volatile unsigned char *zz_gl_stop;
int zz_gl_cancelled;
/* Column-major GL API matrices; nonidentity model and perspective W. */
static const float model[16]={.875f,-.125f,0,0, .125f,.875f,0,0, 0,0,.5f,0, .03125f,-.03125f,0,1};
static const float projection[16]={1,0,0,0, 0,1,0,0, 0,0,1,.125f, 0,0,0,1};
static float vertices[3072*4];
void zz_tgl_scene_prepare(unsigned kind,unsigned triangles) {
    for(unsigned t=0;t<triangles;t++)for(unsigned k=0;k<3;k++) {
        float *v=vertices+(t*3+k)*4;
        float x=((int)(t%32)-16)/18.0f,y=((int)((t/32)%32)-16)/18.0f;
        if(kind==0){v[0]=x+(k==1?.015625f:0);v[1]=y+(k==2?.015625f:0);}
        else if(kind==1){ /* skinny edge-crossing triangles: clipping, little fill */
            float sign=(t&1)?1.0f:-1.0f;
            v[0]=sign*(k==1?1.75f:.75f);v[1]=((int)(t%64)-32)/40.0f+(k==2?.015625f:0);
        } else {v[0]=k==1?.875f:-.875f;v[1]=k==2?.875f:-.875f;}
        v[2]=(kind==3||kind==5)?(float)(t%32)/64:(kind==7||kind==6)?((t&1)?-.25f:.25f):.5f-(float)(t%32)/64;if(kind==7||kind==6){v[0]+=(float)((int)(t%7)-3)/16;v[1]+=(float)((int)(t%5)-2)/16;}v[3]=1;
    }
}
int zz_matrix_scene_open(void) {
    buffer=ZB_open(640,480,ZB_MODE_RGBA,0);if(!buffer)return 0;
    glInit(buffer);glViewport(0,0,640,480);
    glDisable(GL_CULL_FACE);glDisable(GL_LIGHTING);
    glEnable(GL_DEPTH_TEST);glDepthMask(GL_TRUE);glShadeModel(GL_SMOOTH);
    for(unsigned y=0;y<256;y++)for(unsigned x=0;x<256;x++){
        unsigned i=(y*256+x)*3,check=((x/16)^(y/16))&1;
        texels[i]=check?240:32;texels[i+1]=(unsigned char)x;texels[i+2]=(unsigned char)y;
    }
    glGenTextures(1,&texture);glBindTexture(GL_TEXTURE_2D,texture);
    glTexImage2D(GL_TEXTURE_2D,0,3,256,256,0,GL_RGB,GL_UNSIGNED_BYTE,texels);
    glDisable(GL_TEXTURE_2D);
    return glGetError()==GL_NO_ERROR;
}
void zz_matrix_scene_draw(unsigned kind,unsigned triangles) {
    (void)kind;
    if(zz_tgl_profile)zz_tgl_enter(ZZ_CLEAR);
    glClearColor(0,0,0,1);glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    if(zz_tgl_profile)zz_tgl_leave();
    glMatrixMode(GL_PROJECTION);glLoadMatrixf(projection);
    glMatrixMode(GL_MODELVIEW);glLoadMatrixf(model);
    for(unsigned i=0;i<triangles*3;i++) {
        if(i%3==0){
            if(kind==4||kind==5||(kind==6&&((i/3)&1)))glEnable(GL_TEXTURE_2D);
            else glDisable(GL_TEXTURE_2D);
            glBegin(GL_TRIANGLES);
        }
        glTexCoord2f(i%3==1?1.0f:0.0f,i%3==2?1.0f:0.0f);
        glColor3f((i%3)==0,(i%3)==1,(i%3)==2);
        glVertex4f(vertices[i*4],vertices[i*4+1],vertices[i*4+2],vertices[i*4+3]);
        if(i%3==2)glEnd();
    }
    glDisable(GL_TEXTURE_2D);
}
unsigned zz_matrix_scene_hash(void) {
    unsigned h=2166136261u;const unsigned char *p=(const unsigned char*)buffer->pbuf;
    for(unsigned y=0;y<480;y++)for(unsigned x=0;x<640*4;x++)h=(h^p[y*buffer->linesize+x])*16777619u;
    /* Include depth buffer as well as visible colour. */
    p=(const unsigned char*)buffer->zbuf;
    for(unsigned i=0;i<640*480*sizeof(*buffer->zbuf);i++)h=(h^p[i])*16777619u;
    return h;
}
void zz_matrix_scene_close(void) {if(buffer){glClose();ZB_close(buffer);buffer=0;}}

