/* GPL-2.0-or-later. Bounded capture of upstream VGA scanlines. */
#include "dosbox.h"
#include "render.h"
#include "pc.h"
#include <string.h>
#include "profile.h"
#include "capture.h"
#include "adapters.h"
unsigned db_width,db_height,db_frames,db_bpp,db_lines;
uint32_t db_pixels[PC_VIDEO_MAX_WIDTH*PC_VIDEO_MAX_HEIGHT];static uint32_t palette[256];static bool updating;
Render_t render;
#ifndef DB_NATIVE
extern void db_video_present();
#endif
static void line(const void *data){
 if(!updating||db_lines>=db_height)E_Exit("Video scanline bound");
 pc_prof_enter(PP_CAPTURE);
 auto p=(const uint8_t*)data;uint32_t *dst=db_pixels+db_lines*db_width;
 if(!pc_capture_row(dst,p,db_width,db_bpp,palette))E_Exit("Capture bpp %u not implemented",db_bpp);
 db_lines++;pc_prof_leave();
}
ScalerLineHandler_t RENDER_DrawLine=line;
void RENDER_SetSize(Bitu w,Bitu h,Bitu bpp,float,double,bool,bool){
 if(!w||!h||w>PC_VIDEO_MAX_WIDTH||h>PC_VIDEO_MAX_HEIGHT||(bpp!=8&&bpp!=15&&bpp!=16&&bpp!=32))E_Exit("Video mode bound %u %u %u",w,h,bpp);
 db_width=w;db_height=h;db_bpp=bpp;updating=false;
}
bool RENDER_StartUpdate(){db_lines=0;updating=true;return true;}
void RENDER_EndUpdate(bool abort){if(updating&&!abort){if(db_lines!=db_height)E_Exit("Incomplete VGA frame");db_frames++;
#ifndef DB_NATIVE
 db_video_present();
#endif
}updating=false;}
/* Hercules palette lookup: no per-pixel tint pass. RGB endpoints are our
 * sRGB approximations, not Staging's OpenGL phosphor/colour-management code. */
void RENDER_SetPal(Bit8u i,Bit8u r,Bit8u g,Bit8u b){
 if(db_machine_option==PC_ADAPTER_HERCULES&&db_monochrome){
  static const unsigned tint[4]={0xffffff,0xffbf00,0x33ff33,0xfff1d6};
  unsigned c=tint[db_monochrome<4?db_monochrome:0],y=r;
  r=(((c>>16)&255)*y+127)/255;g=(((c>>8)&255)*y+127)/255;b=((c&255)*y+127)/255;
 }
 palette[i]=(r<<16)|(g<<8)|b;
}
bool RENDER_GetForceUpdate(){return true;}void RENDER_SetForceUpdate(bool){}
void db_video_reset(){
#ifdef DB_PRESENT_TEST
 extern void db_presentation_reset();db_presentation_reset();
#endif
 db_width=db_height=db_frames=db_bpp=db_lines=0;updating=false;memset(db_pixels,0,sizeof(db_pixels));memset(palette,0,sizeof(palette));}
