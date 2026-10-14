#include <assert.h>
#include <stdio.h>
#include "video_trace.h"
#include <intuition/intuition.h>
static uint32_t storage[(PC_BOOT_TEXT+4096)/4];
static volatile uint8_t *shared=(volatile uint8_t*)storage;
static int locked,pending,opened,freed,action=-2,failed;
static struct Window mock_window;
static ULONG receive(unsigned o){return ad_get(shared,o);}
static void publish(unsigned o,ULONG n){ad_put(shared,o,n);}
static int pc_display_locked(void){return locked;}
static int pc_display_pending(void){return pending;}
static void pc_display_console(int on){assert(on);}
struct Window *BuildEasyRequestArgs(struct Window*w,struct EasyStruct*e,ULONG flags,void*a){
 (void)w;(void)e;(void)flags;(void)a;assert(!locked&&!pending);
 if(failed)return (struct Window*)(uintptr_t)(failed-1);
 opened++;return &mock_window;
}
LONG SysReqHandler(struct Window*w,ULONG*flags,LONG wait){assert(w==&mock_window&&!wait);(void)flags;return action;}
void FreeSysRequest(struct Window*w){assert(w==&mock_window);freed++;}
#include "video_debug_host.h"
static void pause_at(unsigned token){
 publish(PC_DEV_PHASE,2);publish(PC_DEV_FRAME,2);
 publish(PC_DEV_AT_FRAME,2);publish(PC_DEV_AT_PHASE,2);publish(PC_DEV_PAUSED,token);
 action=-2;
}
int main(void){
 assert(video_debug_service(0)&&opened==0);
 pause_at(1);locked=1;assert(video_debug_service(0)&&opened==0);
 locked=0;pending=1;assert(video_debug_service(0)&&opened==0);
 pending=0;assert(video_debug_service(0)&&opened==1&&freed==0);
 assert(video_debug_service(0)&&opened==1);
 action=2;assert(video_debug_service(0)&&freed==1);
 assert(receive(PC_DEV_FRAME)==3&&receive(PC_DEV_PHASE)==2&&receive(PC_DEV_RESUME)==1);
 assert(video_debug_service(0)&&opened==1); /* Await ARM clearing pause. */
 pause_at(2);assert(video_debug_service(0));action=1;assert(video_debug_service(0));
 assert(receive(PC_DEV_PHASE)==0&&receive(PC_DEV_RESUME)==2);
 pause_at(3);assert(video_debug_service(0));action=0;assert(!video_debug_service(0));
 assert(receive(PC_DEV_RESUME)==2&&!debug_request);
 pause_at(4);assert(video_debug_service(0));assert(video_debug_service(1)&&!debug_request);
 pause_at(5);assert(video_debug_service(0));publish(PC_DEV_PAUSED,0);
 assert(video_debug_service(0)&&!debug_request&&opened==freed);
 for(failed=1;failed<=2;failed++){pause_at(6);assert(video_debug_service(0)==-1&&!debug_request);}
 puts("PASS checkpoint UI: lock/flip drain, nonblocking controls, resume/step/stop, cleanup and allocation failure");
}
