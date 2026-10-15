/* GPL-3.0-or-later. ZZDosBox MUI launcher; Core1 stays owned by the player. */
#include <exec/memory.h>
#include <utility/hooks.h>
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
#include "developer.h"
#include "conf.h"
#pragma GCC diagnostic ignored "-Wpointer-sign"
#include "style.h"
#include "version.h"
#include "program_list.h"
#include "changelog_text.h"
#include "sidebar-art.h"
static const ULONG sidebar_colours[]={0,0,0, 0,0,0, 0xffffffff,0xffffffff,0xffffffff, 0,0,0};
struct Library *MUIMasterBase,*AslBase,*WorkbenchBase;
static struct dl_library library;
static Object *app,*win,*list,*title,*drawer,*program,*helper,*memory,*machine,*speed,*fg,*bg,*live,*status,*command,*bootdisk,*display,*window_size,*fps,*system_choice,*browse_button,*open_drawer,*terminal_reg,*cd_mode,*cd_path,*cd_browse;
static Object *program_views[DL_CATEGORIES+1];
static Object *editor_pages[2],*program_tabs,*disk_image,*category,*category_lists[DL_CATEGORIES],*category_run[DL_CATEGORIES],*category_edit[DL_CATEGORIES];
static const char *categories[]={"Games","Applications","OS",NULL};
enum {PAGE_LOAD,PAGE_TERMINAL,PAGE_CPU,PAGE_DISPLAY,PAGE_AUDIO,PAGE_NETWORK,PAGE_DISKS,PAGE_DEVELOPER,PAGE_COMPATIBILITY,PAGE_ABOUT,PAGE_COUNT};
static Object *midi,*midi_port,*scanlines,*presentation,*monochrome,*renderer,*scanline_help;
static Object *dev_trace,*dev_profile,*dev_break,*dev_frame,*dev_handoff,*dev_poll;
static char dev_arguments[192];
static const char *last_log="LastRun.log";
static const char *developer_tabs[]={"Diagnostics","Memory","dosbox.conf",NULL};
static struct dc_config conf;
static struct dl_profile effective_profile;
static const char *active_runtime;
static unsigned active_trace,active_break;
static const char *break_choices[]={"Off","Before frame","After frame",NULL};
static const char *handoff_choices[]={"Default","Serial","Pipelined",NULL};
static const char *poll_choices[]={"Default","1 ms","4 ms","20 ms",NULL};
static const char *renderer_choices[]={"Direct","OpenGL (experimental)",NULL};
static const char *mono_choices[]={"White","Amber","Green","Paperwhite",NULL};
static const char *presentation_choices[]={"Normal","Adaptive",NULL};
static const char *scanline_choices[]={"Off","Classic","Soft","Gradient",NULL};
static const char *midi_choices[]={"Off","Roland MT-32 (external)","General MIDI (external)",NULL};
static Object *startup_mode[2],*startup_file[2];
struct editor {Object *list,*line;char rows[128][256];unsigned count;};
static struct editor editors[3];
static int selected=-1,busy;
static char appdir[DL_PATH],configpath[DL_PATH],message[256];
static const char *sections[]={"Load program","DOS terminal","CPU & memory","Display","Audio","Network","Disks","Developer","Compatibility","About",NULL};
static Object *navigation,*navigation_view,*page_heading,*page_context,*main_pages;
enum {ID_NAVIGATE=43,ID_PROGRAM_TAB=44};
static char context_text[96];
static Object *page_icon;
static const ULONG section_icons[]={MUII_Drawer,MUII_PopFile,MUII_Chip,MUII_Chip,MUII_TapePlay,MUII_Network,MUII_HardDisk,MUII_Chip,MUII_PopFile,MUII_PopFile};
/* Separate columns keep captions aligned even when stock icons differ in width.
 * Static entries outlive the list; HookEntry bridges MUI's register ABI. */
