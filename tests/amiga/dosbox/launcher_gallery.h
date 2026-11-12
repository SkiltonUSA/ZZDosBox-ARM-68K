/* Read-only screenshot driver: scratch profiles only, no saves or ARM launches.
 * Gallery.page selects a page; Gallery.ready acknowledges completed layout.
 * 12/13 show the 60000/100000 CPU choices. 99 closes the gallery. */
static int layout_probe(void){
 int last=-1;unsigned idle=0;ULONG signals;
 struct Window *native=(struct Window*)val(win,MUIA_Window_Window);
 SetWindowTitles(native,(STRPTR)"ZZDosBox",(STRPTR)-1);
 ChangeWindowBox(native,native->LeftEdge,native->TopEdge,940,580);layout_pump();
 while(idle++<6000){
  int page=-1;FILE*f=fopen("PROGDIR:Gallery.page","r");
  if(f){if(fscanf(f,"%d",&page)!=1)page=-1;fclose(f);}
  if(page==99)return 0;
  if(page>=0&&page<14&&page!=last){
   set(speed,MUIA_Cycle_Active,library.p[selected].speed);
   if(page>=12)set(speed,MUIA_Cycle_Active,page==12?4:5);
   navigation_show(page>=12?PAGE_CPU:(unsigned)page);
   DoMethod(loglist,MUIM_List_Clear);loglines=0;note("ZZDosBox " ZZ_LAUNCHER_VERSION);note("Ready.");
   layout_pump();
   f=fopen("PROGDIR:Gallery.ready","w");if(!f)return 20;
   fprintf(f,"%d\n",page);
   fprintf(f,"contents %ldx%ld\n",(long)_mwidth(ui_scroll_objects[active_page].contents),(long)_mheight(ui_scroll_objects[active_page].contents));
   fprintf(f,"cpu cycle %ld %ld %ldx%ld\n",(long)_mleft(cpu_model),(long)_mtop(cpu_model),(long)_mwidth(cpu_model),(long)_mheight(cpu_model));
   fprintf(f,"panel %ldx%ld view %ldx%ld virtual %lux%lu\n",(long)_mwidth(main_pages),(long)_mheight(main_pages),(long)_mwidth(ui_scroll_objects[active_page].view),(long)_mheight(ui_scroll_objects[active_page].view),(unsigned long)val(ui_scroll_objects[active_page].view,MUIA_Virtgroup_Width),(unsigned long)val(ui_scroll_objects[active_page].view,MUIA_Virtgroup_Height));fclose(f);last=page;idle=0;
  }
  ULONG id=DoMethod(app,MUIM_Application_NewInput,(ULONG)&signals);
  if(id==(ULONG)MUIV_Application_ReturnID_Quit)return 0;
  ui_scroll_update();Delay(5);
 }
 return 20;
}
