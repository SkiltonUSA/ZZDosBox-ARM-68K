/* GPL-3.0-or-later. console.device reads must not block the Core1 host loop. */
#include <exec/io.h>
#include <intuition/intuition.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <stdlib.h>
#include <stdio.h>
#include <graphics/displayinfo.h>
#define GfxBase ConsoleGfxBase
#include <proto/graphics.h>
struct GfxBase *ConsoleGfxBase;
#include "window.h"
struct DbWindow {
 struct Window *window;
 struct Screen *screen;
 struct MsgPort *read_port,*write_port;
 struct IOStdReq *read,*write;
 UBYTE key;
 int opened,pending,closed,themed;
 unsigned fg,bg,esc_len;UBYTE escape[32];
};
static void queue(struct DbWindow *c){
 c->read->io_Command=CMD_READ;c->read->io_Data=&c->key;c->read->io_Length=1;
 SendIO((struct IORequest *)c->read);c->pending=1;
}
static struct DbWindow *open_console(int themed,unsigned fg,unsigned bg){
 struct DbWindow *c=calloc(1,sizeof(*c));if(!c)return NULL;
 c->themed=themed;c->fg=fg;c->bg=bg;
 if(themed){
  struct Screen *pub=LockPubScreen(NULL);ULONG mode;
  static const struct ColorSpec colours[]={
   {0,0,0,0},{1,15,3,3},{2,3,15,3},{3,15,15,3},{4,3,3,15},{5,15,3,15},{6,3,15,15},{7,15,15,15},{-1,0,0,0}};
  if(!pub)goto fail;
  ConsoleGfxBase=(struct GfxBase*)OpenLibrary("graphics.library",39);if(!ConsoleGfxBase){UnlockPubScreen(NULL,pub);goto fail;}mode=GetVPModeID(&pub->ViewPort);UnlockPubScreen(NULL,pub);
  c->screen=OpenScreenTags(NULL,SA_DisplayID,mode,SA_Depth,8,SA_Title,(ULONG)"ZZDosBox terminal",SA_Colors,(ULONG)colours,TAG_DONE);
  if(!c->screen)goto fail;
 }
 c->window=OpenWindowTags(NULL,c->screen?WA_CustomScreen:TAG_IGNORE,(ULONG)c->screen,WA_Left,20,WA_Top,40,WA_Width,760,WA_Height,400,
  WA_Title,(ULONG)"ZZDosBox",WA_Flags,WFLG_DRAGBAR|WFLG_DEPTHGADGET|WFLG_CLOSEGADGET|WFLG_ACTIVATE,
  WA_IDCMP,IDCMP_CLOSEWINDOW,TAG_DONE);
 if(!c->window)goto fail;
 c->read_port=CreateMsgPort();c->write_port=CreateMsgPort();
 if(!c->read_port||!c->write_port)goto fail;
 c->read=(struct IOStdReq *)CreateIORequest(c->read_port,sizeof(*c->read));
 c->write=(struct IOStdReq *)CreateIORequest(c->write_port,sizeof(*c->write));
 if(!c->read||!c->write)goto fail;
 c->write->io_Data=c->window;c->write->io_Length=sizeof(*c->window);
 if(OpenDevice("console.device",0,(struct IORequest *)c->write,0))goto fail;
 c->opened=1;c->read->io_Device=c->write->io_Device;c->read->io_Unit=c->write->io_Unit;
 if(themed){char ansi[48];int n=snprintf(ansi,sizeof(ansi),"\033[%u;%um\033[2J\033[H",30+fg,40+bg);if(!db_window_write(c,(const UBYTE*)ansi,n))goto fail;}
 queue(c);return c;
fail:db_window_close(c);return NULL;
}
struct DbWindow *db_window_open(void){return open_console(0,7,0);}
struct DbWindow *db_window_open_colours(unsigned fg,unsigned bg){return fg<8&&bg<8&&fg!=bg?open_console(1,fg,bg):NULL;}
int db_window_closed(struct DbWindow *c){
 struct IntuiMessage *msg;
 while((msg=(struct IntuiMessage *)GetMsg(c->window->UserPort))!=NULL){
  if(msg->Class==IDCMP_CLOSEWINDOW)c->closed=1;
  ReplyMsg((struct Message *)msg);
 }
 return c->closed;
}
int db_window_read(struct DbWindow *c){
 int key;
 if(!c->pending||!CheckIO((struct IORequest *)c->read))return -1;
 WaitIO((struct IORequest *)c->read);c->pending=0;
 if(c->read->io_Error||c->read->io_Actual!=1)return -2;
 key=c->key;queue(c);return key;
}
static int raw_write(struct DbWindow *c,const UBYTE *data,ULONG n){
 c->write->io_Command=CMD_WRITE;c->write->io_Data=(APTR)data;c->write->io_Length=n;
 return DoIO((struct IORequest *)c->write)==0&&c->write->io_Actual==n;
}
int db_window_write(struct DbWindow *c,const UBYTE *data,ULONG n){
 UBYTE text[256];unsigned used=0,i;
 if(!c->themed)return raw_write(c,data,n);
 for(i=0;i<n;i++){
  UBYTE ch=data[i];
  if(c->esc_len||ch==27){
   if(used){if(!raw_write(c,text,used))return 0;used=0;}
   c->escape[c->esc_len++]=ch;
   if(c->esc_len>=2 && (c->escape[1]!='[' || (c->esc_len>2 && ch>=0x40&&ch<=0x7e) || c->esc_len==sizeof(c->escape))){
    if(c->escape[1]=='['&&ch=='m'){char theme[24];int len=snprintf(theme,sizeof(theme),"\033[%u;%um",30+c->fg,40+c->bg);if(!raw_write(c,(const UBYTE*)theme,len))return 0;}
    else if(!raw_write(c,c->escape,c->esc_len))return 0;
    c->esc_len=0;
   }
  }else{ text[used++]=ch;if(used==sizeof(text)){if(!raw_write(c,text,used))return 0;used=0;}}
 }
 return !used||raw_write(c,text,used);
}
void db_window_close(struct DbWindow *c){
 if(!c)return;
 if(c->pending){AbortIO((struct IORequest *)c->read);WaitIO((struct IORequest *)c->read);}
 if(c->opened)CloseDevice((struct IORequest *)c->write);
 if(c->read)DeleteIORequest((struct IORequest *)c->read);
 if(c->write)DeleteIORequest((struct IORequest *)c->write);
 if(c->read_port)DeleteMsgPort(c->read_port);
 if(c->write_port)DeleteMsgPort(c->write_port);
 if(c->window){db_window_closed(c);CloseWindow(c->window);}
 if(c->screen)CloseScreen(c->screen);
 if(ConsoleGfxBase){CloseLibrary((struct Library*)ConsoleGfxBase);ConsoleGfxBase=NULL;}
 free(c);
}
