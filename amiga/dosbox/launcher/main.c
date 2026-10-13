/* GPL-3.0-or-later. ZZDosBox MUI launcher; Core1 stays owned by the player. */
#include <exec/memory.h>
#include <libraries/mui.h>
#include <libraries/asl.h>
#include <dos/dostags.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/muimaster.h>
#include <proto/asl.h>
#include <proto/wb.h>
#include <clib/alib_protos.h>
#include "model.h"
#pragma GCC diagnostic ignored "-Wpointer-sign"
struct Library *MUIMasterBase,*AslBase,*WorkbenchBase;
static struct dl_library library;
static Object *app,*win,*list,*title,*drawer,*program,*helper,*edited,*memory,*machine,*speed,*fg,*bg,*live,*status,*command,*bootdisk;
struct editor {Object *list,*line;char rows[128][256];unsigned count;};
static struct editor editors[2];
static int selected=-1,busy;
static char appdir[DL_PATH],configpath[DL_PATH],message[256];
static const char *tabs[]={"Programs","AUTOEXEC.BAT","CONFIG.SYS","DOS terminal","Settings","Disks","About",NULL};
static const char *memories[]={"1 MB","2 MB","4 MB","8 MB",NULL},*machines[]={"VGA","EGA","CGA",NULL},*speeds[]={"Slow (1000)","Normal (3000)","Fast (6000)",NULL},*colours[]={"Black","Red","Green","Yellow","Blue","Magenta","Cyan","White",NULL},*lengths[]={"Startup test (8 seconds)","Until closed",NULL};
enum {ID_SELECT=1,ID_ADD,ID_REMOVE,ID_SAVE,ID_RUN,ID_TERM,ID_COMMAND,ID_AUTO,ID_SCRIPT,ID_BROWSE_DRAWER,ID_OPEN_DRAWER,ID_DISKS,ID_MOUNT_DISKS,ID_EJECT_DISKS,ID_BOOT_DISK,ID_MOUNT_HD,ID_EJECT_HD,ID_OPEN_HD,ID_EDITOR=100};
static ULONG val(Object*o,ULONG a){ULONG v=0;get(o,a,&v);return v;}
static void note(const char*s){snprintf(message,sizeof(message),"%s",s);set(status,MUIA_Text_Contents,(ULONG)message);}
static Object *label(const char*s){return TextObject,MUIA_Text_Contents,(ULONG)s,End;}
static Object *fieldlabel(const char*s){Object*o=label(s);set(o,MUIA_Text_SetMax,TRUE);set(o,MUIA_HorizWeight,0);return o;}
static Object *str(unsigned n){return StringObject,StringFrame,MUIA_String_MaxLen,n,End;}
static Object *cycle(const char**s){return CycleObject,MUIA_Cycle_Entries,(ULONG)s,End;}
static void notify(Object*o,ULONG attr,ULONG trigger,unsigned id){DoMethod(o,MUIM_Notify,attr,trigger,(ULONG)app,2,MUIM_Application_ReturnID,id);}
static void path(char*out,const char*name){strcpy(out,appdir);AddPart(out,(STRPTR)name,DL_PATH);}
static int same_dir(const char*a,const char*b){BPTR x=Lock((STRPTR)a,ACCESS_READ),y=Lock((STRPTR)b,ACCESS_READ);int same=x&&y&&SameLock(x,y)==LOCK_SAME;if(x)UnLock(x);if(y)UnLock(y);return same;}
static int exists(const char*p){BPTR l=Lock((STRPTR)p,ACCESS_READ);if(l)UnLock(l);return !!l;}
static int save(void){char tmp[DL_PATH],bak[DL_PATH];path(tmp,"Launcher.tmp");path(bak,"Launcher.bak");if(!dl_library_write(tmp,&library))return 0;if(exists(configpath)){if(exists(bak)&&!DeleteFile(bak))return 0;if(!Rename(configpath,bak))return 0;}if(Rename(tmp,configpath))return 1;if(exists(bak))Rename(bak,configpath);return 0;}
static void editor_show(unsigned n,const char*s){struct editor*e=&editors[n];e->count=0;DoMethod(e->list,MUIM_List_Clear);while(*s&&e->count<128){unsigned j=0;while(*s&&*s!='\n'){if(*s!='\r'&&j<255)e->rows[e->count][j++]=*s;s++;}if(*s=='\n')s++;e->rows[e->count][j]=0;DoMethod(e->list,MUIM_List_InsertSingle,(ULONG)e->rows[e->count],MUIV_List_Insert_Bottom);e->count++;}set(e->line,MUIA_String_Contents,(ULONG)"");}
static int editor_read(unsigned n,char*out){struct editor*e=&editors[n];unsigned i;size_t used=0;for(i=0;i<e->count;i++){size_t len=strlen(e->rows[i]);if(used+len+2>=DL_TEXT)return 0;memcpy(out+used,e->rows[i],len);used+=len;out[used++]='\n';}out[used]=0;return 1;}
static int store_selection(void){char a[DL_TEXT],c[DL_TEXT];struct dl_profile*p;if(selected<0)return 1;if(!editor_read(0,a)||!editor_read(1,c)||!dl_text_valid(a)||!dl_text_valid(c)){note("Startup file is too long.");return 0;}p=&library.p[selected];snprintf(p->title,sizeof(p->title),"%s",(char*)val(title,MUIA_String_Contents));snprintf(p->drive,sizeof(p->drive),"%s",(char*)val(drawer,MUIA_String_Contents));snprintf(p->program,sizeof(p->program),"%s",(char*)val(program,MUIA_String_Contents));p->helper=val(helper,MUIA_Selected);p->edited=val(edited,MUIA_Selected);p->memory=1u<<val(memory,MUIA_Cycle_Active);p->machine=val(machine,MUIA_Cycle_Active);p->speed=val(speed,MUIA_Cycle_Active);p->fg=val(fg,MUIA_Cycle_Active);p->bg=val(bg,MUIA_Cycle_Active);p->live=val(live,MUIA_Cycle_Active);strcpy(p->autoexec,a);strcpy(p->configsys,c);return 1;}
static void show_selection(int n){struct dl_profile*p;if(n<0||(unsigned)n>=library.count)return;selected=n;library.last=n;p=&library.p[n];set(title,MUIA_String_Contents,(ULONG)p->title);set(drawer,MUIA_String_Contents,(ULONG)p->drive);set(program,MUIA_String_Contents,(ULONG)p->program);set(helper,MUIA_Selected,p->helper);set(edited,MUIA_Selected,p->edited);set(memory,MUIA_Cycle_Active,p->memory==8?3:p->memory==4?2:p->memory==2?1:0);set(machine,MUIA_Cycle_Active,p->machine);set(speed,MUIA_Cycle_Active,p->speed);set(fg,MUIA_Cycle_Active,p->fg);set(bg,MUIA_Cycle_Active,p->bg);set(live,MUIA_Cycle_Active,p->live);editor_show(0,p->autoexec);editor_show(1,p->configsys);}
static void refresh(void){unsigned i;busy=1;DoMethod(list,MUIM_List_Clear);for(i=0;i<library.count;i++)DoMethod(list,MUIM_List_InsertSingle,(ULONG)library.p[i].title,MUIV_List_Insert_Bottom);if(library.count){if(library.last>=library.count)library.last=0;set(list,MUIA_List_Active,library.last);show_selection(library.last);}else selected=-1;busy=0;}
static int browse(char*out,int onlydrawer,int saving,const char*caption){struct FileRequester*r;int ok=0;set(app,MUIA_Application_Sleep,TRUE);r=AllocAslRequestTags(ASL_FileRequest,ASLFR_TitleText,(ULONG)caption,ASLFR_InitialDrawer,(ULONG)(selected>=0?library.p[selected].drive:appdir),ASLFR_DrawersOnly,onlydrawer,ASLFR_DoSaveMode,saving,TAG_DONE);if(r){if(AslRequestTags(r,TAG_DONE)&&strlen(r->fr_Drawer)<DL_PATH){strcpy(out,r->fr_Drawer);ok=onlydrawer||AddPart(out,r->fr_File,DL_PATH);}FreeAslRequest(r);}set(app,MUIA_Application_Sleep,FALSE);return ok;}
static void add(void){char p[DL_PATH];struct dl_profile*x;if(library.count==DL_MAX){note("Program list is full.");return;}if(!browse(p,0,0,"Add DOS program"))return;if(!store_selection())return;x=&library.p[library.count];dl_profile_default(x);snprintf(x->program,sizeof(x->program),"%s",FilePart(p));snprintf(x->title,sizeof(x->title),"%s",FilePart(p));*PathPart(p)=0;strcpy(x->drive,p);library.last=library.count++;refresh();if(!save())note("Could not save program list.");}
static int copy_helper(const char*drive){char source[DL_PATH],dest[DL_PATH];FILE*a,*b;unsigned char data[1024];size_t n;int ok=1;path(source,"LOADFIX.COM");strcpy(dest,drive);if(!AddPart(dest,"LOADFIX.COM",sizeof(dest)))return 0;if(exists(dest))return 1;a=fopen(source,"rb");if(!a)return 0;b=fopen(dest,"wb");if(!b){fclose(a);return 0;}while((n=fread(data,1,sizeof(data),a))>0)if(fwrite(data,1,n,b)!=n){ok=0;break;}if(ferror(a))ok=0;fclose(a);if(fclose(b))ok=0;if(!ok)DeleteFile(dest);return ok;}
static void launch(unsigned mode,const char*script){
 struct dl_profile*p;struct dl_session s;char boot[DL_PATH],bootname[16],session[DL_PATH],player[DL_PATH],qp[DL_PATH*2+3],qs[DL_PATH*2+3],cmd[DL_PATH*4+64];FILE*f;unsigned i;LONG rc;BPTR log;const char*doscommand;int withhelper;
 if(selected<0){note("Add a program first.");return;}if(!store_selection())return;p=&library.p[selected];if(!dl_profile_valid(p)){note("Check settings; text and background must differ.");return;}
 if(FindPort("Sixies.ARM.Debug.Owner")){note("Another ARM application is running.");return;}
 doscommand=mode==0?p->program:mode==2?(char*)val(command,MUIA_String_Contents):mode==4?script:"";withhelper=mode==0&&p->helper;
 if(withhelper&&(strchr(doscommand,' ')||strchr(doscommand,'\t'))){note("LOADFIX currently accepts one program filename.");return;}
 if(withhelper&&!copy_helper(p->drive)){note("Could not install LOADFIX.COM in the program drawer.");return;}
 for(i=0;i<1000;i++){snprintf(bootname,sizeof(bootname),"ZZL%04u.BAT",i);strcpy(boot,p->drive);if(!AddPart(boot,bootname,sizeof(boot)))return;if(!exists(boot))break;}
 if(i==1000){note("No free startup filename.");return;}
 f=fopen(boot,"wb");if(!f){note("Program drawer must be writable.");return;}
 {int ok=dl_boot(f,p,doscommand,mode==0||mode==3,withhelper);if(fclose(f))ok=0;if(!ok){DeleteFile(boot);note("Invalid or oversized startup command.");return;}}
 dl_defaults(&s);strcpy(s.drive,p->drive);strcpy(s.command,bootname);s.memory=p->memory;s.machine=p->machine;s.cycles=p->speed==0?1000:p->speed==1?3000:6000;s.fg=p->fg;s.bg=p->bg;s.live=p->live;s.terminal=mode!=0||!*doscommand;
 path(session,"Launch.session");path(player,"ZZDosBox");if(!save()||!dl_session_write(session,&s)||!dl_quote(qp,sizeof(qp),player)||!dl_quote(qs,sizeof(qs),session)){DeleteFile(boot);note("Could not save launch settings.");return;}
 snprintf(cmd,sizeof(cmd),"%s --session %s",qp,qs);path(player,"LastRun.log");log=Open(player,MODE_NEWFILE);if(!log){DeleteFile(boot);note("Cannot open session log.");return;}
 set(win,MUIA_Window_Open,FALSE);rc=SystemTags(cmd,SYS_Input,0,SYS_Output,log,SYS_Asynch,FALSE,NP_StackSize,65536,TAG_DONE);Close(log);DeleteFile(boot);DeleteFile(session);set(win,MUIA_Window_Open,TRUE);note(rc==0?"Session finished.":"Session failed; see LastRun.log.");
}
static Object *editor_page(unsigned n){Object *add,*replace,*remove,*up,*down,*load,*export,*reset,*view;struct editor*e=&editors[n];Object*o=VGroup,
 Child,label(n?"Embedded DOS: reference only. Booted DOS uses the disk CONFIG.SYS.":"AUTOEXEC.BAT"),
 Child,view=ListviewObject,MUIA_Listview_List,e->list=ListObject,InputListFrame,MUIA_List_ConstructHook,MUIV_List_ConstructHook_String,MUIA_List_DestructHook,MUIV_List_DestructHook_String,End,End,
 Child,e->line=str(256),Child,HGroup,Child,add=SimpleButton("Add"),Child,replace=SimpleButton("Replace"),Child,remove=SimpleButton("Delete"),Child,up=SimpleButton("Up"),Child,down=SimpleButton("Down"),End,
 Child,HGroup,Child,load=SimpleButton("Import..."),Child,export=SimpleButton("Export..."),Child,reset=SimpleButton("Default"),End,End;
 /* Notifications are installed after the Application object exists. */
 set(add,MUIA_UserData,ID_EDITOR+n*16+1);set(replace,MUIA_UserData,ID_EDITOR+n*16+2);set(remove,MUIA_UserData,ID_EDITOR+n*16+3);set(up,MUIA_UserData,ID_EDITOR+n*16+4);set(down,MUIA_UserData,ID_EDITOR+n*16+5);set(load,MUIA_UserData,ID_EDITOR+n*16+6);set(export,MUIA_UserData,ID_EDITOR+n*16+7);set(reset,MUIA_UserData,ID_EDITOR+n*16+8);
 (void)view;return o;
}
static void editor_action(unsigned n,unsigned action){struct editor*e=&editors[n];LONG at=(LONG)val(e->list,MUIA_List_Active);char text[DL_TEXT],p[DL_PATH];const char*line=(char*)val(e->line,MUIA_String_Contents);unsigned i;FILE*f;
 if(action==0){if(at>=0&&(unsigned)at<e->count)set(e->line,MUIA_String_Contents,(ULONG)e->rows[at]);return;}
 if(action==1){if(e->count>=128){note("Too many lines.");return;}snprintf(e->rows[e->count++],256,"%s",line);at=e->count-1;}
 if(action==2&&at>=0&&(unsigned)at<e->count)snprintf(e->rows[at],256,"%s",line);
 if(action==3&&at>=0&&(unsigned)at<e->count){memmove(e->rows[at],e->rows[at+1],(--e->count-at)*256);at=-1;}
 if((action==4||action==5)&&at>=0&&(unsigned)at<e->count){int to=at+(action==4?-1:1);if(to>=0&&(unsigned)to<e->count){char temp[256];strcpy(temp,e->rows[at]);strcpy(e->rows[at],e->rows[to]);strcpy(e->rows[to],temp);at=to;}}
 if(action==6){size_t size;if(!browse(p,0,0,n?"Import CONFIG.SYS":"Import AUTOEXEC.BAT"))return;f=fopen(p,"rb");if(!f){note("Cannot read file.");return;}size=fread(text,1,sizeof(text)-1,f);if(fgetc(f)!=EOF||ferror(f)){fclose(f);note("File is too large.");return;}fclose(f);text[size]=0;if(!dl_text_valid(text)){note("Use a plain ASCII text file.");return;}editor_show(n,text);return;}
 if(action==7){int ok;if(!editor_read(n,text)||!browse(p,0,1,n?"Export CONFIG.SYS":"Export AUTOEXEC.BAT"))return;if(exists(p)&&!MUI_Request(app,win,0,"Replace file?","_Replace|_Cancel","%s",p))return;f=fopen(p,"wb");if(!f){note("Cannot write file.");return;}ok=dl_dos_text(f,text);if(fclose(f))ok=0;note(ok?"File exported.":"Write failed.");return;}
 if(action==8){editor_show(n,n?dl_default_config:dl_default_autoexec);return;}
 DoMethod(e->list,MUIM_List_Clear);for(i=0;i<e->count;i++)DoMethod(e->list,MUIM_List_InsertSingle,(ULONG)e->rows[i],MUIV_List_Insert_Bottom);if(at>=0)set(e->list,MUIA_List_Active,at);
}
/* Find action buttons through group children, without allocating hook state. */
static void wire(Object*o){struct List*children=NULL;Object*child;APTR state;ULONG id=val(o,MUIA_UserData);if(id)notify(o,MUIA_Pressed,FALSE,id);get(o,MUIA_Group_ChildList,&children);if(children){state=children->lh_Head;while((child=NextObject(&state)))wire(child);}}
static const char *bootlabels[]={"MS-DOS hard disk","DOS boot disk","Setup disk 1","Setup disk 2","Setup disk 3","Supplemental tools",NULL};
static const char *bootfiles[]={"DOS622.IMG","DOSBOOT.IMG","DISK1.IMG","DISK2.IMG","DISK3.IMG","SUPPDISK.IMG"};
static void boot_disk(void){
 char player[DL_PATH],drive[DL_PATH],qp[DL_PATH*2+3],qd[DL_PATH*2+3],cmd[DL_PATH*4+96];unsigned n=val(bootdisk,MUIA_Cycle_Active);BPTR log;LONG rc;
 if(n>=6)return;if(FindPort("Sixies.ARM.Debug.Owner")){note("Another ARM application is running.");return;}
 if(!n){char image[DL_PATH];BPTR lock;path(image,"Disks/DOS622.IMG");lock=Lock(image,EXCLUSIVE_LOCK);if(!lock){note("Hard disk unavailable; eject it from Workbench first.");return;}UnLock(lock);}
 path(player,"ZZDosBox");path(drive,"Disks");if(!dl_quote(qp,sizeof(qp),player)||!dl_quote(qd,sizeof(qd),drive))return;
 snprintf(cmd,sizeof(cmd),"%s --drive %s %s %s --memory %u",qp,qd,n?"--boot":"--hdd",bootfiles[n],1u<<val(memory,MUIA_Cycle_Active));path(player,"LastBoot.log");log=Open(player,MODE_NEWFILE);if(!log){note("Cannot open boot log.");return;}
 set(win,MUIA_Window_Open,FALSE);rc=SystemTags(cmd,SYS_Input,0,SYS_Output,log,SYS_Asynch,FALSE,NP_StackSize,65536,TAG_DONE);Close(log);set(win,MUIA_Window_Open,TRUE);note(rc==0?"Session finished.":"Boot failed; see LastBoot.log.");
}
static int build(void){Object *add,*remove,*save_button,*run,*term,*execute,*auto_run,*script,*browse_button,*open_drawer,*open_disks,*mount_disks,*eject_disks,*boot_button,*mount_hd,*eject_hd,*open_hd,*root;
 app=ApplicationObject,MUIA_Application_Title,(ULONG)"ZZDosBox",MUIA_Application_Base,(ULONG)"ZZDOSBOX",MUIA_Application_Version,(ULONG)"$VER: ZZDosBox Launcher 0.8.0-dev (13.10.2026)",
 SubWindow,win=WindowObject,MUIA_Window_Title,(ULONG)"ZZDosBox Launcher",MUIA_Window_ID,0x5a444c31,MUIA_Window_Width,820,MUIA_Window_Height,440,
 WindowContents,root=VGroup,
 Child,RegisterGroup(tabs),
 Child,VGroup,
  Child,ListviewObject,MUIA_Listview_List,list=ListObject,InputListFrame,MUIA_List_ConstructHook,MUIV_List_ConstructHook_String,MUIA_List_DestructHook,MUIV_List_DestructHook_String,End,End,
  Child,HGroup,Child,add=SimpleButton("Add program..."),Child,remove=SimpleButton("Remove"),End,
  Child,ColGroup(2),Child,fieldlabel("Name"),Child,title=str(64),Child,fieldlabel("DOS drive C:"),Child,HGroup,Child,drawer=str(DL_PATH),Child,browse_button=SimpleButton("Browse..."),End,Child,fieldlabel("Program"),Child,program=str(128),End,
  Child,HGroup,Child,helper=CheckMark(FALSE),Child,label("Low-memory compatibility (LOADFIX)"),End,
  Child,HGroup,Child,run=SimpleButton("Launch"),Child,open_drawer=SimpleButton("Open C: drawer"),End,End,
 Child,VGroup,Child,HGroup,Child,edited=CheckMark(FALSE),Child,label("Use edited AUTOEXEC.BAT"),End,Child,editor_page(0),End,
 Child,editor_page(1),
 Child,VGroup,Child,ColGroup(2),Child,fieldlabel("Text"),Child,fg=cycle(colours),Child,fieldlabel("Background"),Child,bg=cycle(colours),End,
  Child,HGroup,Child,command=str(128),Child,execute=SimpleButton("Run command"),End,
  Child,HGroup,Child,term=SimpleButton("Open terminal"),Child,auto_run=SimpleButton("Run AUTOEXEC"),Child,script=SimpleButton("Run script..."),End,
  Child,label("C: uses the selected program drawer. F10 closes a game."),End,
 Child,VGroup,Child,ColGroup(2),Child,fieldlabel("Guest RAM"),Child,memory=cycle(memories),Child,fieldlabel("Video adapter"),Child,machine=cycle(machines),Child,fieldlabel("CPU cycles"),Child,speed=cycle(speeds),Child,fieldlabel("Session"),Child,live=cycle(lengths),End,
  Child,label("Conventional DOS memory: 640 KB. XMS / EMS and audio are not available yet."),End,
 Child,VGroup,
  Child,HGroup,Child,bootdisk=cycle(bootlabels),Child,boot_button=SimpleButton("Boot"),End,
  Child,HGroup,Child,mount_hd=SimpleButton("Mount hard disk"),Child,eject_hd=SimpleButton("Eject hard disk"),Child,open_hd=SimpleButton("Open hard disk"),End,
  Child,label("Eject the Workbench hard disk before booting DOS. F10 returns here."),
  Child,HGroup,Child,mount_disks=SimpleButton("Mount floppies"),Child,eject_disks=SimpleButton("Eject floppies"),End,
  Child,open_disks=SimpleButton("Open image drawer"),End,
 Child,VGroup,Child,label("ZZDosBox 0.8.0-dev"),Child,label("DOSBox / libretro contributors"),Child,label("XACP / Xani - ZZ9000 Cortex-A9"),Child,label("MUI - Stefan Stuntz"),Child,label("Development: Codex assisted, Amiga-Bridge MCP"),End,
 End,Child,status=label(""),Child,save_button=SimpleButton("Save settings"),End,End,End;
 if(!app)return 0;
 notify(win,MUIA_Window_CloseRequest,TRUE,MUIV_Application_ReturnID_Quit);notify(list,MUIA_List_Active,MUIV_EveryTime,ID_SELECT);
 notify(add,MUIA_Pressed,FALSE,ID_ADD);notify(remove,MUIA_Pressed,FALSE,ID_REMOVE);notify(run,MUIA_Pressed,FALSE,ID_RUN);notify(save_button,MUIA_Pressed,FALSE,ID_SAVE);notify(term,MUIA_Pressed,FALSE,ID_TERM);notify(execute,MUIA_Pressed,FALSE,ID_COMMAND);notify(auto_run,MUIA_Pressed,FALSE,ID_AUTO);notify(script,MUIA_Pressed,FALSE,ID_SCRIPT);notify(browse_button,MUIA_Pressed,FALSE,ID_BROWSE_DRAWER);
 notify(mount_hd,MUIA_Pressed,FALSE,ID_MOUNT_HD);notify(eject_hd,MUIA_Pressed,FALSE,ID_EJECT_HD);notify(open_hd,MUIA_Pressed,FALSE,ID_OPEN_HD);
 notify(boot_button,MUIA_Pressed,FALSE,ID_BOOT_DISK);
 notify(open_drawer,MUIA_Pressed,FALSE,ID_OPEN_DRAWER);notify(open_disks,MUIA_Pressed,FALSE,ID_DISKS);notify(mount_disks,MUIA_Pressed,FALSE,ID_MOUNT_DISKS);notify(eject_disks,MUIA_Pressed,FALSE,ID_EJECT_DISKS);
 wire(root);notify(editors[0].list,MUIA_List_Active,MUIV_EveryTime,ID_EDITOR);notify(editors[1].list,MUIA_List_Active,MUIV_EveryTime,ID_EDITOR+16);return 1;
}
static void seed(void){const char*dirs[]={"KEEN6","VETTE","ELITE"},*titles[]={"Commander Keen 6","Vette","Elite"},*programs[]={"KEEN6C.EXE","VETTE.EXE","ELITE.EXE"};char parent[DL_PATH];unsigned i;strcpy(parent,appdir);*PathPart(parent)=0;
 {struct dl_profile*p=&library.p[library.count];dl_profile_default(p);strcpy(p->title,"DOS hard drive");strcpy(p->drive,parent);AddPart(p->drive,"Drives/C",DL_PATH);if(exists(p->drive))library.count++;}
 for(i=0;i<3;i++){struct dl_profile*p=&library.p[library.count];dl_profile_default(p);strcpy(p->title,titles[i]);strcpy(p->drive,parent);AddPart(p->drive,"Shell/Games",DL_PATH);AddPart(p->drive,(STRPTR)dirs[i],DL_PATH);strcpy(p->program,programs[i]);p->helper=i!=0;if(exists(p->drive))library.count++;}}
