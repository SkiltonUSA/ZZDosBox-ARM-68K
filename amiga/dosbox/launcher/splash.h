/* GPL-3.0-or-later. Native MUI startup/loading panels; no external image/datatype needed. */
#include <devices/timer.h>
#include "splash-art.h"
enum {ID_SPLASH_DISMISS=700};
static const ULONG splash_colours[]={
 0,0,0, 0,0,0, 0xffffffff,0xffffffff,0xffffffff, 0,0,0
};
/* Groups do not forward clicks from their children on MUI 3.8. */
static void splash_clickable(Object *o){
 struct List *children=NULL;Object *child;APTR state;
 SetAttrs(o,MUIA_InputMode,MUIV_InputMode_RelVerify,MUIA_ShowSelState,FALSE,TAG_DONE);
 notify(o,MUIA_Pressed,FALSE,ID_SPLASH_DISMISS);
 get(o,MUIA_Group_ChildList,&children);
 if(children){state=children->lh_Head;while((child=NextObject(&state)))splash_clickable(child);}
}
static int splash_show(int loading){
 Object *panel,*window,*copy;struct MsgPort *port;struct timerequest *timer;
 ULONG signals,id;int proceed=1;
 /* Failure to create this optional presentation must not prevent launching. */
 port=CreateMsgPort();if(!port)return 1;
 timer=(struct timerequest*)CreateIORequest(port,sizeof(*timer));
 if(!timer){DeleteMsgPort(port);return 1;}
 if(OpenDevice(TIMERNAME,UNIT_MICROHZ,(struct IORequest*)timer,0)){
  DeleteIORequest((struct IORequest*)timer);DeleteMsgPort(port);return 1;
 }
 copy=loading?VGroup,MUIA_Font,ui_font(),MUIA_Group_Spacing,14,
  Child,VSpace(0),
  Child,TextObject,MUIA_Font,ui_title_font(),MUIA_Text_Contents,(ULONG)"LOADING",End,
  Child,ColGroup(2),MUIA_Group_HorizSpacing,18,MUIA_Group_VertSpacing,8,
   Child,label("Alt + Return"),Child,label("Fullscreen / window"),
   Child,label("F10"),Child,label("Exit to launcher"),
   Child,label("Ctrl + ="),Child,label("Faster CPU cycles"),
   Child,label("Ctrl + -"),Child,label("Slower CPU cycles"),End,
  Child,VSpace(0),End:
  VGroup,MUIA_Group_Spacing,10,
   Child,VSpace(0),
   Child,TextObject,MUIA_Font,ui_title_font(),MUIA_Text_Contents,(ULONG)"ZZDosBox",End,
   Child,TextObject,MUIA_Font,ui_font(),MUIA_Text_Contents,(ULONG)"Version " ZZ_LAUNCHER_VERSION,End,
   Child,TextObject,MUIA_Font,ui_font(),MUIA_Text_Contents,(ULONG)"AmigaOS / ZZ9000",End,
   Child,VSpace(0),End;
 window=WindowObject,MUIA_Window_Title,(ULONG)(loading?"ZZDosBox Loading":"ZZDosBox Startup"),
  MUIA_Window_Borderless,TRUE,MUIA_Window_CloseGadget,FALSE,
  MUIA_Window_SizeGadget,FALSE,MUIA_Window_DepthGadget,FALSE,
  MUIA_Window_DragBar,FALSE,MUIA_Window_Activate,TRUE,
  MUIA_Window_LeftEdge,MUIV_Window_LeftEdge_Centered,
  MUIA_Window_TopEdge,MUIV_Window_TopEdge_Centered,
  WindowContents,panel=HGroup,GroupFrame,MUIA_Background,MUII_GroupBack,
   MUIA_InputMode,MUIV_InputMode_RelVerify,MUIA_Group_Spacing,24,InnerSpacing(28,24),
   Child,BodychunkObject,MUIA_FixWidth,128,MUIA_FixHeight,128,
    MUIA_Bitmap_Width,128,MUIA_Bitmap_Height,128,MUIA_Bodychunk_Depth,2,
    MUIA_Bodychunk_Body,(ULONG)splash_body,MUIA_Bodychunk_Compression,0,
    MUIA_Bodychunk_Masking,0,MUIA_Bitmap_SourceColors,(ULONG)splash_colours,
    MUIA_Bitmap_Transparent,0,End,
   Child,copy,End,End;
 if(window){
  DoMethod(app,OM_ADDMEMBER,(ULONG)window);
  splash_clickable(panel);
  notify(window,MUIA_Window_CloseRequest,TRUE,ID_SPLASH_DISMISS);
  set(window,MUIA_Window_Open,TRUE);
  if(val(window,MUIA_Window_Open)){
   timer->tr_node.io_Command=TR_ADDREQUEST;timer->tr_time.tv_secs=loading?4:2;timer->tr_time.tv_micro=0;
   SendIO((struct IORequest*)timer);
   for(;;){
    id=DoMethod(app,MUIM_Application_NewInput,(ULONG)&signals);
    if(id==ID_SPLASH_DISMISS)break;
    if(id==(ULONG)MUIV_Application_ReturnID_Quit){proceed=0;break;}
    if(CheckIO((struct IORequest*)timer))break;
    /* A zero signal mask means MUI still has queued work to drain. */
    if(id||!signals)continue;
    if(Wait(signals|(1ul<<port->mp_SigBit)|SIGBREAKF_CTRL_C)&SIGBREAKF_CTRL_C){proceed=0;break;}
   }
   if(!CheckIO((struct IORequest*)timer))AbortIO((struct IORequest*)timer);
   WaitIO((struct IORequest*)timer);
  }
  set(window,MUIA_Window_Open,FALSE);
  DoMethod(app,OM_REMMEMBER,(ULONG)window);MUI_DisposeObject(window);
 }
 CloseDevice((struct IORequest*)timer);DeleteIORequest((struct IORequest*)timer);DeleteMsgPort(port);
 return proceed;
}

static int startup_splash(void){return splash_show(0);}
static int loading_splash(void){
 int proceed;set(win,MUIA_Window_Sleep,TRUE);proceed=splash_show(1);
 set(win,MUIA_Window_Sleep,FALSE);return proceed;
}
