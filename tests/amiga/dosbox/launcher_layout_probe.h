/* Included only by the diagnostic build. Never changes a screen mode or saves. */
#ifndef ZZ_UI_CAPTURE_TICKS
#define ZZ_UI_CAPTURE_TICKS 3000
#endif
static void layout_pump(void){
 ULONG signals;unsigned i;
 for(i=0;i<12;i++){DoMethod(app,MUIM_Application_NewInput,(ULONG)&signals);Delay(1);}
}
static int layout_probe(void){
 FILE *report=freopen("PROGDIR:layout-report.txt","w",stdout);
 struct List *children;APTR state;Object *scroll,*virt;unsigned page=0;int errors=0;
 struct Window *native=(struct Window*)val(win,MUIA_Window_Window);
 if(!report)return 20;
 printf("LIMITS %d %d / %d %d\n",native->MinWidth,native->MinHeight,native->MaxWidth,native->MaxHeight);
 printf("LAYOUT screen budget %u x %u compact %d window %u x %u\n",ZZ_UI_TEST_WIDTH,ZZ_UI_TEST_HEIGHT,ui_compact,native->Width,native->Height);
 if(native->Width>ZZ_UI_TEST_WIDTH||native->Height>ZZ_UI_TEST_HEIGHT-20)errors++;
 get(main_pages,MUIA_Group_ChildList,&children);state=children->lh_Head;
 while((scroll=NextObject(&state))){
  navigation_show(page);layout_pump();virt=(Object*)val(scroll,MUIA_Scrollgroup_Contents);
  printf("PAGE %u %s viewport %ld x %ld content %lu x %lu\n",page,sections[page],(long)_mwidth(virt),(long)_mheight(virt),(unsigned long)val(virt,MUIA_Virtgroup_Width),(unsigned long)val(virt,MUIA_Virtgroup_Height));
  if(_mwidth(virt)<180||_mheight(virt)<100)errors++;
  SetAttrs(virt,MUIA_Virtgroup_Left,10000,MUIA_Virtgroup_Top,10000,TAG_DONE);layout_pump();
  printf("SCROLL %lu %lu\n",(unsigned long)val(virt,MUIA_Virtgroup_Left),(unsigned long)val(virt,MUIA_Virtgroup_Top));
  SetAttrs(virt,MUIA_Virtgroup_Left,0,MUIA_Virtgroup_Top,0,TAG_DONE);
  fflush(stdout);page++;
 }
 if(page!=PAGE_COUNT)errors++;
 /* Exercise real MUI resizing, without injecting a held mouse gesture. */
 {const unsigned widths[]={480,608,768};unsigned i;
  for(i=0;i<3;i++){
   ChangeWindowBox(native,native->LeftEdge,native->TopEdge,widths[i],360);layout_pump();
   printf("RESIZE requested %u x 360 actual %u x %u\n",widths[i],native->Width,native->Height);
   if((unsigned)native->Width!=widths[i]||native->Height!=360)errors++;
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
