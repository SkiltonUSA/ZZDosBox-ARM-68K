/* GPL-3.0-or-later. Local per-program PPM previews, no network or datatypes. */
#include <proto/Picasso96.h>
struct Library *P96Base;
#define SNAP_W (ui_compact?128u:240u)
#define SNAP_H (ui_compact?96u:180u)
#define SNAP_BYTES (240*180*3)
static struct MUI_CustomClass *snapshot_class;
static Object *snapshot_views[DL_CATEGORIES+1],*snapshot_capture[DL_CATEGORIES+1];
static UBYTE snapshot_pixels[SNAP_BYTES];
static int snapshot_valid;
enum {ID_CAPTURE=94};
static int snapshot_path(const struct dl_profile *p,char *out){
 const char *parts[]={p->drive,p->program,p->image};uint32_t a=2166136261u,b=5381u;unsigned i;
 char name[48];
 for(i=0;i<3;i++){const unsigned char *s=(const unsigned char*)parts[i];unsigned c;
  do{c=*s++;if(c>='A'&&c<='Z')c+='a'-'A';a=(a^c)*16777619u;b=(b*33u)^c;}while(c);
 }
 a=(a^p->system)*16777619u;b=(b*33u)^p->system;
 snprintf(name,sizeof(name),"Snapshots/%08lx%08lx.ppm",(unsigned long)a,(unsigned long)b);strcpy(out,appdir);return AddPart(out,name,DL_PATH-4);
}
static void snapshot_refresh(void){
 char file[DL_PATH],line[64];FILE *f=NULL;unsigned w,h,x,y,next=0;UBYTE *row=NULL;
 snapshot_valid=0;
 if(selected>=0&&snapshot_path(&library.p[selected],file))f=fopen(file,"rb");
 if(f&&fgets(line,sizeof(line),f)&&!strcmp(line,"P6\n")&&fgets(line,sizeof(line),f)&&
    sscanf(line,"%u %u",&w,&h)==2&&w&&h&&w<=960&&h<=720&&
    fgets(line,sizeof(line),f)&&!strcmp(line,"255\n")){
  row=AllocVec(w*3,MEMF_PUBLIC);
  if(row){
   for(y=0;y<h;y++){
    if(fread(row,3,w,f)!=w)break;
    while(next<SNAP_H&&next*h/SNAP_H==y){
     for(x=0;x<SNAP_W;x++)memcpy(snapshot_pixels+(next*SNAP_W+x)*3,row+(x*w/SNAP_W)*3,3);
     next++;
    }
   }
   snapshot_valid=y==h&&next==SNAP_H&&fgetc(f)==EOF&&!ferror(f);FreeVec(row);
  }
 }
 if(f)fclose(f);
 for(x=0;x<DL_CATEGORIES+1;x++)if(snapshot_views[x])MUI_Redraw(snapshot_views[x],MADF_DRAWOBJECT);
}
static ULONG snapshot_dispatch(struct IClass *cl __asm("a0"),Object *obj __asm("a2"),Msg msg __asm("a1")){
 ULONG result; (void)cl;
 switch(msg->MethodID){
 case MUIM_AskMinMax:{struct MUIP_AskMinMax *m=(struct MUIP_AskMinMax*)msg;result=DoSuperMethodA(cl,obj,msg);
  m->MinMaxInfo->MinWidth+=SNAP_W;m->MinMaxInfo->DefWidth+=SNAP_W;m->MinMaxInfo->MaxWidth+=SNAP_W;
  m->MinMaxInfo->MinHeight+=SNAP_H;m->MinMaxInfo->DefHeight+=SNAP_H;m->MinMaxInfo->MaxHeight+=SNAP_H;return result;}
 case MUIM_Draw:
  result=DoSuperMethodA(cl,obj,msg);
  if(((struct MUIP_Draw*)msg)->flags&(MADF_DRAWOBJECT|MADF_DRAWUPDATE)){
   unsigned index=val(obj,MUIA_UserData);
   SetAPen(_rp(obj),_pens(obj)[MPEN_BACKGROUND]);RectFill(_rp(obj),_mleft(obj),_mtop(obj),_mright(obj),_mbottom(obj));
   if(snapshot_valid&&selected>=0&&(!index||library.p[selected].category==index-1)){struct RenderInfo ri;memset(&ri,0,sizeof(ri));ri.Memory=snapshot_pixels;ri.BytesPerRow=SNAP_W*3;ri.RGBFormat=RGBFB_R8G8B8;
    p96WritePixelArray(&ri,0,0,_rp(obj),_mleft(obj),_mtop(obj),SNAP_W,SNAP_H);
   }else{const char *s="No screenshot";SetAPen(_rp(obj),_pens(obj)[MPEN_TEXT]);Move(_rp(obj),_mleft(obj)+(SNAP_W-TextLength(_rp(obj),s,strlen(s)))/2,_mtop(obj)+SNAP_H/2);Text(_rp(obj),s,strlen(s));}
  }return result;
 }
 return DoSuperMethodA(cl,obj,msg);
}
static Object *snapshot_panel(unsigned index){Object *capture,*group;
 group=VGroup,Child,VSpace(0),Child,snapshot_views[index]=NewObject(snapshot_class->mcc_Class,NULL,
  MUIA_Frame,MUIV_Frame_Text,MUIA_UserData,index,TAG_DONE),
  Child,capture=snapshot_capture[index]=ui_button("Capture..."),Child,VSpace(0),End;
 set(capture,MUIA_ShortHelp,(ULONG)"Launch the selected program. Ctrl+F5 captures; F10 returns to the library.");
 return group;
}
