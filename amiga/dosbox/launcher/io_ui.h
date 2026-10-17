/* GPL-3.0-or-later. Host-side I/O setup and live physical joystick test. */
#include <devices/timer.h>
#include <libraries/lowlevel.h>
#include <proto/lowlevel.h>
struct Library *LowLevelBase;
static struct MsgPort *joy_timer_port;
static struct timerequest *joy_timer;
static Object *joy_diagram,*joy_device,*joy_axes;
static ULONG joy_previous=~0ul;
struct joy_diagram_data {ULONG state;unsigned buttons;LONG green;};
static struct MUI_CustomClass *joy_diagram_class;
static const ULONG joy_masks[]={JPF_JOY_UP,JPF_JOY_DOWN,JPF_JOY_LEFT,JPF_JOY_RIGHT,JPF_BUTTON_RED,JPF_BUTTON_BLUE,JPF_BUTTON_GREEN,JPF_BUTTON_YELLOW,JPF_BUTTON_FORWARD,JPF_BUTTON_REVERSE};
static const char *joy_names[]={"Up","Down","Left","Right","Red","Blue","Green","Yellow","Fwd","Rev"};
static const char *joy_symbols[]={"^","v","<",">","1","2","3","4","5","6"};
static const short joy_positions[][2]={{82,12},{82,116},{6,64},{158,64},{238,25},{314,25},{390,25},{238,95},{314,95},{390,95}};
static ULONG joy_diagram_dispatch(struct IClass*cl __asm("a0"),Object*obj __asm("a2"),Msg msg __asm("a1")){
 struct joy_diagram_data*d=INST_DATA(cl,obj);ULONG result;
 switch(msg->MethodID){
 case MUIM_Setup:
  if(!DoSuperMethodA(cl,obj,msg))return FALSE;
  d->green=ObtainBestPen(_screen(obj)->ViewPort.ColorMap,0x44444444u,0xccccccccu,0x66666666u,TAG_DONE);return TRUE;
 case MUIM_Cleanup:
  if(d->green>=0)ReleasePen(_screen(obj)->ViewPort.ColorMap,d->green);d->green=-1;break;
 case MUIM_AskMinMax:{struct MUIP_AskMinMax*m=(struct MUIP_AskMinMax*)msg;result=DoSuperMethodA(cl,obj,msg);
  m->MinMaxInfo->MinWidth+=470;m->MinMaxInfo->DefWidth+=470;m->MinMaxInfo->MaxWidth=MUI_MAXMAX;
  m->MinMaxInfo->MinHeight+=166;m->MinMaxInfo->DefHeight+=166;m->MinMaxInfo->MaxHeight+=166;return result;}
 case MUIM_Draw:{unsigned i;struct RastPort*rp;LONG left,top;
  result=DoSuperMethodA(cl,obj,msg);if(!(((struct MUIP_Draw*)msg)->flags&(MADF_DRAWOBJECT|MADF_DRAWUPDATE)))return result;
  rp=_rp(obj);left=_mleft(obj)+(_mwidth(obj)-470)/2;top=_mtop(obj);
  SetAPen(rp,_pens(obj)[MPEN_BACKGROUND]);RectFill(rp,_mleft(obj),top,_mright(obj),_mbottom(obj));
  for(i=0;i<4+d->buttons;i++){LONG x=left+joy_positions[i][0]+35,y=top+joy_positions[i][1]+16,dy,rx=i<4?28:16,ry=i<4?18:16;
   int on=(d->state&joy_masks[i])!=0;const char*t=joy_names[i];LONG n=strlen(t);
   SetAPen(rp,on&&d->green>=0?d->green:_pens(obj)[MPEN_FILL]);
   for(dy=-ry;dy<=ry;dy++){LONG dx=rx;while(dx*dx*ry*ry+dy*dy*rx*rx>rx*rx*ry*ry)dx--;Move(rp,x-dx,y+dy);Draw(rp,x+dx,y+dy);}
   SetAPen(rp,_pens(obj)[MPEN_SHADOW]);DrawEllipse(rp,x,y,rx,ry);
   SetAPen(rp,_pens(obj)[MPEN_TEXT]);Move(rp,x-TextLength(rp,joy_symbols[i],1)/2,y+4);Text(rp,joy_symbols[i],1);
   Move(rp,x-TextLength(rp,t,n)/2,y+32);Text(rp,t,n);
  }return result;}
 }return DoSuperMethodA(cl,obj,msg);
}
static void joystick_test_sample(void){
 ULONG state=ReadJoyPort(1),type=state&JP_TYPE_MASK,bits=0;char axes[64];
 struct joy_diagram_data*d=INST_DATA(joy_diagram_class->mcc_Class,joy_diagram);
 unsigned count=val(joy_count,MUIA_Cycle_Active)+1;
 if(state==joy_previous&&d->buttons==count)return;joy_previous=state;d->buttons=count;
 if(type==JP_TYPE_GAMECTLR)bits=state;else if(type==JP_TYPE_JOYSTK)bits=state&(JPF_JOY_UP|JPF_JOY_DOWN|JPF_JOY_LEFT|JPF_JOY_RIGHT|JPF_BUTTON_RED|JPF_BUTTON_BLUE);
 set(joy_device,MUIA_Text_Contents,(ULONG)(type==JP_TYPE_JOYSTK?"Standard joystick":type==JP_TYPE_GAMECTLR?"Game controller":type==JP_TYPE_MOUSE?"Mouse detected":"Not detected - move joystick or press Fire"));
 snprintf(axes,sizeof(axes),"X: %+d     Y: %+d",!!(bits&JPF_JOY_RIGHT)-!!(bits&JPF_JOY_LEFT),!!(bits&JPF_JOY_DOWN)-!!(bits&JPF_JOY_UP));
 set(joy_axes,MUIA_Text_Contents,(ULONG)axes);d->state=bits;MUI_Redraw(joy_diagram,MADF_DRAWOBJECT);
}
static void joystick_test_arm(void){joy_timer->tr_node.io_Command=TR_ADDREQUEST;joy_timer->tr_time.tv_secs=0;joy_timer->tr_time.tv_micro=50000;SendIO((struct IORequest*)joy_timer);}
static void joystick_test_active(int active){
 if(!active){
  if(joy_timer){if(!CheckIO((struct IORequest*)joy_timer))AbortIO((struct IORequest*)joy_timer);WaitIO((struct IORequest*)joy_timer);CloseDevice((struct IORequest*)joy_timer);DeleteIORequest((struct IORequest*)joy_timer);joy_timer=NULL;}
  if(joy_timer_port){DeleteMsgPort(joy_timer_port);joy_timer_port=NULL;}
  if(LowLevelBase){CloseLibrary(LowLevelBase);LowLevelBase=NULL;}return;
 }
 if(joy_timer)return;
 LowLevelBase=OpenLibrary("lowlevel.library",40);if(!LowLevelBase){set(joy_device,MUIA_Text_Contents,(ULONG)"lowlevel.library V40 unavailable");return;}
 joy_timer_port=CreateMsgPort();if(joy_timer_port)joy_timer=(struct timerequest*)CreateIORequest(joy_timer_port,sizeof(*joy_timer));
 if(!joy_timer||OpenDevice(TIMERNAME,UNIT_MICROHZ,(struct IORequest*)joy_timer,0)){
  if(joy_timer){DeleteIORequest((struct IORequest*)joy_timer);joy_timer=NULL;}
  joystick_test_active(0);set(joy_device,MUIA_Text_Contents,(ULONG)"Joystick timer unavailable");return;
 }
 joy_previous=~0ul;joystick_test_sample();joystick_test_arm();
}
static ULONG joystick_test_signal(void){return joy_timer_port?1ul<<joy_timer_port->mp_SigBit:0;}
static void joystick_test_poll(void){if(joy_timer&&CheckIO((struct IORequest*)joy_timer)){WaitIO((struct IORequest*)joy_timer);joystick_test_sample();joystick_test_arm();}}
#include "serial_ui.h"
static Object *io_page(void){return serial_page();}
static const char *joy_counts[]={"1 button","2 buttons","3 buttons","4 buttons","5 buttons","6 buttons",NULL};
static const char *joy_key_names[]={"Off","Space","Enter","Left Ctrl","Left Alt","Z","X","C","V","Left Shift","Tab","Escape",NULL};
static void joystick_controls(void){unsigned i,count=val(joy_count,MUIA_Cycle_Active)+1;
 for(i=0;i<4;i++)set(joy_binding[i],MUIA_Disabled,count<i+3);
 if(LowLevelBase){joy_previous=~0ul;joystick_test_sample();}
 else {struct joy_diagram_data*d=INST_DATA(joy_diagram_class->mcc_Class,joy_diagram);d->buttons=count;MUI_Redraw(joy_diagram,MADF_DRAWOBJECT);}
}
static Object *joy_label(const char*s){Object*o=label(s);set(o,MUIA_Text_SetMax,FALSE);return o;}
static Object *joystick_page(void){return VGroup,UI_PAGE,
 Child,VGroup,UI_PANEL("DOS gameport"),Child,ColGroup(2),
 Child,fieldlabel("Enable for this program"),Child,joystick=CheckMark(FALSE),
 Child,fieldlabel("Controller"),Child,joy_count=cycle(joy_counts),
 Child,fieldlabel("Input"),Child,joy_label("Amiga port 2 (joystick)"),
 Child,fieldlabel("Guest device"),Child,joy_label("Two axes / buttons 1-2 - I/O 201h"),End,
 Child,RectangleObject,MUIA_FixWidth,470,MUIA_FixHeight,0,End,End,
 Child,VGroup,UI_PANEL("Joystick and button test"),Child,VGroup,MUIA_Background,MUII_GroupBack,
 Child,joy_device=joy_label(""),Child,joy_diagram=NewObject(joy_diagram_class->mcc_Class,NULL,TAG_DONE),
 Child,joy_axes=joy_label("X: +0     Y: +0"),End,End,
 Child,ColGroup(4),UI_PANEL("Extra buttons - DOS keys"),
 Child,fieldlabel("3 / Green"),Child,joy_binding[0]=cycle(joy_key_names),
 Child,fieldlabel("4 / Yellow"),Child,joy_binding[1]=cycle(joy_key_names),
 Child,fieldlabel("5 / Forward"),Child,joy_binding[2]=cycle(joy_key_names),
 Child,fieldlabel("6 / Reverse"),Child,joy_binding[3]=cycle(joy_key_names),End,
 Child,joy_label("Buttons 3-6 require a lowlevel-compatible gamepad / adapter."),Child,VSpace(0),End;}
