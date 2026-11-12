/* GPL-3.0-or-later. ZZ8-Bit-style virtual pages for MUI 3.8.
 * Show scrollbars only when the active page needs them. */
static struct MUI_CustomClass *viewport_class;
static ULONG viewport_dispatch(struct IClass *cl __asm("a0"),Object *obj __asm("a2"),Msg msg __asm("a1")){
 ULONG result=DoSuperMethodA(cl,obj,msg);
 if(msg->MethodID==MUIM_AskMinMax){
  struct MUI_MinMax *m=((struct MUIP_AskMinMax*)msg)->MinMaxInfo;
  m->MinWidth=m->MinHeight=0;m->MaxWidth=m->MaxHeight=MUI_MAXMAX;
 }
 return result;
}
struct ui_scroll {Object *group,*view,*bar[2];};
static struct ui_scroll ui_scroll_objects[PAGE_COUNT];
static unsigned ui_scroll_count;
static void ui_scroll_update(void){
 unsigned page=val(main_pages,MUIA_Group_ActivePage);if(page>=ui_scroll_count)return;
 struct ui_scroll*s=&ui_scroll_objects[page];
 unsigned visible[2]={_mwidth(s->view),_mheight(s->view)};
 unsigned total[2]={val(s->view,MUIA_Virtgroup_Width),val(s->view,MUIA_Virtgroup_Height)};
 unsigned attrs[2]={MUIA_Virtgroup_Left,MUIA_Virtgroup_Top};
 if(!visible[0]||!visible[1])return;
 for(unsigned j=0;j<2;j++){
  int show=total[j]>visible[j];
  if((int)val(s->bar[j],MUIA_ShowMe)!=show){DoMethod(s->group,MUIM_Group_InitChange);set(s->bar[j],MUIA_ShowMe,show);DoMethod(s->group,MUIM_Group_ExitChange);}
  unsigned first=val(s->view,attrs[j]),last=total[j]>visible[j]?total[j]-visible[j]:0;
  if(first>last){first=last;set(s->view,attrs[j],first);}
  SetAttrs(s->bar[j],MUIA_NoNotify,TRUE,MUIA_Prop_Entries,total[j],MUIA_Prop_Visible,visible[j],MUIA_Prop_First,first,TAG_DONE);
 }
}
static int ui_scroll_pages(Object *pages){
 Object *children[PAGE_COUNT],*child;struct List *list;APTR state;unsigned count=0;
 get(pages,MUIA_Group_ChildList,&list);state=list->lh_Head;
 while((child=NextObject(&state))&&count<PAGE_COUNT)children[count++]=child;
 for(unsigned i=0;i<count;i++){
  struct ui_scroll*s=&ui_scroll_objects[i];DoMethod(pages,OM_REMMEMBER,(ULONG)children[i]);
  s->view=NewObject(viewport_class->mcc_Class,NULL,MUIA_Group_Child,children[i],MUIA_Group_Spacing,0,InnerSpacing(0,0),TAG_DONE);
  s->bar[0]=PropObject,PropFrame,MUIA_Prop_Horiz,TRUE,MUIA_FixHeight,12,MUIA_ShowMe,FALSE,End;
  s->bar[1]=PropObject,PropFrame,MUIA_Prop_Horiz,FALSE,MUIA_FixWidth,12,MUIA_ShowMe,FALSE,End;
  s->group=VGroup,MUIA_Group_Spacing,2,InnerSpacing(0,0),Child,HGroup,MUIA_Group_Spacing,2,Child,s->view,Child,s->bar[1],End,Child,s->bar[0],End;
  if(!s->group)return 0;
  DoMethod(s->bar[0],MUIM_Notify,MUIA_Prop_First,MUIV_EveryTime,s->view,3,MUIM_Set,MUIA_Virtgroup_Left,MUIV_TriggerValue);
  DoMethod(s->bar[1],MUIM_Notify,MUIA_Prop_First,MUIV_EveryTime,s->view,3,MUIM_Set,MUIA_Virtgroup_Top,MUIV_TriggerValue);
  DoMethod(pages,OM_ADDMEMBER,(ULONG)s->group);ui_scroll_count++;
 }
 return 1;
}
