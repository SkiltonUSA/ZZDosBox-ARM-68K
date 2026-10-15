/* GPL-3.0-or-later. On-demand readback of an owned, completed display frame. */
static char screenshot_path[256];
static int screenshot_pending;
void pc_display_screenshot_path(const char *path){
 snprintf(screenshot_path,sizeof(screenshot_path),"%s",path);
}
static void screenshot_save(void){
 struct RenderInfo ri;UBYTE *rgb,*src;unsigned w,h,y,x,top;LONG lock;
 char temp[272],backup[272];FILE *f;int ok,had_old;
 if(!screenshot_pending||active||direct_locked||!direct_bitmap||
    (fullscreen&&!pc_flow_idle(&flow,&full_slots)))return;
 screenshot_pending=0;
 w=fullscreen?640:paint_width;h=fullscreen?480:paint_height;
 if(!w||!h||w>960||h>720||!*screenshot_path)return;
 rgb=AllocVec(w*h*3,MEMF_PUBLIC);if(!rgb){puts("SCREENSHOT out of memory");return;}
 memset(&ri,0,sizeof(ri));lock=p96LockBitMap(direct_bitmap,(UBYTE*)&ri,sizeof(ri));
 top=fullscreen?full_slots.front*480:0;
 ok=ri.Memory&&ri.BytesPerRow>0&&(unsigned)ri.BytesPerRow>=w*4&&(ULONG)ri.RGBFormat==direct_native_format;
 if(ok){
  src=(UBYTE*)ri.Memory+top*ri.BytesPerRow;
  syncio->pull(src,(h-1)*ri.BytesPerRow+w*4,NULL);
  for(y=0;y<h;y++)for(x=0;x<w;x++){
   const UBYTE *p=src+y*ri.BytesPerRow+x*4;UBYTE *q=rgb+(y*w+x)*3;
   q[0]=p[direct_format==PC_BGRA32?2:1];q[1]=p[direct_format==PC_BGRA32?1:2];q[2]=p[direct_format==PC_BGRA32?0:3];
  }
 }
 p96UnlockBitMap(direct_bitmap,lock);
 /* Release the display lock before any filesystem operations. */
 snprintf(temp,sizeof(temp),"%s.tmp",screenshot_path);snprintf(backup,sizeof(backup),"%s.bak",screenshot_path);
 f=ok?fopen(temp,"wb"):NULL;ok=f!=NULL;
 if(f){ok=fprintf(f,"P6\n%u %u\n255\n",w,h)>0&&fwrite(rgb,3,w*h,f)==w*h;if(fclose(f))ok=0;}
 FreeVec(rgb);
 if(ok){BPTR old=Lock(screenshot_path,ACCESS_READ);had_old=old!=0;if(old)UnLock(old);
  if(had_old){DeleteFile(backup);ok=Rename(screenshot_path,backup);}
  if(ok&&!Rename(temp,screenshot_path)){ok=0;if(had_old)Rename(backup,screenshot_path);}
  if(ok&&had_old)DeleteFile(backup);
 }
 if(!ok)DeleteFile(temp);
 printf("SCREENSHOT %s %s (%ux%u)\n",ok?"saved":"failed",screenshot_path,w,h);
}
