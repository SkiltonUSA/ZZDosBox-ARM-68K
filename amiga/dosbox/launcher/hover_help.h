/* GPL-3.0-or-later. Event-driven footer help for MUI 3.8 buttons. */
static struct MUI_CustomClass *hover_help_class;
struct hover_help_data {struct MUI_EventHandlerNode events;int shown,hover;};
static void hover_help_set(struct hover_help_data *d,int hover){
 if(d->hover==hover)return;d->hover=hover;
 set(status,MUIA_Text_Contents,(ULONG)(hover?
  "Delete removes game/app files and saves. OS and shared installations are protected.":message));
}
static ULONG hover_help_dispatch(struct IClass *cl __asm("a0"),
 Object *obj __asm("a2"),Msg msg __asm("a1")){
 struct hover_help_data *d=INST_DATA(cl,obj);
 switch(msg->MethodID){
 case MUIM_Setup:
  if(!DoSuperMethodA(cl,obj,msg))return FALSE;
  memset(&d->events,0,sizeof(d->events));
  d->events.ehn_Object=obj;d->events.ehn_Class=cl;
  d->events.ehn_Events=IDCMP_MOUSEMOVE|IDCMP_INACTIVEWINDOW;
  d->events.ehn_Priority=10;d->events.ehn_Flags=0;
  DoMethod(_win(obj),MUIM_Window_AddEventHandler,(ULONG)&d->events);return TRUE;
 case MUIM_Show:
  if(!DoSuperMethodA(cl,obj,msg))return FALSE;d->shown=1;return TRUE;
 case MUIM_Hide:d->shown=0;hover_help_set(d,0);break;
 case MUIM_Cleanup:
  hover_help_set(d,0);DoMethod(_win(obj),MUIM_Window_RemEventHandler,(ULONG)&d->events);break;
 case MUIM_HandleEvent:{
  struct IntuiMessage *im=((struct MUIP_HandleEvent*)msg)->imsg;
  if(!im||(im->Class!=IDCMP_MOUSEMOVE&&im->Class!=IDCMP_INACTIVEWINDOW))break;
  if(d->shown){
   if(im->Class==IDCMP_INACTIVEWINDOW)hover_help_set(d,0);
   else if(im->Class==IDCMP_MOUSEMOVE)hover_help_set(d,
    im->MouseX>=_left(obj)&&im->MouseX<=_right(obj)&&
    im->MouseY>=_top(obj)&&im->MouseY<=_bottom(obj));
  }return 0;
 }
 }
 return DoSuperMethodA(cl,obj,msg);
}
static Object *delete_button(void){return NewObject(hover_help_class->mcc_Class,NULL,
 ButtonFrame,MUIA_Font,ui_font(),MUIA_InputMode,MUIV_InputMode_RelVerify,
 MUIA_Background,MUII_ButtonBack,MUIA_CycleChain,1,MUIA_InnerTop,5,MUIA_InnerBottom,5,
 MUIA_Text_PreParse,(ULONG)"\33c",MUIA_Text_Contents,(ULONG)"Delete",TAG_DONE);}