struct nav_entry {char icon[32];const char *caption;};
static struct nav_entry nav_entries[PAGE_COUNT];
static ULONG navigation_display(struct Hook *hook,char **columns,struct nav_entry *entry){
 (void)hook;columns[0]=entry?entry->icon:"";columns[1]=entry?(char*)entry->caption:"";return 0;
}
static struct Hook navigation_hook={{NULL,NULL},(ULONG (*)())HookEntry,(ULONG (*)())navigation_display,NULL};
static const char *library_tabs[]={"All","Games","Applications","OS",NULL};
static const char *terminal_tabs[]={"Terminal","AUTOEXEC.BAT","CONFIG.SYS","CD-ROM",NULL};
static const char *startup_choices[]={"Default","Edited","File",NULL},*cd_choices[]={"None","ISO image","Amiga CD-ROM",NULL};
static const char *sizes[]={"320 x 240","640 x 480","960 x 720",NULL};
static const char *displays[]={"Workbench window","Fullscreen",NULL},*systems[]={"DOSBox DOS 5","MS-DOS 6.22","Disk image (DOS / Windows)","DOSBox DOS 5 + image",NULL};
static const char *memories[]={"1 MB","2 MB","4 MB","8 MB",NULL},*machines[]={"VGA","EGA","CGA","S3 / VESA","Tandy","PCjr","Hercules",NULL},*speeds[]={"Slow (1000)","Normal (3000)","Fast (6000)",NULL},*colours[]={"Black","Red","Green","Yellow","Blue","Magenta","Cyan","White",NULL},*lengths[]={"Startup test (8 seconds)","Until closed",NULL};
enum {ID_SELECT=1,ID_ADD,ID_REMOVE,ID_SAVE,ID_RUN,ID_TERM,ID_COMMAND,ID_AUTO,ID_SCRIPT,ID_BROWSE_DRAWER,ID_OPEN_DRAWER,ID_DISKS,ID_MOUNT_DISKS,ID_EJECT_DISKS,ID_BOOT_DISK,ID_MOUNT_HD,ID_EJECT_HD,ID_OPEN_HD,ID_SYSTEM,ID_DISPLAY,ID_STARTMODE,ID_CD_MODE,ID_CD_BROWSE,ID_CD_CHECK,ID_TERMPAGE,ID_AUDIO_CHECK,ID_GITHUB,ID_DEV_CHANGE,ID_DEV_LOG,ID_CATEGORY,ID_CONF_VALIDATE,ID_DISK_SELECT,ID_DISK_BROWSE,ID_DISK_CREATE,ID_CAT_SELECT=50,ID_CAT_RUN=60,ID_CAT_EDIT=70,ID_EDITOR=100};
static ULONG val(Object*o,ULONG a){ULONG v=0;get(o,a,&v);return v;}
static void program_list_focus(void){
 unsigned page=val(program_tabs,MUIA_Group_ActivePage);
 if(val(main_pages,MUIA_Group_ActivePage)==PAGE_LOAD&&page<=DL_CATEGORIES)
  set(win,MUIA_Window_ActiveObject,(ULONG)program_views[page]);
}
static void note(const char*s){snprintf(message,sizeof(message),"%s",s);set(status,MUIA_Text_Contents,(ULONG)message);}
/* Navigation changes only the visible page. Unsaved controls remain alive. */
static void navigation_show(unsigned page){
 if(page>=PAGE_COUNT)page=val(main_pages,MUIA_Group_ActivePage);
 SetAttrs(navigation,MUIA_NoNotify,TRUE,MUIA_List_Active,page,TAG_DONE);
 set(main_pages,MUIA_Group_ActivePage,page);
 set(page_heading,MUIA_Text_Contents,(ULONG)sections[page]);set(page_icon,MUIA_Image_Spec,section_icons[page]);
 snprintf(context_text,sizeof(context_text),"Program: %s",selected>=0?(char*)val(title,MUIA_String_Contents):"No program selected");
 set(page_context,MUIA_Text_Contents,(ULONG)context_text);
}
static void display_help_update(void){const char *text;
 if(!scanline_help)return;
 if(val(system_choice,MUIA_Cycle_Active)==2)text="Scanlines\nUnavailable for disk-image / Windows\nsessions in the current runtime.";
 else if(!val(display,MUIA_Cycle_Active))text="Scanlines\nDisabled in windowed mode.\nUse Fullscreen with Show FPS off.";
 else if(val(fps,MUIA_Selected))text="Scanlines\nDisabled while Show FPS is on.\nTurn Show FPS off to choose a style.";
 else text="Scanlines\nClassic: black alternate lines.\nSoft: dim lines. Gradient: blended bands.";
 set(scanline_help,MUIA_Text_Contents,(ULONG)text);
}
static void display_controls(void){set(monochrome,MUIA_Disabled,val(machine,MUIA_Cycle_Active)!=PC_ADAPTER_HERCULES);set(window_size,MUIA_Disabled,val(display,MUIA_Cycle_Active)!=0);set(scanlines,MUIA_Disabled,val(system_choice,MUIA_Cycle_Active)==2||!dl_scanlines_available(val(display,MUIA_Cycle_Active),val(fps,MUIA_Selected)));display_help_update();}
static void system_controls(void){ULONG kind=val(system_choice,MUIA_Cycle_Active),booted=kind==1||kind==2;
 set(disk_image,MUIA_Disabled,kind<2);
 set(renderer,MUIA_Disabled,kind==2);set(presentation,MUIA_Disabled,kind==2);set(scanlines,MUIA_Disabled,kind==2||!dl_scanlines_available(val(display,MUIA_Cycle_Active),val(fps,MUIA_Selected)));
 set(scanlines,MUIA_ShortHelp,(ULONG)(kind==2?"Not supported by the disk-image / Windows runtime.":"Requires fullscreen with Show FPS off."));
 set(editor_pages[0],MUIA_Disabled,kind==2);set(editor_pages[1],MUIA_Disabled,kind==2);
 set(fg,MUIA_Disabled,booted);set(bg,MUIA_Disabled,booted);set(startup_mode[1],MUIA_Disabled,!booted);
 set(drawer,MUIA_Disabled,kind==1);set(browse_button,MUIA_Disabled,kind==1);set(open_drawer,MUIA_Disabled,kind==1);set(live,MUIA_Disabled,booted);display_help_update();
}
static Object *label(const char*s){return TextObject,MUIA_Text_Contents,(ULONG)s,End;}
static Object *fieldlabel(const char*s){Object*o=label(s);set(o,MUIA_Text_SetMax,TRUE);set(o,MUIA_HorizWeight,0);return o;}
/* Equal text columns across both Display panels, scaled with the UI font. */
static Object *displaylabel(const char*s){return TextObject,MUIA_Font,ui_font(),MUIA_Text_Contents,(ULONG)s,MUIA_FixWidthTxt,(ULONG)"Monochrome colour",MUIA_HorizWeight,0,End;}
static Object *str(unsigned n){return StringObject,StringFrame,MUIA_Font,ui_font(),MUIA_String_MaxLen,n,End;}
static Object *cycle(const char**s){return CycleObject,MUIA_Font,ui_font(),MUIA_Cycle_Entries,(ULONG)s,End;}
static void notify(Object*o,ULONG attr,ULONG trigger,unsigned id){DoMethod(o,MUIM_Notify,attr,trigger,(ULONG)app,2,MUIM_Application_ReturnID,id);}
static void path(char*out,const char*name){strcpy(out,appdir);AddPart(out,(STRPTR)name,DL_PATH);}
static int same_dir(const char*a,const char*b){BPTR x=Lock((STRPTR)a,ACCESS_READ),y=Lock((STRPTR)b,ACCESS_READ);int same=x&&y&&SameLock(x,y)==LOCK_SAME;if(x)UnLock(x);if(y)UnLock(y);return same;}
static int exists(const char*p){BPTR l=Lock((STRPTR)p,ACCESS_READ);if(l)UnLock(l);return !!l;}
static int save(void){char tmp[DL_PATH],bak[DL_PATH];path(tmp,"Launcher.tmp");path(bak,"Launcher.bak");if(!dl_library_write(tmp,&library))return 0;if(exists(configpath)){if(exists(bak)&&!DeleteFile(bak))return 0;if(!Rename(configpath,bak))return 0;}if(Rename(tmp,configpath))return 1;if(exists(bak))Rename(bak,configpath);return 0;}
static void editor_show(unsigned n,const char*s){struct editor*e=&editors[n];e->count=0;DoMethod(e->list,MUIM_List_Clear);while(*s&&e->count<128){unsigned j=0;while(*s&&*s!='\n'){if(*s!='\r'&&j<255)e->rows[e->count][j++]=*s;s++;}if(*s=='\n')s++;e->rows[e->count][j]=0;DoMethod(e->list,MUIM_List_InsertSingle,(ULONG)e->rows[e->count],MUIV_List_Insert_Bottom);e->count++;}set(e->line,MUIA_String_Contents,(ULONG)"");}
static int editor_read(unsigned n,char*out){struct editor*e=&editors[n];unsigned i;size_t used=0;for(i=0;i<e->count;i++){size_t len=strlen(e->rows[i]);if(used+len+2>=DL_TEXT)return 0;memcpy(out+used,e->rows[i],len);used+=len;out[used++]='\n';}out[used]=0;return 1;}
static int dl_read_text(const char*,char*);
static int conf_parse_editor(void){char text[DL_TEXT],error[160];
 if(!editor_read(2,text)){note("dosbox.conf is too large.");return 0;}
 if(!dc_parse(text,&conf,error,sizeof(error))){note(error);return 0;}return 1;
}
static int conf_save(void){char text[DL_TEXT],file[DL_PATH],tmp[DL_PATH],bak[DL_PATH];FILE*f;int ok;
 if(!conf_parse_editor()||!editor_read(2,text))return 0;
 path(file,"dosbox.conf");path(tmp,"dosbox.conf.tmp");path(bak,"dosbox.conf.bak");
 f=fopen(tmp,"wb");if(!f){note("Cannot write dosbox.conf.");return 0;}ok=fputs(text,f)!=EOF;if(fclose(f))ok=0;
 if(!ok){DeleteFile(tmp);note("Cannot write dosbox.conf.");return 0;}
 if(exists(file)){if(exists(bak)&&!DeleteFile(bak)){note("Cannot replace dosbox.conf backup.");return 0;}if(!Rename(file,bak)){note("Cannot back up dosbox.conf.");return 0;}}
 if(!Rename(tmp,file)){if(exists(bak))Rename(bak,file);note("Cannot save dosbox.conf.");return 0;}return 1;
}
static int conf_load(void){char text[DL_TEXT],file[DL_PATH];path(file,"dosbox.conf");
 if(exists(file)){if(!dl_read_text(file,text)){note("Cannot read dosbox.conf (maximum 4095 bytes).");return 0;}}
 else if(!dc_template(text,sizeof(text)))return 0;
 editor_show(2,text);return 1;
}
static int network_store(struct dn_config*);
static void network_show(const struct dn_config*);
static int network_options(const struct dl_profile*);
static int store_selection(void){char a[DL_TEXT],c[DL_TEXT];struct dl_profile*p;if(selected<0)return 1;if(!editor_read(0,a)||!editor_read(1,c)||!dl_text_valid(a)||!dl_text_valid(c)){note("Startup file is too long.");return 0;}p=&library.p[selected];if(!network_store(&p->network))return 0;p->category=val(category,MUIA_Cycle_Active);snprintf(p->title,sizeof(p->title),"%s",(char*)val(title,MUIA_String_Contents));snprintf(p->drive,sizeof(p->drive),"%s",(char*)val(drawer,MUIA_String_Contents));snprintf(p->program,sizeof(p->program),"%s",(char*)val(program,MUIA_String_Contents));p->helper=val(helper,MUIA_Selected);p->edited=val(startup_mode[0],MUIA_Cycle_Active);p->config_mode=val(startup_mode[1],MUIA_Cycle_Active);
 snprintf(p->auto_file,sizeof(p->auto_file),"%s",(char*)val(startup_file[0],MUIA_String_Contents));snprintf(p->config_file,sizeof(p->config_file),"%s",(char*)val(startup_file[1],MUIA_String_Contents));snprintf(p->cd_path,sizeof(p->cd_path),"%s",(char*)val(cd_path,MUIA_String_Contents));p->cd_mode=val(cd_mode,MUIA_Cycle_Active);p->midi=val(midi,MUIA_Cycle_Active);snprintf(p->midi_port,sizeof(p->midi_port),"%s",(char*)val(midi_port,MUIA_String_Contents));p->memory=1u<<val(memory,MUIA_Cycle_Active);p->machine=val(machine,MUIA_Cycle_Active);p->speed=val(speed,MUIA_Cycle_Active);p->fg=val(fg,MUIA_Cycle_Active);p->bg=val(bg,MUIA_Cycle_Active);p->live=val(live,MUIA_Cycle_Active);p->display=val(display,MUIA_Cycle_Active);p->size=val(window_size,MUIA_Cycle_Active);p->fps=val(fps,MUIA_Selected);p->scanlines=val(scanlines,MUIA_Cycle_Active);p->presentation=val(presentation,MUIA_Cycle_Active);p->monochrome=val(monochrome,MUIA_Cycle_Active);p->renderer=val(renderer,MUIA_Cycle_Active);p->system=val(system_choice,MUIA_Cycle_Active);snprintf(p->image,sizeof(p->image),"%s",p->system>=2?(char*)val(disk_image,MUIA_String_Contents):"");strcpy(p->autoexec,a);strcpy(p->configsys,c);return 1;}
