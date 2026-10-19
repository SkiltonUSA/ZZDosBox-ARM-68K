/* GPL-3.0-or-later. Read-only dependency check and non-destructive first-run seeds. */
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/expansion.h>
#include <proto/asl.h>
#include <libraries/asl.h>
#include <devices/ahi.h>
#include <proto/ahi.h>
#include <dos/dosextens.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dependencies.h"
#include "../launcher/model.h"
#include "averify.h"
struct Library *AHIBase;
struct Library *AslBase;
struct ExpansionBase *ExpansionBase;
static struct dl_library library, defaults, check;
static const char version[] __attribute__((used))="$VER: ZZInstall 0.22.6 (19.10.2026)";

static int library_ok(const char *name, unsigned minimum) {
    struct Library *base=OpenLibrary((STRPTR)name,minimum);
    if(!base){printf("MISSING: %s v%u+\n",name,minimum);return 0;}
    printf("OK: %s %u.%u\n",name,base->lib_Version,base->lib_Revision);
    CloseLibrary(base);return 1;
}
static int audio_ok(void) {
    struct MsgPort *port=CreateMsgPort();struct AHIRequest *req=NULL;
    int ok=0,opened=0;unsigned modes=0;ULONG id=AHI_INVALID_ID;
    if(!port)goto done;
    req=(struct AHIRequest*)CreateIORequest(port,sizeof(*req));if(!req)goto done;
    req->ahir_Version=4;
    if(OpenDevice(AHINAME,AHI_NO_UNIT,(struct IORequest*)req,0))goto done;
    opened=1;AHIBase=(struct Library*)req->ahir_Std.io_Device;
    printf("AHI: device %u.%u\n",AHIBase->lib_Version,AHIBase->lib_Revision);
    while((id=AHI_NextAudioID(id))!=AHI_INVALID_ID)modes++;
    CloseDevice((struct IORequest*)req);opened=0;AHIBase=NULL;
    if(!modes){puts("MISSING: AHI audio modes/drivers");goto done;}
    req->ahir_Version=4;
    if(OpenDevice(AHINAME,0,(struct IORequest*)req,0)){puts("MISSING: configured AHI unit 0");goto done;}
    opened=1;ok=1;printf("OK: %u AHI modes; unit 0 opens (no sound played)\n",modes);
done:
    if(opened)CloseDevice((struct IORequest*)req);
    AHIBase=NULL;if(req)DeleteIORequest((struct IORequest*)req);if(port)DeleteMsgPort(port);
    if(!ok)puts("Install AHI v4+ and an output driver, then configure unit 0 in AHI preferences.");
    return ok;
}
static int dependencies(int demos) {
    unsigned available=0;int os=1;
    if(library_ok("muimaster.library",19))available|=ZZ_MUI;
    else puts("Install MUI 3.8 or later.");
    os&=library_ok("workbench.library",44);os&=library_ok("asl.library",38);
    os&=library_ok("keymap.library",37);if(os)available|=ZZ_OS;
    if(library_ok("Picasso96API.library",2))available|=ZZ_RTG;
    ExpansionBase=(struct ExpansionBase*)OpenLibrary("expansion.library",0);
    if(ExpansionBase){
        if(FindConfigDev(NULL,0x6d6e,4)&&FindConfigDev(NULL,0x6d6e,5))available|=ZZ_BOARD;
        CloseLibrary((struct Library*)ExpansionBase);ExpansionBase=NULL;
    }
    puts(available&ZZ_BOARD?"OK: ZZ9000 graphics and Fast RAM boards detected":"MISSING: ZZ9000 graphics/Fast RAM configuration");
    Forbid();
    if(!FindPort("ZZDOS_LAUNCHER")&&!FindPort("Sixies.ARM.Debug.Owner"))available|=ZZ_IDLE;
    Permit();
    if(!(available&ZZ_IDLE))puts("Close ZZDosBox and the current Core1 application before installing.");
    if(audio_ok())available|=ZZ_AUDIO;
    if(!(available&ZZ_AUDIO)&&!demos)puts("OPTIONAL: audio unavailable; Standard can run with Sound Off.");
    library_ok("camd.library",37); /* Informational: MIDI is optional. */
    if(zz_missing(available,demos)){puts("Dependencies not met; installation stopped.");return 10;}
    puts("Dependencies passed. XACP firmware, display modes and sound quality remain runtime checks.");
    return 0;
}
static int exists(const char *p){BPTR lock=Lock((STRPTR)p,ACCESS_READ);if(lock){UnLock(lock);return 1;}return 0;}
static int join(char *out,size_t cap,const char *base,const char *name){int n=snprintf(out,cap,"%s%s%s",base,(!base[0]||base[strlen(base)-1]==':')?"":"/",name);return n>0&&(size_t)n<cap;}
static int mkdir_ok(const char *p){BPTR lock;if(exists(p))return 1;lock=CreateDir((STRPTR)p);if(!lock)return 0;UnLock(lock);return 1;}
/* Seed only absent files. Games/configs created here are deliberately not receipt-owned. */
static int seed_tree(const char *src,const char *dest){
    BPTR lock=Lock((STRPTR)src,ACCESS_READ);struct FileInfoBlock *fib;
    int ok=0;char a[512],b[512];if(!lock)return 0;
    fib=AllocDosObject(DOS_FIB,NULL);if(!fib){UnLock(lock);return 0;}
    if(!mkdir_ok(dest)||!Examine(lock,fib))goto done;
    ok=1;
    while(ExNext(lock,fib)){
        if(!join(a,sizeof(a),src,(const char*)fib->fib_FileName)||!join(b,sizeof(b),dest,(const char*)fib->fib_FileName)){ok=0;break;}
        if(fib->fib_DirEntryType>0){if(!seed_tree(a,b)){ok=0;break;}}
        else if(!exists(b)){
            FILE *in=fopen(a,"rb"),*out=NULL;char buf[8192];size_t n;int copied=1;
            if(in)out=fopen(b,"wb");
            if(!in||!out)copied=0;
            else{while((n=fread(buf,1,sizeof(buf),in)))if(fwrite(buf,1,n,out)!=n){copied=0;break;}if(ferror(in))copied=0;}
            if(in)fclose(in);if(out&&fclose(out))copied=0;
            if(!copied){DeleteFile((STRPTR)b);ok=0;break;}
        }
    }
    if(ok&&IoErr()!=ERROR_NO_MORE_ENTRIES)ok=0;
done:FreeDosObject(DOS_FIB,fib);UnLock(lock);return ok;
}
static int seed(const char *dest,int demos){
    char cfg[512],tmp[512],bak[512],src[512],games[512];unsigned i,j;int changed=0;
    if(!join(cfg,sizeof(cfg),dest,"PC/Launcher.cfg")||!join(tmp,sizeof(tmp),dest,"PC/Launcher.install-new")||!join(bak,sizeof(bak),dest,"PC/Launcher.install-backup"))return 10;
    memset(&library,0,sizeof(library));
    if(exists(cfg)&&!dl_library_read(cfg,&library)){puts("Existing library is unreadable; preserved without changes.");return 10;}
    if(demos){
        if(!join(src,sizeof(src),dest,"DemoDefaults/Launcher.cfg")||!dl_library_read(src,&defaults))return 10;
        if(!join(src,sizeof(src),dest,"DemoDefaults/Games")||!join(games,sizeof(games),dest,"PC/Games")||!seed_tree(src,games))return 10;
        for(i=0;i<defaults.count;i++){
            for(j=0;j<library.count;j++)if(!stricmp(library.p[j].drive,defaults.p[i].drive)&&!stricmp(library.p[j].program,defaults.p[i].program))break;
            if(j==library.count){if(library.count==DL_MAX){puts("Library full; existing entries preserved.");return 10;}library.p[library.count++]=defaults.p[i];changed=1;}
        }
    }
    if(exists(cfg)&&!changed){puts("Existing launcher library preserved.");return 0;}
    if(!dl_library_write(tmp,&library)||!dl_library_read(tmp,&check))return 10;
    if(exists(cfg)){
        if(exists(bak)){puts("Previous install backup exists; keep it and rename it before retrying.");DeleteFile((STRPTR)tmp);return 10;}
        if(!Rename((STRPTR)cfg,(STRPTR)bak))return 10;
    }
    if(!Rename((STRPTR)tmp,(STRPTR)cfg)){if(exists(bak))Rename((STRPTR)bak,(STRPTR)cfg);return 10;}
    puts("Launcher library ready; existing profiles and saves preserved.");return 0;
}
static char *read_all(const char *path){FILE*f=fopen(path,"rb");long n;char*b;if(!f)return NULL;fseek(f,0,SEEK_END);n=ftell(f);rewind(f);if(n<0||n>2097152){fclose(f);return NULL;}b=malloc(n+1);if(!b){fclose(f);return NULL;}if(fread(b,1,n,f)!=(size_t)n){free(b);fclose(f);return NULL;}b[n]=0;fclose(f);return b;}
int main(int argc,char **argv){
    if(argc==2&&!strcmp(argv[1],"choose")){
        struct FileRequester *request;char dest[512];int ok=0;
        AslBase=OpenLibrary("asl.library",38);if(!AslBase)return 10;
        request=AllocAslRequestTags(ASL_FileRequest,ASLFR_TitleText,(ULONG)"Choose parent drawer for ZZDosBox",ASLFR_DrawersOnly,TRUE,ASLFR_InitialDrawer,(ULONG)"SYS:Programs",TAG_DONE);
        if(request&&AslRequestTags(request,TAG_DONE)&&join(dest,sizeof(dest),(const char*)request->fr_Drawer,"ZZDosBox")){printf("\"%s\"\n",dest);ok=1;}
        if(request)FreeAslRequest(request);CloseLibrary(AslBase);return ok?0:10;
    }
    if(argc==3&&!strcmp(argv[1],"check")&&(!strcmp(argv[2],"standard")||!strcmp(argv[2],"demos")))return dependencies(!strcmp(argv[2],"demos"));
    if(argc==4&&!strcmp(argv[1],"seed")&&(!strcmp(argv[2],"standard")||!strcmp(argv[2],"demos")))return seed(argv[3],!strcmp(argv[2],"demos"));
    if(argc==5&&!strcmp(argv[1],"verify")){
        char *catalog=read_all(argv[2]),*sig=read_all(argv[3]),*key=read_all(argv[4]);int ok=0;
        if(catalog&&sig&&key){key[strcspn(key,"\r\n")]=0;sig[strcspn(sig,"\r\n")]=0;ok=amipkg_verify_index_key((unsigned char*)catalog,strlen(catalog),sig,key);}
        free(catalog);free(sig);free(key);puts(ok?"Catalogue signature verified.":"Invalid catalogue signature; installation stopped.");return ok?0:10;
    }
    puts("ZZInstall check standard|demos / seed standard|demos destination / verify catalogue signature public-key");return 10;
}
