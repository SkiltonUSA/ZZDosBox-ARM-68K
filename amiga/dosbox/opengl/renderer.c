/* GPL-3.0-or-later. Fixed 640x480 4:3 software OpenGL output on the A9.
 * Keep the native renderer as default. This is not a guest OpenGL driver. */
#include "renderer.h"
#include <TGL/gl.h>
#include <zbuffer.h>
static ZBuffer *buffer;
static int active;
/* Used only by our private glDrawPixels patch while an external buffer is bound. */
unsigned zz_gl_format;
const volatile unsigned char *zz_gl_stop;
int zz_gl_cancelled;
int zz_gl_prepare(void)
{
 if(!buffer){
  buffer=ZB_open(640,480,ZB_MODE_RGBA,0);if(!buffer)return 0;
  glInit(buffer);active=1;
  if(glGetError()!=GL_NO_ERROR){zz_gl_close();return 0;}
  glViewport(0,0,640,480);
  glMatrixMode(GL_PROJECTION);glLoadIdentity();
  glMatrixMode(GL_MODELVIEW);glLoadIdentity();
  glDisable(GL_DEPTH_TEST);glDepthMask(GL_FALSE);
 }
 return 1;
}
int zz_gl_render(const uint32_t *pixels,unsigned width,unsigned height)
{
 if(!pixels||!width||!height||width>1024||height>768)return 0;
 if(!zz_gl_prepare())return 0;
 glRasterPos2f(-1,1);
 glPixelZoom(640.0f/width,-480.0f/height);
 glDrawPixels(width,height,GL_RGB,GL_UNSIGNED_INT,(void*)pixels);
 return glGetError()==GL_NO_ERROR;
}
int zz_gl_render_to(const uint32_t *pixels,unsigned width,unsigned height,
                   volatile uint32_t *dst,unsigned pitch,unsigned format,
                   const volatile unsigned char *stop)
{
 if(!buffer||!active||!dst||((uintptr_t)dst&3)||pitch<640*4||pitch>4096||
    (pitch&3)||(format!=3&&format!=4)||!pixels||!width||!height||width>1024||height>768)return 0;
 PIXEL *saved=buffer->pbuf;int saved_pitch=buffer->linesize;
 buffer->pbuf=(PIXEL*)dst;buffer->linesize=pitch;
 zz_gl_format=format;zz_gl_stop=stop;zz_gl_cancelled=0;
 int result=zz_gl_render(pixels,width,height);
 /* Restore ownership before any caller unmap, error unwind or close. */
 buffer->pbuf=saved;buffer->linesize=saved_pitch;
 zz_gl_format=0;zz_gl_stop=0;
 return zz_gl_cancelled?-1:result;
}
const uint32_t *zz_gl_pixels(void){return buffer?(const uint32_t*)buffer->pbuf:0;}
void zz_gl_close(void){if(active){glClose();active=0;}if(buffer){ZB_close(buffer);buffer=0;}}
