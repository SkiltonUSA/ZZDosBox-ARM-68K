/* GPL-3.0-or-later. Offline directory browser for the MUI launcher. */
#include "compatibility.h"
#include "compatibility_store.h"
static struct compat_store compat_database;
#include <utility/hooks.h>
enum {ID_COMPAT_FILTER=40,ID_COMPAT_SELECT,ID_COMPAT_REPORT};
static Object *compat_title,*compat_year,*compat_publisher,*compat_tester,*compat_versions;
static Object *compat_info,*compat_list,*compat_letter,*compat_query,*compat_count,*compat_url,*compat_report;
static unsigned compat_visible[COMPAT_COUNT],compat_visible_count;
static char compat_summary[96],compat_link[96];
static const char *compat_letters[]={"All","A","B","C","D","E","F","G","H","I","J","K","L","M","N","O","P","Q","R","S","T","U","V","W","X","Y","Z","0-9",NULL};
/* Entries outlive every display-hook invocation. HookEntry supplies the
 * standard C stack ABI from MUI's A0/A2/A1 register calling convention. */
static struct compat_version_row compat_version_rows[COMPAT_MAX_VERSIONS];
static ULONG compat_version_display(struct Hook *hook,char **columns,struct compat_version_row *entry){
 (void)hook;
 columns[0]=entry?entry->version:"DOSBox version";
 columns[1]=entry?entry->status:"Status";
 columns[2]=entry?entry->rating:"Rating";
 return 0;
}
static struct Hook compat_version_hook={{NULL,NULL},(ULONG (*)())HookEntry,(ULONG (*)())compat_version_display,NULL};
static void compat_info_line(const char *text){
 char line[96];const char *start=text;unsigned n,columns=48;
 /* Keep a readable measure even while the inactive MUI page has no layout. */
 while(*start){
  n=0;while(start[n]&&start[n]!='\n'&&n<columns)n++;
  if(start[n]&&start[n]!='\n'){unsigned split=n;while(split&&start[split]!=' ')split--;if(split)n=split;}
  memcpy(line,start,n);line[n]=0;
  DoMethod(compat_info,MUIM_List_InsertSingle,(ULONG)line,MUIV_List_Insert_Bottom);
  start+=n;if(*start=='\n')start++;while(*start==' ')start++;
 }
}
static void compat_show_details(unsigned id){
 const struct compat_game_details *d;char text[256],file[DL_PATH];unsigned count=0;int result;
 if(!compat_database.file){path(file,"Compatibility.dat");compat_store_open(&compat_database,file);}
 d=compat_store_find(&compat_database,id);
 const char *p;
 if(!d){compat_info_line(compat_database.error?compat_database.error:"No offline details for this game.");return;}
 set(compat_publisher,MUIA_Text_Contents,(ULONG)(*d->publisher?d->publisher:"Not listed"));
 set(compat_tester,MUIA_Text_Contents,(ULONG)(*d->tester?d->tester:"Not listed"));
 p=d->history;
 while(count<COMPAT_MAX_VERSIONS&&(result=compat_next_version(&p,&compat_version_rows[count]))>0){
  DoMethod(compat_versions,MUIM_List_InsertSingle,(ULONG)&compat_version_rows[count],MUIV_List_Insert_Bottom);count++;
 }
 if(*p)compat_info_line("Some version rows could not be displayed. See the original report.");
 if(d->comments){
  compat_info_line(d->discussion);
  snprintf(text,sizeof(text),"Latest comment: %s / %s",d->date,d->author);compat_info_line(text);
  compat_info_line("Latest-comment excerpt:");compat_info_line(d->preview);
 }else compat_info_line("No community comments on this report.");
 set(compat_info,MUIA_List_First,0);set(compat_versions,MUIA_List_First,0);
}
static void compat_select(void){
 LONG n=(LONG)val(compat_list,MUIA_List_Active);
 compat_link[0]=0;
 set(compat_info,MUIA_List_Quiet,TRUE);DoMethod(compat_info,MUIM_List_Clear);
 set(compat_versions,MUIA_List_Quiet,TRUE);DoMethod(compat_versions,MUIM_List_Clear);
 set(compat_title,MUIA_Text_Contents,(ULONG)"");set(compat_year,MUIA_Text_Contents,(ULONG)"");
 set(compat_publisher,MUIA_Text_Contents,(ULONG)"");set(compat_tester,MUIA_Text_Contents,(ULONG)"");
 if(n>=0&&(unsigned)n<compat_visible_count){
  const struct compat_entry *e=&compat_entries[compat_visible[n]];
  set(compat_title,MUIA_Text_Contents,(ULONG)e->title);set(compat_year,MUIA_Text_Contents,(ULONG)e->year);
  snprintf(compat_link,sizeof(compat_link),"https://www.dosbox.com/comp_list.php?showID=%u",e->id);
  compat_show_details(e->id);
 }
 set(compat_info,MUIA_List_Quiet,FALSE);set(compat_versions,MUIA_List_Quiet,FALSE);
 set(compat_url,MUIA_Text_Contents,(ULONG)compat_link);
 set(compat_report,MUIA_Disabled,!compat_link[0]);
}
static void compat_filter(void){
 unsigned i,bucket=val(compat_letter,MUIA_Cycle_Active);const char *query=(char*)val(compat_query,MUIA_String_Contents);
 set(compat_list,MUIA_List_Quiet,TRUE);DoMethod(compat_list,MUIM_List_Clear);compat_visible_count=0;
 for(i=0;i<COMPAT_COUNT;i++)if(compat_matches(&compat_entries[i],bucket,query)){
  /* String construct hook owns a copy; static source records remain read-only. */
  DoMethod(compat_list,MUIM_List_InsertSingle,(ULONG)compat_entries[i].title,MUIV_List_Insert_Bottom);
  if(val(compat_list,MUIA_List_Entries)!=compat_visible_count+1){note("Not enough memory to display all reports.");break;}
  compat_visible[compat_visible_count++]=i;
 }
 set(compat_list,MUIA_List_Quiet,FALSE);
 snprintf(compat_summary,sizeof(compat_summary),"%u of %u reports - " COMPAT_SNAPSHOT,compat_visible_count,(unsigned)COMPAT_COUNT);
 set(compat_count,MUIA_Text_Contents,(ULONG)compat_summary);
 set(compat_list,MUIA_List_Active,compat_visible_count?0:MUIV_List_Active_Off);compat_select();
}
static Object *compat_page(void){
 return VGroup,
  Child,label("DOSBox community reports - not verified ZZDosBox results."),
  Child,HGroup,Child,compat_letter=cycle(compat_letters),Child,compat_query=str(128),
   Child,TextObject,ButtonFrame,MUIA_InputMode,MUIV_InputMode_RelVerify,MUIA_Background,MUII_ButtonBack,MUIA_Text_Contents,(ULONG)"Search",MUIA_UserData,ID_COMPAT_FILTER,End,End,
  Child,HGroup,
  Child,ListviewObject,MUIA_HorizWeight,35,MUIA_Listview_List,compat_list=ListObject,InputListFrame,MUIA_List_ConstructHook,MUIV_List_ConstructHook_String,MUIA_List_DestructHook,MUIV_List_DestructHook_String,End,End,
  Child,VGroup,MUIA_HorizWeight,65,
   Child,ColGroup(2),
    Child,fieldlabel("Game"),Child,compat_title=label(""),
    Child,fieldlabel("Year"),Child,compat_year=label(""),
    Child,fieldlabel("Publisher"),Child,compat_publisher=label(""),
    Child,fieldlabel("Tested by"),Child,compat_tester=label(""),End,
   Child,ListviewObject,MUIA_VertWeight,45,MUIA_Listview_Input,FALSE,
    MUIA_Listview_List,compat_versions=ListObject,ReadListFrame,
     MUIA_List_Title,TRUE,MUIA_List_Format,(ULONG)"BAR,BAR,",
     MUIA_List_DisplayHook,(ULONG)&compat_version_hook,End,End,
   Child,ListviewObject,MUIA_VertWeight,55,MUIA_Listview_Input,FALSE,MUIA_Listview_List,compat_info=ListObject,ReadListFrame,MUIA_List_ConstructHook,MUIV_List_ConstructHook_String,MUIA_List_DestructHook,MUIV_List_DestructHook_String,End,End,
  End,
  End,
  Child,compat_count=label(""),Child,compat_url=label(""),
  Child,compat_report=SimpleButton("Original report"),End;
}
static void compat_init(void){
 set(compat_letter,MUIA_Cycle_Active,1);
 set(compat_query,MUIA_ShortHelp,(ULONG)"Search game titles; choose All to search every letter. Press Return to search.");
 set(compat_report,MUIA_ShortHelp,(ULONG)"Open the selected report in your browser (requires OpenURL and a network connection).");
 notify(compat_letter,MUIA_Cycle_Active,MUIV_EveryTime,ID_COMPAT_FILTER);
 notify(compat_query,MUIA_String_Acknowledge,MUIV_EveryTime,ID_COMPAT_FILTER);
 notify(compat_list,MUIA_List_Active,MUIV_EveryTime,ID_COMPAT_SELECT);
 notify(compat_report,MUIA_Pressed,FALSE,ID_COMPAT_REPORT);
 compat_filter();
}
static void compat_open(void){
 char cmd[128];if(!compat_link[0])return;
 if(!exists("C:OpenURL")){note("OpenURL is required; the report address is shown above.");return;}
 /* URL is constructed only from the numeric report ID, never website text. */
 snprintf(cmd,sizeof(cmd),"C:OpenURL %s",compat_link);
 if(SystemTags(cmd,SYS_Asynch,FALSE,NP_StackSize,16384,TAG_DONE)<0)note("Could not open the report.");
}