static void show_selection(int n){struct dl_profile*p;if(n<0||(unsigned)n>=library.count)return;selected=n;library.last=n;p=&library.p[n];SetAttrs(category,MUIA_NoNotify,TRUE,MUIA_Cycle_Active,p->category,TAG_DONE);set(title,MUIA_String_Contents,(ULONG)p->title);set(drawer,MUIA_String_Contents,(ULONG)p->drive);set(program,MUIA_String_Contents,(ULONG)p->program);set(helper,MUIA_Selected,p->helper);set(startup_mode[0],MUIA_Cycle_Active,p->edited);set(startup_mode[1],MUIA_Cycle_Active,p->config_mode);set(startup_file[0],MUIA_String_Contents,(ULONG)p->auto_file);set(startup_file[1],MUIA_String_Contents,(ULONG)p->config_file);set(cd_mode,MUIA_Cycle_Active,p->cd_mode);set(cd_path,MUIA_String_Contents,(ULONG)p->cd_path);set(midi,MUIA_Cycle_Active,p->midi);set(midi_port,MUIA_String_Contents,(ULONG)p->midi_port);set(memory,MUIA_Cycle_Active,p->memory==8?3:p->memory==4?2:p->memory==2?1:0);set(machine,MUIA_Cycle_Active,p->machine);set(speed,MUIA_Cycle_Active,p->speed);set(fg,MUIA_Cycle_Active,p->fg);set(bg,MUIA_Cycle_Active,p->bg);set(live,MUIA_Cycle_Active,p->live);set(display,MUIA_Cycle_Active,p->display);set(window_size,MUIA_Cycle_Active,p->size);set(fps,MUIA_Selected,p->fps);SetAttrs(scanlines,MUIA_NoNotify,TRUE,MUIA_Cycle_Active,p->scanlines,TAG_DONE);set(presentation,MUIA_Cycle_Active,p->presentation);set(monochrome,MUIA_Cycle_Active,p->monochrome);set(renderer,MUIA_Cycle_Active,p->renderer);display_controls();set(disk_image,MUIA_String_Contents,(ULONG)p->image);set(system_choice,MUIA_Cycle_Active,p->system);editor_show(0,p->autoexec);editor_show(1,p->configsys);network_show(&p->network);system_controls();navigation_show(val(main_pages,MUIA_Group_ActivePage));}
static void category_refresh(void){unsigned c,i;int row;
 for(c=0;c<DL_CATEGORIES;c++){
  SetAttrs(category_lists[c],MUIA_NoNotify,TRUE,MUIA_List_Active,MUIV_List_Active_Off,TAG_DONE);
  DoMethod(category_lists[c],MUIM_List_Clear);row=0;
  for(i=0;i<library.count;i++)if(library.p[i].category==c){
   DoMethod(category_lists[c],MUIM_List_InsertSingle,(ULONG)library.p[i].title,MUIV_List_Insert_Bottom);
   if((int)i==selected)SetAttrs(category_lists[c],MUIA_NoNotify,TRUE,MUIA_List_Active,row,TAG_DONE);
   row++;
  }
  set(category_run[c],MUIA_Disabled,selected<0||library.p[selected].category!=c);
  set(category_edit[c],MUIA_Disabled,selected<0||library.p[selected].category!=c);
 }
}
static void refresh(void){unsigned i;busy=1;DoMethod(list,MUIM_List_Clear);for(i=0;i<library.count;i++)DoMethod(list,MUIM_List_InsertSingle,(ULONG)library.p[i].title,MUIV_List_Insert_Bottom);if(library.count){if(library.last>=library.count)library.last=0;SetAttrs(list,MUIA_NoNotify,TRUE,MUIA_List_Active,library.last,TAG_DONE);show_selection(library.last);}else selected=-1;category_refresh();navigation_show(val(main_pages,MUIA_Group_ActivePage));busy=0;}
static int select_profile(int n){if(n<0||(unsigned)n>=library.count)return 0;if(n==selected)return 1;if(!store_selection())return 0;show_selection(n);SetAttrs(list,MUIA_NoNotify,TRUE,MUIA_List_Active,n,TAG_DONE);category_refresh();return 1;}
static Object *category_page(unsigned c){return VGroup,UI_PAGE,
 Child,program_views[c+1]=program_list_new(&category_lists[c]),
 Child,HGroup,Child,category_run[c]=ui_button("\33bLaunch"),Child,category_edit[c]=ui_button("Edit"),End,End;}
