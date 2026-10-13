/* GPL-3.0-or-later. Retain both OS allocations until synchronous worker reset.
 * Mode changes occur only after frame unmapping and PAN acknowledgement. */
struct pc_saved_view {struct Window *window;struct BitMap *bitmap;ULONG format,native;};
static struct pc_saved_view saved_window,saved_full;
static char live_title[96];
static ULONG focus_due;static int focus_pending;
/* Intuition screen-depth changes are asynchronous. Activate after it has
 * processed the switch; an immediate activation can be overwritten. */
static void display_focus(void){if(focus_pending&&(LONG)(us()-focus_due)>=0){ActivateWindow(window);focus_pending=0;}}
static void update_live_title(void){if(window){snprintf(live_title,sizeof(live_title),"ZZDosBox - F10 closes - %u cycles/ms",ad_get(shared,PC_CYCLE_REQUEST));SetWindowTitles(window,live_title,(STRPTR)-1);}}
static void save_view(struct pc_saved_view *v){v->window=window;v->bitmap=direct_bitmap;v->format=direct_format;v->native=direct_native_format;}
static void load_view(const struct pc_saved_view *v){window=v->window;direct_bitmap=v->bitmap;direct_format=v->format;direct_native_format=v->native;}
static int display_hotkey(UWORD raw,UWORD qualifiers){
 unsigned key=raw&127;int down=!(raw&128);
#ifdef DB_SCANLINE_TEST
 if(key==0x58){
  if(down&&!(qualifiers&IEQUALIFIER_REPEAT)){
   pc_display_scanlines((scan_mode+1)%3);
   printf("SCANLINE_MODE requested=%u epoch=%u (0=off 1=full 2=skip)\n",scan_mode,scan_epoch);
  }
  return 1;
 }
#endif
 if(key==0x44&&(qualifiers&(IEQUALIFIER_LALT|IEQUALIFIER_RALT))){
  if(down&&!(qualifiers&IEQUALIFIER_REPEAT))switch_requested=!fullscreen;
  return 1;
 }
 if((key==0x0b||key==0x0c)&&(qualifiers&IEQUALIFIER_CONTROL)){
  if(down){unsigned speed=ad_get(shared,PC_CYCLE_REQUEST);if(speed<500||speed>10000)speed=3000;
   if(key==0x0c){if(speed<10000)speed+=500;}else if(speed>500)speed-=500;
   publish(PC_CYCLE_REQUEST,speed);update_live_title();printf("CPU_CYCLES requested=%u\n",speed);
  }return 1;
 }
 return 0;
}
static int display_switch(void){
 unsigned i;if(switch_requested<0||switch_requested==fullscreen){switch_requested=-1;return 1;}
 if(active||direct_locked||(fullscreen&&!pc_flow_idle(&flow,&full_slots)))return 1;
 if(!mouse_room())return 1;
 if(!mouse_release())return 0;
 /* Release guest modifiers before changing the input window. */
 for(i=0;i<128;i++)if(window_keys[i]&&!window_key(i|128))return 0;
 if(fullscreen){
  struct RenderInfo ri;ULONG address;
  memset(&ri,0,sizeof(ri));direct_lock=p96LockBitMap(direct_bitmap,(UBYTE*)&ri,sizeof(ri));direct_locked=1;
  address=(ULONG)ri.Memory;
  if(!address||ri.BytesPerRow<2560||ri.BytesPerRow>4096||address<(ULONG)full_board+0x10000u||address>(ULONG)full_board+0x4000000u-1440*(ULONG)ri.BytesPerRow)return 0;
  full_origin=address-(ULONG)full_board-0x10000u;full_pitch=ri.BytesPerRow;full_origin_known=1;
  full_write(0x44,2);if(full_front())full_pan(full_origin,0);full_unlock();
  save_view(&saved_full);ScreenToBack(full_screen);fullscreen=0;load_view(&saved_window);
  if(!window&&!window_open())return 0;ScreenToFront(window->WScreen);window_paint();
 }else{
  save_view(&saved_window);fullscreen=1;load_view(&saved_full);
  if(!window&&!full_open())return 0;
  nf_init(&full_slots);pc_flow_init(&flow,pipeline?2:1);ScreenToFront(full_screen);
 }
 focus_due=us()+100000u;focus_pending=1;
#ifdef DB_SCANLINE_TEST
 scan_epoch++;
#endif
 update_live_title();console_shown=0;switch_requested=-1;printf("DISPLAY_SWITCH mode=%s completed_frame=%u\n",fullscreen?"fullscreen":"window",complete);return 1;
}
