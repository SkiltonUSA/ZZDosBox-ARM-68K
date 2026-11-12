/* Included only by the diagnostic build. Never changes a screen mode or saves. */
#ifndef ZZ_UI_CAPTURE_TICKS
#define ZZ_UI_CAPTURE_TICKS 3000
#endif
static void layout_pump(void){
 ULONG signals;unsigned i;
 for(i=0;i<12;i++){DoMethod(app,MUIM_Application_NewInput,(ULONG)&signals);ui_scroll_update();Delay(1);}
}
#ifdef ZZ_UI_GALLERY
#include "launcher_gallery.h"
#else
static int layout_probe(void){
 FILE *report=freopen("PROGDIR:layout-report.txt","w",stdout);
 Object *virt;unsigned page=0;int errors=0;
 struct Window *native=(struct Window*)val(win,MUIA_Window_Window);
 if(!report)return 20;
 printf("LIMITS %d %d / %d %d\n",native->MinWidth,native->MinHeight,native->MaxWidth,native->MaxHeight);
 printf("LAYOUT screen budget %u x %u compact %d window %u x %u\n",ZZ_UI_TEST_WIDTH,ZZ_UI_TEST_HEIGHT,ui_compact,native->Width,native->Height);
 if(native->Width>ZZ_UI_TEST_WIDTH||native->Height>ZZ_UI_TEST_HEIGHT-20)errors++;
 for(page=0;page<PAGE_COUNT;page++){
  navigation_show(page);layout_pump();virt=ui_scroll_objects[page].view;
  if(val(main_pages,MUIA_Group_ActivePage)!=page)errors++;
  if(page==PAGE_CPU&&_mright(cpu_model)<_mright(virt)-24)errors++;
  printf("PAGE %u %s viewport %ld x %ld content %lu x %lu\n",page,sections[page],(long)_mwidth(virt),(long)_mheight(virt),(unsigned long)val(virt,MUIA_Virtgroup_Width),(unsigned long)val(virt,MUIA_Virtgroup_Height));
  if(_mwidth(virt)<180||_mheight(virt)<100)errors++;
  SetAttrs(virt,MUIA_Virtgroup_Left,10000,MUIA_Virtgroup_Top,10000,TAG_DONE);layout_pump();
  printf("SCROLL %lu %lu\n",(unsigned long)val(virt,MUIA_Virtgroup_Left),(unsigned long)val(virt,MUIA_Virtgroup_Top));
  SetAttrs(virt,MUIA_Virtgroup_Left,0,MUIA_Virtgroup_Top,0,TAG_DONE);
  fflush(stdout);
 }
 if(page!=PAGE_COUNT)errors++;
 for(unsigned i=0;i<220;i++)note("Status history capacity check");
 if(loglines!=200||val(loglist,MUIA_List_Entries)!=200)errors++;
 DoMethod(loglist,MUIM_List_Clear);loglines=0;note("All settings pages checked. Ready.");
 printf("STATUS bounded history=%lu\n",(unsigned long)val(loglist,MUIA_List_Entries));
 for(unsigned i=0;i<4;i++){set(program_tabs,MUIA_Group_ActivePage,i);navigation_show(PAGE_LOAD);layout_pump();}
 set(program_tabs,MUIA_Group_ActivePage,0);
 /* Exercise real MUI resizing, without injecting a held mouse gesture. */
 {const unsigned widths[]={480,608,768};unsigned i;
  for(i=0;i<3;i++){
   ChangeWindowBox(native,native->LeftEdge,native->TopEdge,widths[i],360);layout_pump();
   printf("RESIZE requested %u x 360 actual %u x %u\n",widths[i],native->Width,native->Height);
   /* Intuition clamps to the font-dependent minimum. The full-size
    * font need not shrink to 480px; the supported 640px screen must fit. */
   unsigned expected=widths[i]<(unsigned)native->MinWidth?(unsigned)native->MinWidth:widths[i];
   if((unsigned)native->Width!=expected||native->Height!=360||native->MinWidth>640)errors++;
  }
  ChangeWindowBox(native,native->LeftEdge,native->TopEdge,ui_layout.width,ui_layout.height);layout_pump();
 }
 /* Keep a real small window available for a screenshot, without mouse drags. */
 navigation_show(PAGE_LOAD);layout_pump();Delay(ZZ_UI_CAPTURE_TICKS);
 navigation_show(PAGE_DISPLAY);layout_pump();Delay(ZZ_UI_CAPTURE_TICKS);
 {Object *splash=splash_create(1);struct Window *w=splash?(struct Window*)val(splash,MUIA_Window_Window):NULL;
  if(!w||w->Width>ZZ_UI_TEST_WIDTH||w->Height>ZZ_UI_TEST_HEIGHT-20)errors++;
  if(w)printf("LOADING %u x %u\n",w->Width,w->Height);
  layout_pump();splash_close(splash);
 }
 printf("LAYOUT %s errors=%d pages=%u\n",errors?"FAIL":"PASS",errors,page);fflush(stdout);
 return errors?20:0;
}

#endif