static int browse(char*out,int onlydrawer,int saving,const char*caption){struct FileRequester*r;int ok=0;set(app,MUIA_Application_Sleep,TRUE);r=AllocAslRequestTags(ASL_FileRequest,ASLFR_TitleText,(ULONG)caption,ASLFR_InitialDrawer,(ULONG)(selected>=0?library.p[selected].drive:appdir),ASLFR_DrawersOnly,onlydrawer,ASLFR_DoSaveMode,saving,TAG_DONE);if(r){if(AslRequestTags(r,TAG_DONE)&&strlen(r->fr_Drawer)<DL_PATH){strcpy(out,r->fr_Drawer);ok=onlydrawer||AddPart(out,r->fr_File,DL_PATH);}FreeAslRequest(r);}set(app,MUIA_Application_Sleep,FALSE);return ok;}
static void add(void){char p[DL_PATH];struct dl_profile*x;if(library.count==DL_MAX){note("Program list is full.");return;}if(!browse(p,0,0,"Add DOS program"))return;if(!store_selection())return;x=&library.p[library.count];dl_profile_default(x);snprintf(x->program,sizeof(x->program),"%s",FilePart(p));snprintf(x->title,sizeof(x->title),"%s",FilePart(p));*PathPart(p)=0;strcpy(x->drive,p);library.last=library.count++;refresh();if(!save())note("Could not save program list.");}
static int copy_helper(const char*drive){char source[DL_PATH],dest[DL_PATH];FILE*a,*b;unsigned char data[1024];size_t n;int ok=1;path(source,"LOADFIX.COM");strcpy(dest,drive);if(!AddPart(dest,"LOADFIX.COM",sizeof(dest)))return 0;if(exists(dest))return 1;a=fopen(source,"rb");if(!a)return 0;b=fopen(dest,"wb");if(!b){fclose(a);return 0;}while((n=fread(data,1,sizeof(data),a))>0)if(fwrite(data,1,n,b)!=n){ok=0;break;}if(ferror(a))ok=0;fclose(a);if(fclose(b))ok=0;if(!ok)DeleteFile(dest);return ok;}
#include "disk_ui.h"
static int hdd_available(void){char image[DL_PATH];BPTR lock;
 path(image,"Disks/DOS622.IMG");lock=Lock(image,EXCLUSIVE_LOCK);
 if(!lock){note("MS-DOS disk unavailable; eject it from Workbench first.");return 0;}
 UnLock(lock);return 1;
}
#include "startup_ui.h"
static unsigned network_requested,network_running,network_busy;
static void network_poll(void);
static void network_live_event(unsigned);
static int network_live_begin(void);
static void network_live_end(void);
static int loading_splash(void);
#include "memory_ui.h"
static int developer_options(void){char frame[24];unsigned profile,handoff,poll;
 active_runtime=NULL;if(!conf_save())return 0;
 active_trace=dc_get(&conf,DC_TRACE,val(dev_trace,MUIA_Selected));active_break=dc_get(&conf,DC_BREAK,val(dev_break,MUIA_Cycle_Active));
 profile=dc_get(&conf,DC_PROFILE,val(dev_profile,MUIA_Selected));handoff=dc_get(&conf,DC_HANDOFF,val(dev_handoff,MUIA_Cycle_Active));poll=dc_get(&conf,DC_POLL,val(dev_poll,MUIA_Cycle_Active));
 if(conf.set[DC_FRAME])snprintf(frame,sizeof(frame),"%u",conf.value[DC_FRAME]);else snprintf(frame,sizeof(frame),"%s",(char*)val(dev_frame,MUIA_String_Contents));
 if(!dl_dev_tools(dev_arguments,sizeof(dev_arguments),active_trace,active_break,frame,profile,handoff,poll)){note("Breakpoint frame must be 1 to 10000000.");return 0;}return 1;
}
static int conf_profile(const struct dl_profile*p){char error[180];
 if(!dc_apply(&conf,p,&effective_profile,error,sizeof(error))){note(error);return 0;}
 active_runtime=dc_runtime(&conf,&effective_profile,active_trace,active_break,val(mem_enabled,MUIA_Selected),error,sizeof(error));
 if(!active_runtime){note(error);return 0;}return network_options(&effective_profile);
}
static const char *developer_runtime(const char *normal){return active_runtime?active_runtime:normal;}
static void conf_log(BPTR log,const struct dl_session*s){
 static const char*cores[]={"auto","normal","dynamic"};
 FPrintf(log,"CONFIG core=%s runtime=%s cycles=%lu memory=%lu\n",cores[dc_get(&conf,DC_CORE,0)],active_runtime?active_runtime:"default",s->cycles,s->memory);Flush(log);
}
static void open_document(const char *name,const char *missing){
 char p[DL_PATH],q[DL_PATH*2+3],cmd[DL_PATH*2+64];const char *viewer;
 path(p,name);
 if(!exists(p)){note(missing);return;}
 viewer=exists("SYS:Utilities/MultiView")?"SYS:Utilities/MultiView":
        exists("C:MultiView")?"C:MultiView":
        exists("SYS:System/MultiView")?"SYS:System/MultiView":NULL;
 if(!viewer){note("MultiView is required to open this document.");return;}
 if(dl_quote(q,sizeof(q),p)){snprintf(cmd,sizeof(cmd),"%s %s",viewer,q);if(SystemTags(cmd,SYS_Asynch,TRUE,NP_StackSize,16384,TAG_DONE)<0)note("Could not open the document viewer.");}
}
static void developer_log(void){open_document(last_log,"No completed log yet.");}
#include "network_ui.h"
static void launch_msdos(const struct dl_profile*p,unsigned mode,const char*doscommand){
 struct dl_session s;char session[DL_PATH],player[DL_PATH],qp[DL_PATH*2+3],qs[DL_PATH*2+3],cmd[DL_PATH*6+1200];BPTR log;LONG rc=20;int changed=0;
 if(mode==4){note("Use Run command with the batch file's DOS path.");return;}
 {char runtime[DL_PATH];path(runtime,developer_runtime(dl_profile_player(p)));if(!exists(runtime)){note("Selected DOSBox runtime is missing.");return;}}
 if(!hdd_available())return;
 dl_defaults(&s);path(s.drive,"Disks");s.system=1;s.terminal=1;s.live=1;
 s.memory=p->memory;s.machine=p->machine;s.cycles=dc_get(&conf,DC_CYCLES,p->speed==0?1000:p->speed==1?3000:6000);
 s.fg=p->fg;s.bg=p->bg;s.display=p->display;s.size=p->size;s.fps=p->fps;s.midi=p->midi;strcpy(s.midi_port,p->midi_port);s.cd_mode=p->cd_mode;strcpy(s.cd_path,p->cd_path);
 if(!startup_prepare(p,&changed))return;
 if(mode==0||mode==2)snprintf(s.command,sizeof(s.command),"%s%s",mode==0&&p->helper?"LOADFIX ":"",doscommand);
 path(session,"Launch.session");path(player,developer_runtime(dl_profile_player(p)));
 if(!save()||!dl_session_write(session,&s)||!dl_quote(qp,sizeof(qp),player)||!dl_quote(qs,sizeof(qs),session)){note("Could not save launch settings.");goto finish;}
 snprintf(cmd,sizeof(cmd),"%s --session %s%s%s%s%s%s",qp,qs,dl_scanline_args(p),dl_presentation_args(p->presentation),dl_mono_args(p->monochrome),dev_arguments,net_arguments);last_log="LastRun.log";path(player,last_log);log=Open(player,MODE_NEWFILE);
 if(!log){note("Cannot open session log.");goto finish;}
 conf_log(log,&s);rc=player_run(cmd,log);Close(log);DeleteFile(session);set(win,MUIA_Window_Open,TRUE);note(rc==0?"Session finished.":"MS-DOS failed; see LastRun.log.");
finish:if(changed&&!startup_finish())note("Startup restore failed; originals remain in the DOS disk backups.");
}
static void launch_image(const struct dl_profile*p,unsigned mode,const char*command){
 struct dl_session s;char file[DL_PATH],session[DL_PATH],player[DL_PATH],qp[DL_PATH*2+3],qs[DL_PATH*2+3],cmd[DL_PATH*6+1200];BPTR lock,log;LONG rc;
 if(mode==3||mode==4){note("Disk startup files run from the image.");return;}
 if(!dl_join(file,sizeof(file),p->drive,p->image))return;lock=Lock(file,EXCLUSIVE_LOCK);
 if(!lock){note("Disk image unavailable; eject it from Workbench first.");return;}UnLock(lock);
 path(player,developer_runtime("ZZDosBox-Windows"));if(!exists(player)){note("ZZDosBox-Windows runtime is missing.");return;}
 dl_defaults(&s);strcpy(s.drive,p->drive);strcpy(s.image,p->image);s.system=2;s.terminal=s.live=1;
 s.memory=p->memory;s.machine=p->machine;s.cycles=dc_get(&conf,DC_CYCLES,p->speed==0?1000:p->speed==1?3000:6000);
 s.fg=p->fg;s.bg=p->bg;s.display=p->display;s.size=p->size;s.fps=p->fps;s.midi=p->midi;strcpy(s.midi_port,p->midi_port);
 s.cd_mode=p->cd_mode;strcpy(s.cd_path,p->cd_path);if(mode==0||mode==2)snprintf(s.command,sizeof(s.command),"%s%s",mode==0&&p->helper?"LOADFIX ":"",command);
 path(session,"Launch.session");if(!save()||!dl_session_write(session,&s)||!dl_quote(qp,sizeof(qp),player)||!dl_quote(qs,sizeof(qs),session)){note("Cannot save image session.");return;}
 snprintf(cmd,sizeof(cmd),"%s --session %s%s%s",qp,qs,dl_mono_args(p->monochrome),dev_arguments);last_log="LastImage.log";path(file,last_log);log=Open(file,MODE_NEWFILE);if(!log){DeleteFile(session);note("Cannot open session log.");return;}
 conf_log(log,&s);rc=player_run(cmd,log);Close(log);DeleteFile(session);set(win,MUIA_Window_Open,TRUE);note(rc==0?"Session finished.":"Image session failed; see LastImage.log.");
}
static void launch_embedded(struct dl_profile*p,unsigned mode,const char*script){
 struct dl_session s;char boot[DL_PATH],bootname[16],session[DL_PATH],player[DL_PATH],qp[DL_PATH*2+3],qs[DL_PATH*2+3],cmd[DL_PATH*6+1200];FILE*f;unsigned i;LONG rc;BPTR log;const char*doscommand=mode==0?p->program:mode==2?(char*)val(command,MUIA_String_Contents):mode==4?script:"";int withhelper=mode==0&&p->helper;
 {char runtime[DL_PATH];path(runtime,developer_runtime(dl_profile_player(p)));if(!exists(runtime)){note("Selected DOSBox runtime is missing.");return;}}
 if(p->network.enabled&&!network_prepare(p->drive,&p->network))return;
 if(!welcome_support(p->drive)){note("Cannot install ZZHELLO2.COM; check for a conflicting file.");return;}
 if(p->edited==2&&!dl_read_text(p->auto_file,p->autoexec)){note("Cannot read selected AUTOEXEC.BAT.");return;}
 if(withhelper&&(strchr(doscommand,' ')||strchr(doscommand,'\t'))){note("LOADFIX currently accepts one program filename.");return;}
 if(withhelper&&!copy_helper(p->drive)){note("Could not install LOADFIX.COM in the program drawer.");return;}
 for(i=0;i<1000;i++){snprintf(bootname,sizeof(bootname),"ZZL%04u.BAT",i);strcpy(boot,p->drive);if(!AddPart(boot,bootname,sizeof(boot)))return;if(!exists(boot))break;}
 if(i==1000){note("No free startup filename.");return;}
 f=fopen(boot,"wb");if(!f){note("Program drawer must be writable.");return;}
 {int ok=fputs("@echo off\r\nZZHELLO2.COM\r\n",f)!=EOF;
 if(ok&&p->network.enabled)ok=dn_start(f,&p->network);
 if(ok)ok=dl_boot(f,p,doscommand,1,withhelper);
 if(ok&&p->network.enabled)ok=fputs("goto ZZNETEND\r\n:ZZNETFAIL\r\necho Network setup failed\r\n:ZZNETEND\r\n",f)!=EOF;
 if(fclose(f))ok=0;if(!ok){DeleteFile(boot);note("Invalid or oversized startup command.");return;}}
 dl_defaults(&s);strcpy(s.drive,p->drive);strcpy(s.command,bootname);s.memory=p->memory;s.machine=p->machine;s.cycles=dc_get(&conf,DC_CYCLES,p->speed==0?1000:p->speed==1?3000:6000);s.fg=p->fg;s.bg=p->bg;s.live=p->live;s.display=p->display;s.size=p->size;s.fps=p->fps;s.midi=p->midi;strcpy(s.midi_port,p->midi_port);s.cd_mode=p->cd_mode;strcpy(s.cd_path,p->cd_path);s.terminal=mode!=0||!*doscommand;
 path(session,"Launch.session");path(player,developer_runtime(dl_profile_player(p)));if(!save()||!dl_session_write(session,&s)||!dl_quote(qp,sizeof(qp),player)||!dl_quote(qs,sizeof(qs),session)){DeleteFile(boot);note("Could not save launch settings.");return;}
 snprintf(cmd,sizeof(cmd),"%s --session %s%s%s%s%s%s",qp,qs,dl_scanline_args(p),dl_presentation_args(p->presentation),dl_mono_args(p->monochrome),dev_arguments,net_arguments);last_log="LastRun.log";path(player,last_log);log=Open(player,MODE_NEWFILE);if(!log){DeleteFile(boot);note("Cannot open session log.");return;}
 conf_log(log,&s);rc=player_run(cmd,log);Close(log);DeleteFile(boot);DeleteFile(session);set(win,MUIA_Window_Open,TRUE);note(rc==0?"Session finished.":"Session failed; see LastRun.log.");
}
static void launch(unsigned mode,const char*script){struct dl_profile*p;const char*doscommand;
 if(!developer_options())return;
 if(selected<0){note("Add a program first.");return;}if(!store_selection())return;p=&library.p[selected];if(!dl_profile_valid(p)){note("Check settings; text and background must differ.");return;}if(!conf_profile(p))return;p=&effective_profile;
 if(FindPort("Sixies.ARM.Debug.Owner")||FindPort("ZZDOS_DISK_CREATE")){note("Another ARM application or disk creator is running.");return;}
 doscommand=mode==0?p->program:mode==2?(char*)val(command,MUIA_String_Contents):mode==4?script:"";
 if(p->system==2){launch_image(p,mode,doscommand);return;}
 if(p->system==1){launch_msdos(p,mode,doscommand);return;}
 if(p->system==3){char image[DL_PATH];static struct dl_profile mounted;
  if(!dl_join(image,sizeof(image),p->drive,p->image)||!disk_image_control("mount",image)){note("Cannot mount the DOSBox C: image. Close its files first.");return;}
  mounted=*p;mounted.system=0;*mounted.image=0;strcpy(mounted.drive,"ZZIMAGE:");
  if(!mounted.edited){if(!dl_read_text("ZZIMAGE:AUTOEXEC.BAT",mounted.autoexec)){disk_image_control("eject",image);note("Cannot read the image AUTOEXEC.BAT.");return;}mounted.edited=1;}
  launch_embedded(&mounted,mode,script);
  if(!disk_image_control("eject",image))note("Session ended; eject the image from Disks before moving it.");return;
 }
 launch_embedded(p,mode,script);
}

