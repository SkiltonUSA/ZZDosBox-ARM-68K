/* GPL-3.0-or-later. ARM renders only into our locked offscreen P96 bitmap.
 * Intuition owns Workbench clipping; no direct writes to its screen bitmap. */
#include "window_geometry.h"
static struct Screen *workbench;
static int window_ready;
static unsigned window_preset=1,window_width,window_height,paint_width,paint_height;
static LONG window_black=-1;
static UBYTE window_keys[128];
#include "mouse_host.h"
static void window_black_rect(LONG x,LONG y,LONG w,LONG h){
 if(w>0&&h>0)RectFill(window->RPort,x,y,x+w-1,y+h-1);
}
static void window_paint(void){
 LONG width,height,w,h,x,y,left,top;
 if(!window||direct_locked)return;
 left=window->BorderLeft;top=window->BorderTop;
 width=window->Width-left-window->BorderRight;height=window->Height-top-window->BorderBottom;
 if(width<=0||height<=0)return;
 SetAPen(window->RPort,window_black);SetDrMd(window->RPort,JAM1);
 if(!window_ready){window_black_rect(left,top,width,height);return;}
 /* A resize can arrive while Core1 finishes the old geometry. Crop that
  * completed frame to the new client; the next grant uses the new size. */
 w=(LONG)paint_width<width?(LONG)paint_width:width;h=(LONG)paint_height<height?(LONG)paint_height:height;
 x=(width-w)/2;y=(height-h)/2;
 window_black_rect(left,top,width,y);window_black_rect(left,top+y+h,width,height-y-h);
 window_black_rect(left,top+y,x,h);window_black_rect(left+x+w,top+y,width-x-w,h);
 BltBitMapRastPort(direct_bitmap,0,0,window->RPort,left+x,top+y,w,h,0xc0);
 WaitBlit();
}
static int window_open(void){
 struct ConfigDev *card=FindConfigDev(NULL,0x6d6e,4);ULONG format;
 unsigned width=320*(window_preset+1),height=240*(window_preset+1);
 if(!card)return 0;
 full_board=card->cd_BoardAddr;
 workbench=LockPubScreen(NULL);if(!workbench)return 0;
 format=p96GetBitMapAttr(workbench->RastPort.BitMap,P96BMA_RGBFORMAT);
 if(format!=RGBFB_B8G8R8A8&&format!=RGBFB_A8R8G8B8){puts("WINDOW requires a 32-bit RTG Workbench");return 0;}
 direct_native_format=format;direct_format=format==RGBFB_B8G8R8A8?PC_BGRA32:PC_ARGB32;
 window_black=ObtainBestPenA(workbench->ViewPort.ColorMap,0,0,0,NULL);if(window_black<0)return 0;
 /* Stable capacity: never reallocate a bitmap in use by Core1. */
 direct_bitmap=p96AllocBitMap(960,720,32,BMF_DISPLAYABLE|BMF_CLEAR,workbench->RastPort.BitMap,format);
 if(!direct_bitmap||!p96GetBitMapAttr(direct_bitmap,P96BMA_ISONBOARD))return 0;
 window=OpenWindowTags(NULL,WA_PubScreen,(ULONG)workbench,WA_Left,60,WA_Top,70,
  WA_InnerWidth,width,WA_InnerHeight,height,WA_Title,(ULONG)"ZZDosBox - F10 closes",
  WA_Flags,WFLG_REPORTMOUSE|WFLG_RMBTRAP|WFLG_SIZEGADGET|WFLG_SIZEBBOTTOM|WFLG_DRAGBAR|WFLG_DEPTHGADGET|WFLG_CLOSEGADGET|WFLG_ACTIVATE|WFLG_SIMPLE_REFRESH,
  WA_IDCMP,IDCMP_MOUSEMOVE|IDCMP_MOUSEBUTTONS|IDCMP_NEWSIZE|IDCMP_RAWKEY|IDCMP_CLOSEWINDOW|IDCMP_REFRESHWINDOW|IDCMP_INACTIVEWINDOW,TAG_DONE);
 if(!window)return 0;
 window_mouse_pointer=AllocMem(12,MEMF_CHIP|MEMF_CLEAR);
 WindowLimits(window,320+window->BorderLeft+window->BorderRight,240+window->BorderTop+window->BorderBottom,960+window->BorderLeft+window->BorderRight,720+window->BorderTop+window->BorderBottom);
 window_paint();
 printf("WINDOW native_format=%u geometry=%ux%u resizable=1 ARM=owned-bitmap present=P96-blit\n",format,width,height);
 return 1;
}
static int window_key(UWORD raw){ULONG w=ad_get(shared,PC_KEY_WRITE),r;
 syncio->pull(shared+PC_KEY_READ,4,NULL);r=ad_get(shared,PC_KEY_READ);
 if(w-r>=PC_KEY_SIZE)return 0;
 shared[PC_KEY_DATA+(w&(PC_KEY_SIZE-1))]=(UBYTE)raw;
 syncio->push(shared+PC_KEY_DATA,PC_KEY_SIZE,NULL);publish(PC_KEY_WRITE,w+1);
 window_keys[raw&127]=!(raw&128);return 1;
}
static int window_events(void){
 struct IntuiMessage *msg;
 if(direct_locked)return 1;
 display_focus();
 while(mouse_room()&&(msg=(struct IntuiMessage*)GetMsg(window->UserPort))){
  ULONG cl=msg->Class;UWORD raw=msg->Code,qual=msg->Qualifier;WORD mx=msg->MouseX,my=msg->MouseY;ReplyMsg((struct Message*)msg);
  if(!mouse_event(cl,raw,mx,my))return 0;
  if(cl==IDCMP_CLOSEWINDOW||(cl==IDCMP_RAWKEY&&raw==0x59))return -1;
  if(cl==IDCMP_REFRESHWINDOW){BeginRefresh(window);window_paint();EndRefresh(window,TRUE);}
  if(cl==IDCMP_NEWSIZE)window_paint();
  if(cl==IDCMP_RAWKEY&&!display_hotkey(raw,qual)&&!window_key(raw))return 0;
  if(cl==IDCMP_INACTIVEWINDOW){unsigned i;if(!mouse_release())return 0;for(i=0;i<128;i++)if(window_keys[i]&&!window_key(i|128))return 0;}

 }
 return 1;
}
static int window_service(void){
 struct RenderInfo ri;struct pc_window_rect rect;ULONG seq,addr,span,nonce;int event;
 if(!window&&!window_open())return 0;
 syncio->pull(shared+PC_VIDEO,44,NULL);seq=ad_get(shared,PC_VIDEO);
 if(active&&ad_get(shared,PC_VIDEO+36)==active){
  ULONG elapsed=us()-copy_start;copy_us+=elapsed;if(elapsed>copy_max)copy_max=elapsed;
  if(ad_get(shared,PC_VIDEO+40))return 0;
  complete=active;active=0;frames++;full_unlock();locked_memory=NULL;releases++;
  window_ready=1;paint_width=window_width;paint_height=window_height;window_paint();
 }
 if((event=window_events())<=0)return event;
 if(switch_requested>=0)return display_switch();
 if(active||seq==complete)return 1;
 if(seq!=complete+1)return 0;
 rect=pc_window_fit(window->Width-window->BorderLeft-window->BorderRight,window->Height-window->BorderTop-window->BorderBottom);
 if(rect.width<320||rect.height<240)return 1;
 window_width=rect.width;window_height=rect.height;
 memset(&ri,0,sizeof(ri));direct_lock=p96LockBitMap(direct_bitmap,(UBYTE*)&ri,sizeof(ri));direct_locked=1;
 full_pitch=ri.BytesPerRow;
 if(!ri.Memory||ri.BytesPerRow<960*4||ri.BytesPerRow>4096||(ri.BytesPerRow&3)||
    (ULONG)ri.RGBFormat!=direct_native_format||!p96GetBitMapAttr(direct_bitmap,P96BMA_ISONBOARD)||
    (ULONG)ri.Memory<(ULONG)full_board+0x10000u||
    (ULONG)ri.Memory>(ULONG)full_board+0x4000000u-720*full_pitch)return 0;
 locked_memory=(UBYTE*)ri.Memory;addr=(ULONG)ri.Memory-(ULONG)full_board+0x1f0000u;
 span=pc_span_size(addr,full_pitch,direct_format,window_width,window_height);if(!span)return 0;
 nonce=ad_get(shared,ZZ_CONTROL)^seq^0x973b102du;
 ad_put(locked_memory,0,nonce);ad_put(locked_memory,span-4,nonce^0x31415926u);
 syncio->push(locked_memory,4,NULL);syncio->push(locked_memory+span-4,4,NULL);
 publish(PC_VIDEO+44,window_width);publish(PC_VIDEO+48,window_height);
 publish(PC_VIDEO+20,addr);publish(PC_VIDEO+24,full_pitch);publish(PC_VIDEO+28,direct_format);publish(PC_VIDEO+32,nonce);
 copy_start=us();publish(PC_VIDEO+16,seq);active=seq;return 1;
}
static void window_cleanup(void){
 /* No blitter/Intuition or allocation teardown until Core1 is synchronously idle. */
 full_unlock();
 if(window){if(window_mouse_hidden)ClearPointer(window);CloseWindow(window);window=NULL;}
 window_mouse_hidden=0;
 if(window_mouse_pointer){FreeMem(window_mouse_pointer,12);window_mouse_pointer=NULL;}
 if(direct_bitmap){p96FreeBitMap(direct_bitmap);direct_bitmap=NULL;}
 if(workbench&&window_black>=0){ReleasePen(workbench->ViewPort.ColorMap,window_black);window_black=-1;}
 if(workbench){UnlockPubScreen(NULL,workbench);workbench=NULL;}

}