int main(void){ULONG signals,id;int running=1,rc=20;struct MsgPort*owner=NULL;char p[DL_PATH];
 if(FindPort("ZZDOS_LAUNCHER"))return 0;owner=CreateMsgPort();if(!owner)return 20;owner->mp_Node.ln_Name="ZZDOS_LAUNCHER";AddPort(owner);
 if(!NameFromLock(GetProgramDir(),appdir,sizeof(appdir)))goto done;path(configpath,"Launcher.cfg");
 if(exists(configpath)){if(!dl_library_read(configpath,&library)){puts("Invalid Launcher.cfg; restore Launcher.bak. No changes made.");goto done;}}else seed();
 MUIMasterBase=OpenLibrary("muimaster.library",15);AslBase=OpenLibrary("asl.library",38);WorkbenchBase=OpenLibrary("workbench.library",44);if(!MUIMasterBase||!AslBase||!WorkbenchBase){puts("MUI 3.8 or later is required.");goto done;}
 if(!build())goto done;refresh();set(win,MUIA_Window_Open,TRUE);if(!val(win,MUIA_Window_Open))goto done;
 while(running){id=DoMethod(app,MUIM_Application_NewInput,(ULONG)&signals);
  switch(id){case (ULONG)MUIV_Application_ReturnID_Quit:if(!store_selection())break;if(!save()){note("Cannot save settings.");break;}running=0;break;
  case ID_SELECT:if(!busy){int n=(LONG)val(list,MUIA_List_Active);if(n!=selected){if(store_selection())show_selection(n);else{busy=1;set(list,MUIA_List_Active,selected);busy=0;}}}break;
  case ID_ADD:add();break;case ID_REMOVE:if(selected>=0){unsigned n=selected;memmove(&library.p[n],&library.p[n+1],(--library.count-n)*sizeof(library.p[0]));library.last=0;refresh();save();}break;
  case ID_SAVE:if(!store_selection())break;note(save()?"Settings saved.":"Could not save settings.");refresh();break;
  case ID_RUN:launch(0,NULL);break;case ID_TERM:launch(1,NULL);break;case ID_COMMAND:launch(2,NULL);break;case ID_AUTO:launch(3,NULL);break;
  case ID_SCRIPT:if(browse(p,0,0,"Select DOS batch file")){char dir[DL_PATH];strcpy(dir,p);*PathPart(dir)=0;store_selection();if(selected>=0&&same_dir(dir,library.p[selected].drive))launch(4,FilePart(p));else note("Choose a batch file inside the selected C: drawer.");}break;
  case ID_OPEN_DRAWER:if(selected>=0)OpenWorkbenchObjectA((STRPTR)val(drawer,MUIA_String_Contents),NULL);break;
  case ID_MOUNT_HD:case ID_EJECT_HD:{char q[DL_PATH*2+3],cmd[DL_PATH*2+32];LONG rc;path(p,"ZZDosDisk");if(dl_quote(q,sizeof(q),p)){snprintf(cmd,sizeof(cmd),"%s %s",q,id==ID_MOUNT_HD?"mount":"eject");set(app,MUIA_Application_Sleep,TRUE);rc=SystemTags(cmd,SYS_Asynch,FALSE,NP_StackSize,16384,TAG_DONE);set(app,MUIA_Application_Sleep,FALSE);note(rc?"Disk unavailable; close DOS and open files first.":id==ID_MOUNT_HD?"Hard disk mounted.":"Hard disk ejected.");}break;}
  case ID_OPEN_HD:OpenWorkbenchObjectA("DOSHD:",NULL);break;
  case ID_BOOT_DISK:boot_disk();break;
  case ID_DISKS:path(p,"Disks");OpenWorkbenchObjectA(p,NULL);break;
  case ID_MOUNT_DISKS:case ID_EJECT_DISKS:{char quoted[DL_PATH*2+3],cmd[DL_PATH*2+32];path(p,id==ID_MOUNT_DISKS?"Disks/Mount-DOS-Disks":"Disks/Eject-DOS-Disks");if(dl_quote(quoted,sizeof(quoted),p)){LONG result;snprintf(cmd,sizeof(cmd),"Execute %s",quoted);set(app,MUIA_Application_Sleep,TRUE);result=SystemTags(cmd,SYS_Asynch,FALSE,NP_StackSize,16384,TAG_DONE);set(app,MUIA_Application_Sleep,FALSE);note(result==0?(id==ID_MOUNT_DISKS?"Disks mounted in Workbench.":"Disks ejected."):"Disk action failed.");}break;}
  case ID_BROWSE_DRAWER:if(browse(p,1,0,"Select C: drawer"))set(drawer,MUIA_String_Contents,(ULONG)p);break;
  default:if(id>=ID_EDITOR&&id<ID_EDITOR+32)editor_action((id-ID_EDITOR)/16,(id-ID_EDITOR)%16);break;}
  if(running&&!id&&signals){if(Wait(signals|SIGBREAKF_CTRL_C)&SIGBREAKF_CTRL_C){if(store_selection()&&save())running=0;}}
 }rc=0;
done:if(app)MUI_DisposeObject(app);if(WorkbenchBase)CloseLibrary(WorkbenchBase);if(AslBase)CloseLibrary(AslBase);if(MUIMasterBase)CloseLibrary(MUIMasterBase);if(owner){RemPort(owner);DeleteMsgPort(owner);}return rc;
}