static Object *conf_page(void){Object *add,*replace,*remove,*up,*down,*load,*export,*reset,*check;struct editor*e=&editors[2];Object*o=VGroup,
 Child,ListviewObject,MUIA_Listview_List,e->list=ListObject,MUIA_Font,ui_code_font(),InputListFrame,MUIA_List_ConstructHook,MUIV_List_ConstructHook_String,MUIA_List_DestructHook,MUIV_List_DestructHook_String,End,End,
 Child,e->line=str(256),Child,HGroup,Child,add=ui_button("Add"),Child,replace=ui_button("Replace"),Child,remove=ui_button("Delete"),Child,up=ui_button("Up"),Child,down=ui_button("Down"),End,
 Child,HGroup,Child,load=ui_button("Open..."),Child,export=ui_button("Save as..."),Child,reset=ui_button("Defaults"),Child,check=ui_button("Validate"),End,End;
 set(add,MUIA_UserData,ID_EDITOR+33);set(replace,MUIA_UserData,ID_EDITOR+34);set(remove,MUIA_UserData,ID_EDITOR+35);set(up,MUIA_UserData,ID_EDITOR+36);set(down,MUIA_UserData,ID_EDITOR+37);set(load,MUIA_UserData,ID_EDITOR+38);set(export,MUIA_UserData,ID_EDITOR+39);set(reset,MUIA_UserData,ID_EDITOR+40);set(check,MUIA_UserData,ID_CONF_VALIDATE);return o;
}
static Object *editor_page(unsigned n){Object *add,*replace,*remove,*up,*down,*load,*export,*reset,*view;struct editor*e=&editors[n];Object*o=VGroup,
 Child,ColGroup(2),Child,fieldlabel("Use"),Child,startup_mode[n]=cycle(startup_choices),Child,fieldlabel("File"),Child,startup_file[n]=str(DL_PATH),End,
 Child,view=ListviewObject,MUIA_Listview_List,e->list=ListObject,MUIA_Font,ui_code_font(),InputListFrame,MUIA_List_ConstructHook,MUIV_List_ConstructHook_String,MUIA_List_DestructHook,MUIV_List_DestructHook_String,End,End,
 Child,e->line=str(256),Child,HGroup,Child,add=ui_button("Add"),Child,replace=ui_button("Replace"),Child,remove=ui_button("Delete"),Child,up=ui_button("Up"),Child,down=ui_button("Down"),End,
 Child,HGroup,Child,load=ui_button("Choose file..."),Child,export=ui_button("Save as..."),Child,reset=ui_button("Default"),End,End;
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
 if(action==6){if(!browse(p,0,0,n==2?"Open dosbox.conf":n?"Choose CONFIG.SYS":"Choose AUTOEXEC.BAT"))return;if(!dl_read_text(p,text)){note("Cannot read a plain ASCII startup file (maximum 4095 bytes).");return;}editor_show(n,text);if(n==2)return;set(startup_file[n],MUIA_String_Contents,(ULONG)p);set(startup_mode[n],MUIA_Cycle_Active,2);return;}
 if(action==7){int ok;if(!editor_read(n,text)||!browse(p,0,1,n==2?"Export dosbox.conf":n?"Export CONFIG.SYS":"Export AUTOEXEC.BAT"))return;if(exists(p)&&!MUI_Request(app,win,0,"Replace file?","_Replace|_Cancel","%s",p))return;f=fopen(p,"wb");if(!f){note("Cannot write file.");return;}ok=dl_dos_text(f,text);if(fclose(f))ok=0;note(ok?"File exported.":"Write failed.");return;}
 if(action==8){if(n==2){if(dc_template(text,sizeof(text)))editor_show(2,text);return;}if(startup_read_default(n,text)){editor_show(n,text);set(startup_mode[n],MUIA_Cycle_Active,0);}else note("Cannot read default startup file.");return;}
 if(n<2&&action>=1&&action<=5)set(startup_mode[n],MUIA_Cycle_Active,1);
 DoMethod(e->list,MUIM_List_Clear);for(i=0;i<e->count;i++)DoMethod(e->list,MUIM_List_InsertSingle,(ULONG)e->rows[i],MUIV_List_Insert_Bottom);if(at>=0)set(e->list,MUIA_List_Active,at);
}
/* Find action buttons through group children, without allocating hook state. */
static void wire(Object*o){struct List*children=NULL;Object*child;APTR state;ULONG id=val(o,MUIA_UserData);if(id)notify(o,MUIA_Pressed,FALSE,id);get(o,MUIA_Group_ChildList,&children);if(children){state=children->lh_Head;while((child=NextObject(&state)))wire(child);}}
static const char *bootlabels[]={"MS-DOS hard disk","DOS boot disk","Setup disk 1","Setup disk 2","Setup disk 3","Supplemental tools",NULL};
static const char *bootfiles[]={"DOS622.IMG","DOSBOOT.IMG","DISK1.IMG","DISK2.IMG","DISK3.IMG","SUPPDISK.IMG"};
static void boot_disk(void){
 struct dl_session s;struct dl_profile base;const struct dl_profile*p;char player[DL_PATH],session[DL_PATH],qp[DL_PATH*2+3],qs[DL_PATH*2+3],cmd[DL_PATH*6+1200];unsigned n=val(bootdisk,MUIA_Cycle_Active);BPTR log;LONG rc;
 if(n>=6||!store_selection()||!developer_options())return;
 if(FindPort("Sixies.ARM.Debug.Owner")||FindPort("ZZDOS_DISK_CREATE")){note("Another ARM application or disk creator is running.");return;}
 if(selected>=0)base=library.p[selected];else{dl_profile_default(&base);strcpy(base.title,"Boot disk");path(base.drive,"Disks");}
 base.system=1;*base.image=0;if(!conf_profile(&base))return;p=&effective_profile;
 if(!n){launch_msdos(p,1,"");return;}
 if(conf.set[DC_AUTO_MODE]||conf.set[DC_AUTO_FILE]||conf.set[DC_CONFIG_MODE]||conf.set[DC_CONFIG_FILE]||(conf.set[DC_LOADFIX]&&p->helper)){note("Floppy boots use startup files stored on the floppy.");return;}
 path(player,developer_runtime(dl_profile_player(p)));if(!exists(player)){note("Selected DOSBox runtime is missing.");return;}
 dl_defaults(&s);path(s.drive,"Disks");s.memory=p->memory;s.machine=p->machine;s.cycles=dc_get(&conf,DC_CYCLES,p->speed==0?1000:p->speed==1?3000:6000);s.display=p->display;s.size=p->size;s.fps=p->fps;s.fg=p->fg;s.bg=p->bg;s.midi=p->midi;strcpy(s.midi_port,p->midi_port);s.cd_mode=p->cd_mode;strcpy(s.cd_path,p->cd_path);
 path(session,"Launch.session");if(!dl_session_write(session,&s)||!dl_quote(qp,sizeof(qp),player)||!dl_quote(qs,sizeof(qs),session)){note("Cannot save boot session.");return;}
 snprintf(cmd,sizeof(cmd),"%s --session %s --boot %s%s%s%s%s",qp,qs,bootfiles[n],dl_scanline_args(p),dl_presentation_args(p->presentation),dl_mono_args(p->monochrome),dev_arguments);last_log="LastBoot.log";path(player,last_log);log=Open(player,MODE_NEWFILE);if(!log){DeleteFile(session);note("Cannot open boot log.");return;}
 conf_log(log,&s);rc=player_run(cmd,log);Close(log);DeleteFile(session);set(win,MUIA_Window_Open,TRUE);note(rc==0?"Session finished.":"Boot failed; see LastBoot.log.");
}
#include "audio_detect.h"
#include "compatibility_ui.h"
static int build(void){Object *add,*remove,*save_button,*run,*term,*execute,*auto_run,*script,*mount_disks,*eject_disks,*boot_button,*cd_check,*audio_check,*github,*dev_log,*root;
 style_open();
 program_list_class=MUI_CreateCustomClass(NULL,MUIC_Listview,NULL,sizeof(struct program_list_data),(APTR)program_list_dispatch);if(!program_list_class)return 0;
 app=ApplicationObject,MUIA_Application_Title,(ULONG)"ZZDosBox",MUIA_Application_Base,(ULONG)"ZZDOSBOX",MUIA_Application_Version,(ULONG)"$VER: ZZDosBox Launcher " ZZ_LAUNCHER_VERSION " (" ZZ_LAUNCHER_DATE ")",
 SubWindow,win=WindowObject,MUIA_Window_Title,(ULONG)"ZZDosBox Launcher",MUIA_Window_ID,0x5a444c31,MUIA_Window_Width,940,MUIA_Window_Height,500,
 WindowContents,root=VGroup,MUIA_Font,ui_font(),MUIA_Group_Spacing,8,InnerSpacing(8,8),
 Child,HGroup,
 Child,VGroup,MUIA_HorizWeight,0,MUIA_Group_Spacing,8,
  Child,HGroup,MUIA_VertWeight,0,InnerSpacing(0,4),Child,HSpace(0),
   Child,BodychunkObject,MUIA_FixWidth,80,MUIA_FixHeight,80,
    MUIA_Bitmap_Width,80,MUIA_Bitmap_Height,80,MUIA_Bodychunk_Depth,2,
    MUIA_Bodychunk_Body,(ULONG)sidebar_body,MUIA_Bodychunk_Compression,0,
    MUIA_Bodychunk_Masking,0,MUIA_Bitmap_SourceColors,(ULONG)sidebar_colours,
    MUIA_Bitmap_Transparent,0,End,Child,HSpace(0),End,
  Child,navigation_view=ListviewObject,MUIA_CycleChain,1,MUIA_FixWidthTxt,(ULONG)"    Compatibility      ",
   MUIA_Listview_List,navigation=ListObject,MUIA_Font,ui_font(),InputListFrame,MUIA_List_MinLineHeight,30,
    MUIA_List_Format,(ULONG)"W=0 D=8,MAW=100",MUIA_List_DisplayHook,(ULONG)&navigation_hook,End,End,
  Child,save_button=ui_button("\33bSave settings"),End,
 Child,VGroup,
  Child,HGroup,TextFrame,MUIA_Background,MUII_ButtonBack,InnerSpacing(10,8),MUIA_VertWeight,0,
   Child,page_icon=ImageObject,MUIA_Image_Spec,MUII_Drawer,MUIA_FixWidth,24,MUIA_FixHeight,24,End,
   Child,page_heading=TextObject,MUIA_Font,ui_title_font(),MUIA_FixWidthTxt,(ULONG)"CPU & memory  ",MUIA_HorizWeight,0,MUIA_Text_PreParse,(ULONG)"\33b",MUIA_Text_Contents,(ULONG)"Load program",End,
   Child,page_context=TextObject,MUIA_Font,ui_font(),MUIA_Text_PreParse,(ULONG)"\33r",MUIA_Text_SetMin,FALSE,MUIA_Text_Contents,(ULONG)"",End,End,
  Child,main_pages=PageGroup,GroupFrame,MUIA_Background,MUII_GroupBack,InnerSpacing(10,10),
 Child,program_tabs=RegisterGroup(library_tabs),MUIA_Font,ui_font(),
 Child,VGroup,UI_PAGE,
  Child,program_views[0]=program_list_new(&list),
  Child,HGroup,Child,add=ui_button("Add program..."),Child,remove=ui_button("Remove"),End,
  Child,ColGroup(2),UI_PANEL("Program"),Child,fieldlabel("Name"),Child,title=str(64),Child,fieldlabel("Category"),Child,category=cycle(categories),Child,fieldlabel("DOSBox C:"),Child,HGroup,Child,drawer=str(DL_PATH),Child,browse_button=ui_button("Browse..."),End,Child,fieldlabel("Program"),Child,program=str(128),End,
  Child,HGroup,Child,helper=CheckMark(FALSE),Child,label("Low-memory compatibility (LOADFIX)"),End,
  Child,HGroup,Child,run=ui_button("\33bLaunch"),Child,open_drawer=ui_button("Open C: drawer"),End,End,
 Child,category_page(DL_GAMES),Child,category_page(DL_APPLICATIONS),Child,category_page(DL_OS),End,
 Child,terminal_reg=RegisterGroup(terminal_tabs),MUIA_Font,ui_font(),
 Child,VGroup,UI_PAGE,Child,ColGroup(2),UI_PANEL("DOS session"),Child,fieldlabel("DOS"),Child,system_choice=cycle(systems),Child,fieldlabel("Disk image"),Child,disk_image=str(13),Child,fieldlabel("Text"),Child,fg=cycle(colours),Child,fieldlabel("Background"),Child,bg=cycle(colours),End,Child,VSpace(0),
  Child,HGroup,Child,command=str(128),Child,execute=ui_button("Run command"),End,
  Child,HGroup,Child,term=ui_button("Open terminal"),Child,auto_run=ui_button("Run AUTOEXEC"),Child,script=ui_button("Run script..."),End,
  End,
 Child,editor_pages[0]=editor_page(0),
 Child,VGroup,Child,label("CONFIG.SYS applies to MS-DOS 6.22."),Child,editor_pages[1]=editor_page(1),End,
 Child,VGroup,Child,ColGroup(2),Child,fieldlabel("Source"),Child,cd_mode=cycle(cd_choices),Child,fieldlabel("Path / device"),Child,HGroup,Child,cd_path=str(DL_PATH),Child,cd_browse=ui_button("Browse..."),End,End,
  Child,label("D: - read-only data CD"),Child,cd_check=ui_button("Check CD-ROM"),Child,VSpace(0),End,
 End,
 Child,VGroup,
  Child,ColGroup(2),UI_PANEL("Emulated PC"),Child,fieldlabel("Guest RAM"),Child,memory=cycle(memories),Child,fieldlabel("CPU cycles"),Child,speed=cycle(speeds),Child,fieldlabel("Session"),Child,live=cycle(lengths),End,Child,VSpace(0),End,
 Child,VGroup,
  Child,HGroup,Child,VGroup,MUIA_HorizWeight,60,
  Child,ColGroup(2),UI_PANEL("Screen"),Child,displaylabel("Display"),Child,display=cycle(displays),Child,displaylabel("Window size"),Child,window_size=cycle(sizes),Child,displaylabel("Video adapter"),Child,machine=cycle(machines),End,
  Child,ColGroup(2),UI_PANEL("Rendering"),Child,displaylabel("Renderer"),Child,renderer=cycle(renderer_choices),Child,displaylabel("Presentation"),Child,presentation=cycle(presentation_choices),Child,displaylabel("Monochrome colour"),Child,monochrome=cycle(mono_choices),Child,displaylabel("Scanlines"),Child,scanlines=cycle(scanline_choices),Child,displaylabel("Show FPS"),Child,HGroup,Child,fps=CheckMark(FALSE),Child,HSpace(0),End,End,Child,VSpace(0),End,
  Child,VGroup,UI_PANEL("Settings guide"),MUIA_HorizWeight,40,
   Child,scanline_help=label("Scanlines\nRequires fullscreen with Show FPS off."),
   Child,label("Display\nWindow size sets the starting size.\nWorkbench windows remain resizable."),
   Child,label("Video adapter / monochrome\nChoose the graphics expected by the app.\nColour tints apply to Hercules only."),
   Child,label("Renderer\nDirect: standard display path.\nOpenGL: experimental software rendering."),
   Child,label("Presentation / FPS\nAdaptive skips display updates if needed.\nFPS shows the displayed frame rate.\nScanlines and Show FPS cannot be\nused together."),
   Child,VSpace(0),End,End,Child,VSpace(0),End,
 Child,VGroup,
  Child,ColGroup(2),UI_PANEL("MIDI output"),Child,fieldlabel("MIDI"),Child,midi=cycle(midi_choices),Child,fieldlabel("CAMD output"),Child,midi_port=str(64),End,
  Child,VGroup,UI_PANEL("Drivers"),Child,audio_camd=label("CAMD: unchecked"),Child,audio_ahi=label("AHI: unchecked"),Child,audio_ax=label("ZZ9000AX: unchecked"),Child,audio_check=ui_button("Check audio drivers"),End,Child,VSpace(0),End,
 Child,network_page(),
 Child,VGroup,
  Child,HGroup,UI_PANEL("Boot image"),Child,bootdisk=cycle(bootlabels),Child,boot_button=ui_button("Boot"),End,
  Child,disk_image_page(),
  Child,HGroup,UI_PANEL("Floppy disks"),Child,mount_disks=ui_button("Mount floppies"),Child,eject_disks=ui_button("Eject floppies"),End,
  Child,VSpace(0),End,
 Child,VGroup,
  Child,label("Development only: diagnostics affect performance."),
  Child,HGroup,Child,mem_enabled=CheckMark(FALSE),Child,label("Memory tools (experimental)"),End,
  Child,RegisterGroup(developer_tabs),MUIA_Font,ui_font(),Child,VGroup,UI_PAGE,
  Child,ColGroup(2),
  Child,fieldlabel("Trace logging"),Child,dev_trace=CheckMark(FALSE),
  Child,fieldlabel("CPU / video profiling"),Child,dev_profile=CheckMark(FALSE),
  Child,fieldlabel("Frame breakpoint"),Child,dev_break=cycle(break_choices),
  Child,fieldlabel("Frame number"),Child,dev_frame=str(9),
  Child,fieldlabel("Display handoff"),Child,dev_handoff=cycle(handoff_choices),
  Child,fieldlabel("Host polling"),Child,dev_poll=cycle(poll_choices),End,
  Child,dev_log=ui_button("Open log"),Child,VSpace(0),End,
  Child,memory_page(),Child,conf_page(),End,End,
 Child,compat_page(),
 Child,VGroup,UI_PAGE,Child,label("DosBox ZZ9000 project"),Child,label(ZZ_LAUNCHER_VERSION " - AmigaOS - 32-bit"),Child,github=ui_button("GitHub"),Child,label("DOSBox / libretro contributors"),Child,label("XACP / Xani - ZZ9000 Cortex-A9"),Child,label("MUI - Stefan Stuntz"),Child,label("TinyGL - Fabrice Bellard and contributors"),Child,label("Development: Codex assisted, Amiga-Bridge MCP"),
  Child,VGroup,UI_PANEL("Changelog"),Child,ListviewObject,MUIA_CycleChain,1,MUIA_Listview_Input,FALSE,
   MUIA_Listview_List,FloattextObject,ReadListFrame,MUIA_Font,ui_font(),
    MUIA_Floattext_Text,(ULONG)dl_changelog_text,End,End,End,End,
 End,End,End,Child,status=TextObject,TextFrame,MUIA_Text_SetMin,FALSE,MUIA_Text_Contents,(ULONG)"",InnerSpacing(6,4),End,End,End,End;
 if(!app)return 0;
 {unsigned n;for(n=0;n<PAGE_COUNT;n++){snprintf(nav_entries[n].icon,sizeof(nav_entries[n].icon),"\33I[6:%lu]",(unsigned long)section_icons[n]);nav_entries[n].caption=sections[n];DoMethod(navigation,MUIM_List_InsertSingle,(ULONG)&nav_entries[n],MUIV_List_Insert_Bottom);}}
 set(navigation_view,MUIA_ShortHelp,(ULONG)"Choose a section. Use Up/Down when this list has keyboard focus.");
 notify(navigation,MUIA_List_Active,MUIV_EveryTime,ID_NAVIGATE);
 notify(program_tabs,MUIA_Group_ActivePage,MUIV_EveryTime,ID_PROGRAM_TAB);
 memory_ui_init();compat_init();network_init();
 {unsigned c;for(c=0;c<DL_CATEGORIES;c++){notify(category_lists[c],MUIA_List_Active,MUIV_EveryTime,ID_CAT_SELECT+c);notify(category_run[c],MUIA_Pressed,FALSE,ID_CAT_RUN+c);notify(category_edit[c],MUIA_Pressed,FALSE,ID_CAT_EDIT+c);}}
 notify(category,MUIA_Cycle_Active,MUIV_EveryTime,ID_CATEGORY);
 set(dev_frame,MUIA_String_Contents,(ULONG)"1");set(dev_frame,MUIA_Disabled,TRUE);
 set(dev_break,MUIA_ShortHelp,(ULONG)"Cooperative presentation checkpoints, not instruction breakpoints. Continue, Next frame or Stop when paused.");
 set(dev_trace,MUIA_ShortHelp,(ULONG)"Requires the matching Trace runtime. Developer options reset when the launcher closes.");
 set(dev_frame,MUIA_ShortHelp,(ULONG)"Presented frames, counted from 1. Adaptive skipped frames are excluded.");
 notify(dev_break,MUIA_Cycle_Active,MUIV_EveryTime,ID_DEV_CHANGE);
 notify(dev_log,MUIA_Pressed,FALSE,ID_DEV_LOG);
 set(disk_image,MUIA_ShortHelp,(ULONG)"Image filename inside the selected C: drawer. Edit its startup files from DOS.");
 set(renderer,MUIA_ShortHelp,(ULONG)"A9 software OpenGL subset. No GLSL shaders.");

 notify(win,MUIA_Window_CloseRequest,TRUE,MUIV_Application_ReturnID_Quit);notify(list,MUIA_List_Active,MUIV_EveryTime,ID_SELECT);
 notify(add,MUIA_Pressed,FALSE,ID_ADD);notify(remove,MUIA_Pressed,FALSE,ID_REMOVE);notify(run,MUIA_Pressed,FALSE,ID_RUN);notify(save_button,MUIA_Pressed,FALSE,ID_SAVE);notify(term,MUIA_Pressed,FALSE,ID_TERM);notify(execute,MUIA_Pressed,FALSE,ID_COMMAND);notify(auto_run,MUIA_Pressed,FALSE,ID_AUTO);notify(script,MUIA_Pressed,FALSE,ID_SCRIPT);notify(browse_button,MUIA_Pressed,FALSE,ID_BROWSE_DRAWER);
 disk_ui_wire();
 notify(boot_button,MUIA_Pressed,FALSE,ID_BOOT_DISK);
 notify(audio_check,MUIA_Pressed,FALSE,ID_AUDIO_CHECK);notify(github,MUIA_Pressed,FALSE,ID_GITHUB);
 notify(cd_mode,MUIA_Cycle_Active,MUIV_EveryTime,ID_CD_MODE);notify(cd_browse,MUIA_Pressed,FALSE,ID_CD_BROWSE);notify(cd_check,MUIA_Pressed,FALSE,ID_CD_CHECK);
 notify(terminal_reg,MUIA_Group_ActivePage,MUIV_EveryTime,ID_TERMPAGE);
 notify(startup_mode[0],MUIA_Cycle_Active,MUIV_EveryTime,ID_STARTMODE);notify(startup_mode[1],MUIA_Cycle_Active,MUIV_EveryTime,ID_STARTMODE);
 notify(display,MUIA_Cycle_Active,MUIV_EveryTime,ID_DISPLAY);notify(machine,MUIA_Cycle_Active,MUIV_EveryTime,ID_DISPLAY);notify(fps,MUIA_Selected,MUIV_EveryTime,ID_DISPLAY);
 notify(system_choice,MUIA_Cycle_Active,MUIV_EveryTime,ID_SYSTEM);
 notify(open_drawer,MUIA_Pressed,FALSE,ID_OPEN_DRAWER);notify(mount_disks,MUIA_Pressed,FALSE,ID_MOUNT_DISKS);notify(eject_disks,MUIA_Pressed,FALSE,ID_EJECT_DISKS);
 wire(root);notify(editors[2].line,MUIA_String_Acknowledge,MUIV_EveryTime,ID_EDITOR+34);notify(editors[2].list,MUIA_List_Active,MUIV_EveryTime,ID_EDITOR+32);notify(editors[0].list,MUIA_List_Active,MUIV_EveryTime,ID_EDITOR);notify(editors[1].list,MUIA_List_Active,MUIV_EveryTime,ID_EDITOR+16);return 1;
}
#include "splash.h"
static void seed(void){const char*dirs[]={"KEEN6","VETTE","ELITE"},*titles[]={"Commander Keen 6","Vette","Elite"},*programs[]={"KEEN6C.EXE","VETTE.EXE","ELITE.EXE"};char parent[DL_PATH];unsigned i;strcpy(parent,appdir);*PathPart(parent)=0;
 {struct dl_profile*p=&library.p[library.count];dl_profile_default(p);p->category=DL_OS;strcpy(p->title,"DOS hard drive");strcpy(p->drive,parent);AddPart(p->drive,"Drives/C",DL_PATH);if(exists(p->drive))library.count++;}
 for(i=0;i<3;i++){struct dl_profile*p=&library.p[library.count];dl_profile_default(p);p->category=DL_GAMES;strcpy(p->title,titles[i]);strcpy(p->drive,parent);AddPart(p->drive,"Shell/Games",DL_PATH);AddPart(p->drive,(STRPTR)dirs[i],DL_PATH);strcpy(p->program,programs[i]);p->helper=i!=0;if(exists(p->drive))library.count++;}}
