/* GPL-3.0-or-later. Nonblocking requester: host transport remains serviced. */
#include <intuition/intuition.h>
#include <proto/intuition.h>
#include "video_trace.h"
static struct Window *debug_request;
static ULONG debug_token;
static char debug_message[96];
static void video_debug_close(void){
 if(debug_request){FreeSysRequest(debug_request);debug_request=NULL;}
 debug_token=0;
}
static int video_debug_service(int stopping){
 ULONG token=receive(PC_DEV_PAUSED);LONG action;
 if(stopping||!token||token==receive(PC_DEV_RESUME)){video_debug_close();return 1;}
 /* Drain the previous flip and release its OS lock before showing UI. */
 if(pc_display_locked()||pc_display_pending())return 1;
 if(!debug_request){
  struct EasyStruct request={sizeof(struct EasyStruct),0,(UBYTE*)"ZZDosBox Developer",
                            (UBYTE*)debug_message,(UBYTE*)"Continue|Next frame|Stop"};
  ULONG frame=receive(PC_DEV_AT_FRAME),phase=receive(PC_DEV_AT_PHASE);
  snprintf(debug_message,sizeof(debug_message),"%s frame %u",phase==1?"Before":"After",frame);
  pc_display_console(1);
  debug_request=BuildEasyRequestArgs(NULL,&request,0,NULL);
  if((uintptr_t)debug_request<=1){debug_request=NULL;puts("Cannot open checkpoint controls");return -1;}
  debug_token=token;
  printf("VIDEO_BREAK frame=%u phase=%u token=%u\n",frame,phase,token);fflush(stdout);
 }
 action=SysReqHandler(debug_request,NULL,FALSE);
 if(action<0)return 1;
 token=debug_token;video_debug_close();
 if(action==0)return 0;
 if(action==2){
  ULONG frame=receive(PC_DEV_AT_FRAME);
  if(frame>=10000000u)publish(PC_DEV_PHASE,0);
  else publish(PC_DEV_FRAME,frame+1);
 }else publish(PC_DEV_PHASE,0);
 publish(PC_DEV_RESUME,token);return 1;
}