static int __attribute__((noinline)) launcher_main(void){ULONG signals,id;int running=1,rc=20;struct MsgPort*owner=NULL;char p[DL_PATH];
 if(FindPort("ZZDOS_LAUNCHER"))return 0;owner=CreateMsgPort();if(!owner)return 20;owner->mp_Node.ln_Name="ZZDOS_LAUNCHER";AddPort(owner);
 if(!NameFromLock(GetProgramDir(),appdir,sizeof(appdir)))goto done;path(configpath,"Launcher.cfg");
 if(exists(configpath)){if(!dl_library_read(configpath,&library)){puts("Invalid Launcher.cfg; restore Launcher.bak. No changes made.");goto done;}}else seed();
 {unsigned i;int found=0;path(p,"Disks/DOS622.IMG");
  for(i=0;i<library.count;i++)if(library.p[i].system)found=1;
  if(!found&&library.count<DL_MAX&&exists(p)){struct dl_profile*q=&library.p[library.count];dl_profile_default(q);strcpy(q->title,"MS-DOS 6.22");path(q->drive,"Disks");q->system=1;q->category=DL_OS;q->memory=8;library.last=library.count++;}
 }
 MUIMasterBase=OpenLibrary("muimaster.library",15);AslBase=OpenLibrary("asl.library",38);WorkbenchBase=OpenLibrary("workbench.library",44);KeymapBase=OpenLibrary("keymap.library",37);if(!MUIMasterBase||!AslBase||!WorkbenchBase||!KeymapBase){puts("MUI 3.8 or later is required.");goto done;}
 disk_choices_init();if(!build())goto done;if(!conf_load())goto done;refresh();if(!startup_splash()){rc=0;goto done;}set(win,MUIA_Window_Open,TRUE);if(!val(win,MUIA_Window_Open))goto done;program_list_focus();
 while(running){mem_poll();id=DoMethod(app,MUIM_Application_NewInput,(ULONG)&signals);
  switch(id){case (ULONG)MUIV_Application_ReturnID_Quit:if(mem_busy){note("Wait for the pending memory request.");break;}if(!store_selection())break;if(!conf_save())break;if(!save()){note("Cannot save settings.");break;}running=0;break;
  case ID_NAVIGATE:navigation_show(val(navigation,MUIA_List_Active));break;
  case ID_PROGRAM_TAB:program_list_focus();break;
  case ID_SELECT:if(!busy){int n=(LONG)val(list,MUIA_List_Active);if(n!=selected&&!select_profile(n))SetAttrs(list,MUIA_NoNotify,TRUE,MUIA_List_Active,selected,TAG_DONE);}break;
  case ID_CATEGORY:if(!busy&&selected>=0&&library.p[selected].category!=val(category,MUIA_Cycle_Active)){if(store_selection()){refresh();note(save()?"":"Could not save category.");}}break;
  case ID_ADD:add();break;case ID_REMOVE:if(selected>=0){unsigned n=selected;memmove(&library.p[n],&library.p[n+1],(--library.count-n)*sizeof(library.p[0]));library.last=0;refresh();save();}break;
  case ID_SAVE:if(!store_selection()||!conf_save())break;note(save()?"Settings saved.":"Could not save settings.");refresh();break;
  case ID_RUN:launch(0,NULL);break;case ID_TERM:launch(1,NULL);break;case ID_COMMAND:launch(2,NULL);break;case ID_AUTO:launch(3,NULL);break;
  case ID_SCRIPT:if(browse(p,0,0,"Select DOS batch file")){char dir[DL_PATH];strcpy(dir,p);*PathPart(dir)=0;store_selection();if(selected>=0&&same_dir(dir,library.p[selected].drive))launch(4,FilePart(p));else note("Choose a batch file inside the selected C: drawer.");}break;
  case ID_DEV_CHANGE:set(dev_frame,MUIA_Disabled,!val(dev_break,MUIA_Cycle_Active));break;
  case ID_DEV_LOG:developer_log();break;
  case ID_COMPAT_FILTER:compat_filter();break;
  case ID_COMPAT_SELECT:compat_select();break;
  case ID_COMPAT_REPORT:compat_open();break;
  case ID_CONF_VALIDATE:if(conf_parse_editor())note("dosbox.conf is valid.");break;
  case ID_DISPLAY:display_controls();system_controls();break;
  case ID_STARTMODE:{unsigned i;for(i=0;i<2;i++)set(startup_file[i],MUIA_Disabled,val(startup_mode[i],MUIA_Cycle_Active)!=2);}break;
  case ID_TERMPAGE:{unsigned n=val(terminal_reg,MUIA_Group_ActivePage);if((n==1||n==2)&&val(system_choice,MUIA_Cycle_Active)!=2&&!val(startup_mode[n-1],MUIA_Cycle_Active)){char text[DL_TEXT];if(startup_read_default(n-1,text))editor_show(n-1,text);else note("Cannot read default startup file.");}}break;
  case ID_CD_MODE:{unsigned n=val(cd_mode,MUIA_Cycle_Active);set(cd_path,MUIA_Disabled,!n);set(cd_browse,MUIA_Disabled,n!=1);if(n==2&&(!*(char*)val(cd_path,MUIA_String_Contents)||strchr((char*)val(cd_path,MUIA_String_Contents),'/')))set(cd_path,MUIA_String_Contents,(ULONG)"UMSD:");}break;
  case ID_CD_BROWSE:if(browse(p,0,0,"Choose ISO image"))set(cd_path,MUIA_String_Contents,(ULONG)p);break;
  case ID_CD_CHECK:{unsigned n=val(cd_mode,MUIA_Cycle_Active);ULONG count;if(!n){note("Choose a CD-ROM source.");break;}set(app,MUIA_Application_Sleep,TRUE);count=db_cd_host_open(n,(char*)val(cd_path,MUIA_String_Contents));db_cd_host_close();set(app,MUIA_Application_Sleep,FALSE);note(count?"CD-ROM ready.":"Cannot read an ISO9660 data CD from that source.");}break;
  case ID_AUDIO_CHECK:audio_detect();break;
  case ID_GITHUB:if(exists("C:OpenURL"))SystemTags("C:OpenURL https://github.com/SkiltonUSA/ZZDosBox-ARM-68K",SYS_Asynch,FALSE,NP_StackSize,16384,TAG_DONE);else note("https://github.com/SkiltonUSA/ZZDosBox-ARM-68K (OpenURL required)");break;
  case ID_SYSTEM:system_controls();break;
  case ID_OPEN_DRAWER:if(selected>=0)disk_open_drawer((STRPTR)val(drawer,MUIA_String_Contents));break;
  case ID_MOUNT_HD:case ID_EJECT_HD:case ID_OPEN_HD:case ID_DISKS:case ID_DISK_SELECT:case ID_DISK_BROWSE:case ID_DISK_CREATE:disk_action(id);break;
  case ID_BOOT_DISK:boot_disk();break;
  case ID_MOUNT_DISKS:case ID_EJECT_DISKS:{char quoted[DL_PATH*2+3],cmd[DL_PATH*2+32];path(p,id==ID_MOUNT_DISKS?"Disks/Mount-DOS-Disks":"Disks/Eject-DOS-Disks");if(dl_quote(quoted,sizeof(quoted),p)){LONG result;snprintf(cmd,sizeof(cmd),"Execute %s",quoted);set(app,MUIA_Application_Sleep,TRUE);result=SystemTags(cmd,SYS_Asynch,FALSE,NP_StackSize,16384,TAG_DONE);set(app,MUIA_Application_Sleep,FALSE);note(result==0?(id==ID_MOUNT_DISKS?"Disks mounted in Workbench.":"Disks ejected."):"Disk action failed.");}break;}
  case ID_BROWSE_DRAWER:if(browse(p,1,0,"Select C: drawer"))set(drawer,MUIA_String_Contents,(ULONG)p);break;
  default:if(id>=ID_NET_MODE&&id<=ID_NET_GUIDE){network_action(id);break;}if(id>=ID_CAT_SELECT&&id<ID_CAT_SELECT+DL_CATEGORIES){unsigned c=id-ID_CAT_SELECT;if(!busy)select_profile(dl_category_index(&library,c,(LONG)val(category_lists[c],MUIA_List_Active)));break;}
  if((id>=ID_CAT_RUN&&id<ID_CAT_RUN+DL_CATEGORIES)||(id>=ID_CAT_EDIT&&id<ID_CAT_EDIT+DL_CATEGORIES)){unsigned c=id>=(unsigned)ID_CAT_EDIT?id-ID_CAT_EDIT:id-ID_CAT_RUN;int n=dl_category_index(&library,c,(LONG)val(category_lists[c],MUIA_List_Active));if(select_profile(n)){if(id>=ID_CAT_EDIT){navigation_show(PAGE_LOAD);set(program_tabs,MUIA_Group_ActivePage,0);}else launch(0,NULL);}break;}
  if(id>=ID_MEM_PAUSE&&id<=ID_MEM_STOP){mem_action(id);break;}if(id>=ID_EDITOR&&id<ID_EDITOR+48)editor_action((id-ID_EDITOR)/16,(id-ID_EDITOR)%16);break;}
  if(running&&!id&&signals){if(Wait(signals|SIGBREAKF_CTRL_C|(mem_replies?1ul<<mem_replies->mp_SigBit:0))&SIGBREAKF_CTRL_C){if(!mem_busy&&store_selection()&&conf_save()&&save())running=0;}}
 }rc=0;
done:compat_store_close(&compat_database);if(mem_replies)DeleteMsgPort(mem_replies);if(app)MUI_DisposeObject(app);if(program_list_class)MUI_DeleteCustomClass(program_list_class);if(KeymapBase)CloseLibrary(KeymapBase);style_close();if(WorkbenchBase)CloseLibrary(WorkbenchBase);if(AslBase)CloseLibrary(AslBase);if(MUIMasterBase)CloseLibrary(MUIMasterBase);if(owner){RemPort(owner);DeleteMsgPort(owner);}return rc;
}

/* Keep the entry frame small: reject undersized CLI stacks before entering
 * the MUI code, whose nested launch/save frames need more than 16 KiB. */
int main(void){
 struct Task*t=FindTask(NULL);
 if((ULONG)t->tc_SPUpper-(ULONG)t->tc_SPLower<65536){puts("ZZDosBox Launcher requires Stack 65536.");return 20;}
 return launcher_main();
}
