/* Native ASan/UBSan checks: malformed profiles, exact scripts and shell quoting. */
#include <assert.h>
#include <unistd.h>
#include "model.h"
int main(void){struct dl_session s,t;static struct dl_library a,b;char tmp[]="/tmp/zzdos-launcher-XXXXXX",q[100],out[DL_TEXT];int fd=mkstemp(tmp);FILE*f;size_t n;assert(fd>=0);close(fd);
 dl_defaults(&s);strcpy(s.drive,"SD032G:Games/Elite");strcpy(s.command,"ZZL0000.BAT");assert(dl_session_write(tmp,&s)&&dl_session_read(tmp,&t));assert(!memcmp(&s,&t,sizeof(s)));
 s.size=2;s.memory=8;s.display=1;s.fps=1;s.system=1;assert(dl_session_write(tmp,&s)&&dl_session_read(tmp,&t)&&t.memory==8&&t.size==2);
 s.memory=64;assert(!dl_session_write(tmp,&s));s.memory=2;s.fg=s.bg;assert(!dl_session_write(tmp,&s));s.fg=7;s.drive[0]='\n';assert(!dl_session_write(tmp,&s));
 f=fopen(tmp,"w");fputs("ZZDOSSESSION1\na\nb\n2 0 3000 7 0 1 0\nINJECT\n",f);fclose(f);assert(!dl_session_read(tmp,&t));
 assert(dl_quote(q,sizeof(q),"Games:Foo *\"Bar"));assert(!strcmp(q,"\"Games:Foo ***\"Bar\""));assert(!dl_quote(q,3,"abc"));assert(!dl_quote(q,sizeof(q),"x\nExecute bad"));
 a.count=2;a.last=1;dl_profile_default(&a.p[0]);strcpy(a.p[0].title,"Elite");strcpy(a.p[0].drive,"Games:Elite");strcpy(a.p[0].program,"ELITE.EXE");a.p[0].helper=1;a.p[0].edited=1;strcpy(a.p[0].autoexec,"@echo off\necho custom\n");a.p[1]=a.p[0];strcpy(a.p[1].title,"Keen");a.p[1].helper=0;a.p[1].machine=1;
 assert(dl_library_write(tmp,&a)&&dl_library_read(tmp,&b));assert(a.count==b.count&&a.last==b.last);for(unsigned i=0;i<a.count;i++){assert(!strcmp(a.p[i].title,b.p[i].title)&&!strcmp(a.p[i].drive,b.p[i].drive)&&!strcmp(a.p[i].program,b.p[i].program)&&!strcmp(a.p[i].autoexec,b.p[i].autoexec)&&!strcmp(a.p[i].configsys,b.p[i].configsys));assert(a.p[i].helper==b.p[i].helper&&a.p[i].machine==b.p[i].machine&&a.p[i].edited==b.p[i].edited);}
 f=fopen(tmp,"wb");assert(dl_boot(f,&a.p[0],"ELITE.EXE",1,1));fclose(f);f=fopen(tmp,"rb");n=fread(out,1,sizeof(out)-1,f);fclose(f);out[n]=0;assert(!strcmp(out,"@echo off\r\necho custom\r\n\r\nLOADFIX ELITE.EXE\r\n"));
 a.p[0].edited=0;f=fopen(tmp,"wb");assert(dl_boot(f,&a.p[0],"ELITE.EXE",1,1));fclose(f);f=fopen(tmp,"rb");n=fread(out,1,sizeof(out)-1,f);fclose(f);out[n]=0;assert(strstr(out,"path C:\\\r\n"));assert(!strstr(out,"custom"));
 memset(out,'a',256);out[256]=0;assert(!dl_text_valid(out));assert(!dl_text_valid("bad\033escape"));assert(dl_text_valid("DEVICE=C:\\DOS\\HIMEM.SYS\r\n"));
 f=fopen(tmp,"w");fputs("ZZDOSLIB1\n9999 0\n",f);fclose(f);assert(!dl_library_read(tmp,&b));
 f=fopen(tmp,"w");fputs("ZZDOSSESSION1\na\nb\n2 0 3000 7 0 1 0\n",f);fclose(f);assert(dl_session_read(tmp,&t)&&t.display==1&&!t.fps&&!t.system);
 s.system=2;strcpy(s.drive,"a");assert(!dl_session_write(tmp,&s));s.system=1;
 a.p[0].size=2;a.p[0].display=0;a.p[0].fps=1;a.p[0].system=1;
 assert(dl_library_write(tmp,&a)&&dl_library_read(tmp,&b));assert(b.p[0].fps==1&&b.p[0].system==1&&!b.p[0].display&&b.p[0].size==2);
 {long end;f=fopen(tmp,"r+");assert(f);assert(!fseek(f,-6,SEEK_END));end=ftell(f);fclose(f);assert(!truncate(tmp,end));assert(!dl_library_read(tmp,&b));}
 f=fopen(tmp,"w");fputs("ZZDOSLIB1\n1 0\n",f);
 assert(dl_hex_write(f,a.p[0].title)&&dl_hex_write(f,a.p[0].drive)&&dl_hex_write(f,a.p[0].program)&&dl_hex_write(f,a.p[0].autoexec)&&dl_hex_write(f,a.p[0].configsys));
 fputs("1 0 8 0 1 7 0 1\n",f);fclose(f);assert(dl_library_read(tmp,&b)&&b.p[0].display==1&&!b.p[0].fps&&!b.p[0].system);
 assert(b.p[0].size==1);
 f=fopen(tmp,"w");fputs("ZZDOSSESSION2\na\nb\n2 0 3000 7 0 1 0 0 1 1\n",f);fclose(f);assert(dl_session_read(tmp,&t)&&t.size==1&&t.system==1&&t.fps==1&&!t.display);
 f=fopen(tmp,"w");fputs("ZZDOSLIB2\n1 0\n",f);
 assert(dl_hex_write(f,a.p[0].title)&&dl_hex_write(f,a.p[0].drive)&&dl_hex_write(f,a.p[0].program)&&dl_hex_write(f,a.p[0].autoexec)&&dl_hex_write(f,a.p[0].configsys));
 fputs("1 0 8 0 1 7 0 1 0 1 1\n",f);fclose(f);assert(dl_library_read(tmp,&b)&&b.p[0].size==1&&!b.p[0].display&&b.p[0].fps&&b.p[0].system);
 s.size=3;assert(!dl_session_write(tmp,&s));a.p[0].size=3;assert(!dl_library_write(tmp,&a));
 dl_defaults(&s);strcpy(s.drive,"C:");s.cd_mode=2;strcpy(s.cd_path,"UMSD:");s.midi=1;strcpy(s.midi_port,"test.midi");assert(dl_session_write(tmp,&s)&&dl_session_read(tmp,&t)&&!memcmp(&s,&t,sizeof(s)));
 s.midi=3;assert(!dl_session_write(tmp,&s));s.midi=1;s.midi_port[0]=0;assert(!dl_session_write(tmp,&s));
 a.p[0].size=1;a.p[0].edited=2;strcpy(a.p[0].auto_file,"RAM:My startup.bat");a.p[0].config_mode=1;a.p[0].cd_mode=1;strcpy(a.p[0].cd_path,"SD:Test.iso");a.p[0].midi=2;strcpy(a.p[0].midi_port,"out.0");assert(dl_library_write(tmp,&a)&&dl_library_read(tmp,&b));assert(!memcmp(&a.p[0],&b.p[0],sizeof(a.p[0])));
 a.p[0].auto_file[0]=0;assert(!dl_library_write(tmp,&a));
 for(unsigned machine=0;machine<PC_ADAPTER_COUNT;machine++){
  dl_profile_default(&a.p[0]);strcpy(a.p[0].title,"Adapter");strcpy(a.p[0].drive,"RAM:Test");a.p[0].machine=machine;
  assert(dl_library_write(tmp,&a)&&dl_library_read(tmp,&b)&&b.p[0].machine==machine);
  dl_defaults(&s);strcpy(s.drive,"RAM:Test");s.machine=machine;
  assert(dl_session_write(tmp,&s)&&dl_session_read(tmp,&t)&&t.machine==machine);
 }
 a.p[0].machine=PC_ADAPTER_COUNT;assert(!dl_library_write(tmp,&a));s.machine=PC_ADAPTER_COUNT;assert(!dl_session_write(tmp,&s));
 /* Scanline preferences are per-program, retained while temporarily unavailable. */
 a.count=2;a.last=1;
 for(unsigned i=0;i<2;i++){dl_profile_default(&a.p[i]);strcpy(a.p[i].title,i?"Masked":"Normal");strcpy(a.p[i].drive,"RAM:Test");a.p[i].display=1;}
 assert(!a.p[0].scanlines);a.p[1].scanlines=1;
 assert(dl_library_write(tmp,&a)&&dl_library_read(tmp,&b));
 assert(!memcmp(&a,&b,sizeof(a)));
 assert(!dl_scanlines_active(&b.p[0])&&dl_scanlines_active(&b.p[1]));
 b.p[1].display=0;assert(!dl_scanlines_active(&b.p[1]));b.p[1].display=1;b.p[1].fps=1;assert(!dl_scanlines_active(&b.p[1]));b.p[1].fps=0;assert(dl_scanlines_active(&b.p[1]));
 a.p[1].scanlines=4;assert(!dl_library_write(tmp,&a));a.p[1].scanlines=1;
 for(unsigned style=1;style<=3;style++){
  a.p[1].scanlines=style;assert(dl_library_write(tmp,&a)&&dl_library_read(tmp,&b));assert(b.p[1].scanlines==style);
  assert(!strcmp(dl_scanline_args(&b.p[1]),style==1?" --scanlines classic":style==2?" --scanlines soft":" --scanlines gradient"));
  b.p[1].fps=1;assert(!*dl_scanline_args(&b.p[1]));
 }
 /* Presentation is independent of display mode and scanline availability. */
 a.p[1].presentation=1;assert(dl_library_write(tmp,&a)&&dl_library_read(tmp,&b));
 assert(!b.p[0].presentation&&b.p[1].presentation==1);
 for(unsigned present=0;present<2;present++)for(unsigned display=0;display<2;display++)for(unsigned fps=0;fps<2;fps++)for(unsigned style=0;style<4;style++){
  struct dl_profile*p=&b.p[0];p->presentation=present;p->display=display;p->fps=fps;p->scanlines=style;
  int masked=style&&display&&!fps;
  assert(!strcmp(dl_player_name(present,dl_scanlines_active(p)),present?"ZZDosBox-Adaptive":masked?"ZZDosBox-Scanlines":"ZZDosBox"));
  assert(!strcmp(dl_presentation_args(present),present?" --presentation adaptive --present-hz 60":""));
 }
 a.p[1].presentation=2;assert(!dl_library_write(tmp,&a));a.p[1].presentation=1;
 /* v5-v8 migrate with white phosphor / direct rendering defaults. */
 a.count=1;a.last=0;a.p[0].presentation=0;a.p[0].scanlines=0;
 for(unsigned colour=0;colour<4;colour++)for(unsigned renderer=0;renderer<2;renderer++){
  a.p[0].monochrome=colour;a.p[0].renderer=renderer;
  assert(dl_library_write(tmp,&a)&&dl_library_read(tmp,&b));
  assert(b.p[0].monochrome==colour&&b.p[0].renderer==renderer);
  assert(!strcmp(dl_profile_player(&b.p[0]),renderer?"ZZDosBox-OpenGL":"ZZDosBox"));
 }
 a.p[0].renderer=2;assert(!dl_library_write(tmp,&a));a.p[0].renderer=0;
 a.p[0].monochrome=4;assert(!dl_library_write(tmp,&a));a.p[0].monochrome=0;
 /* Categories persist and filtering maps rows to stable profile indices. */
 for(unsigned c=0;c<DL_CATEGORIES;c++){
  a.p[0].category=c;assert(dl_library_write(tmp,&a)&&dl_library_read(tmp,&b));
  assert(!memcmp(&a.p[0],&b.p[0],sizeof(a.p[0])));
 }
 a.p[0].category=DL_CATEGORIES;assert(!dl_library_write(tmp,&a));
 a.p[0].category=dl_legacy_category(&a.p[0]);
 assert(dl_library_write(tmp,&a));f=fopen(tmp,"rb");n=fread(out,1,sizeof(out)-1,f);fclose(f);
 assert(n>18&&!memcmp(out,"ZZDOSLIB12",10));
 /* Remove the appended network record to exercise v5-v11 migrations. */
 out[n]=0;char *netrecord=strstr(out,"ZZNET1\n");assert(netrecord);size_t full_n=n;n=(size_t)(netrecord-out);out[9]='1';
 f=fopen(tmp,"wb");assert(fwrite(out,1,n,f)==n);fclose(f);assert(dl_library_read(tmp,&b)&&!memcmp(&a.p[0],&b.p[0],sizeof(a.p[0])));
 /* Strip only the fields absent from each old format; the two-digit header
    shrinks by one byte. All launch settings must survive migration. */
 for(unsigned v=5;v<=10;v++){
  f=fopen(tmp,"wb");fprintf(f,"ZZDOSLIB%u",v);
  assert(fwrite(out+10,1,n-11-(10-v)*2,f)==n-11-(10-v)*2);fclose(f);
  assert(dl_library_read(tmp,&b)&&b.count==1&&!memcmp(&a.p[0],&b.p[0],sizeof(a.p[0])));
 }
 out[9]='2';n=full_n;
 f=fopen(tmp,"wb");assert(fwrite(out,1,n-2,f)==n-2);fclose(f);assert(!dl_library_read(tmp,&b));
 out[n-3]='!';f=fopen(tmp,"wb");assert(fwrite(out,1,n,f)==n);fclose(f);assert(!dl_library_read(tmp,&b));out[n-3]='o';
 f=fopen(tmp,"wb");assert(fwrite(out,1,n,f)==n);fputs("extra\n",f);fclose(f);assert(!dl_library_read(tmp,&b));
 a.count=4;a.p[0].category=DL_GAMES;a.p[1]=a.p[2]=a.p[3]=a.p[0];
 a.p[1].category=DL_APPLICATIONS;a.p[3].category=DL_OS;
 assert(dl_category_index(&a,DL_GAMES,0)==0&&dl_category_index(&a,DL_GAMES,1)==2);
 assert(dl_category_index(&a,DL_APPLICATIONS,0)==1&&dl_category_index(&a,DL_OS,0)==3);
 assert(dl_category_index(&a,DL_GAMES,2)==-1&&dl_category_index(&a,DL_OS,-1)==-1);
 a.p[0].category=DL_APPLICATIONS;
 assert(dl_category_index(&a,DL_GAMES,0)==2&&dl_category_index(&a,DL_APPLICATIONS,1)==1);
 memmove(a.p,a.p+1,3*sizeof(a.p[0]));a.count--;
 assert(dl_category_index(&a,DL_GAMES,0)==1&&dl_category_index(&a,DL_OS,0)==2);
 dl_defaults(&s);strcpy(s.drive,"SD:Apps");s.system=2;strcpy(s.image,"APPS.IMG");s.machine=0;s.midi=0;s.cd_mode=0;
 assert(dl_session_write(tmp,&s)&&dl_session_read(tmp,&t)&&!memcmp(&s,&t,sizeof(s)));
 strcpy(s.image,"APPS2026.IMG");assert(dl_session_write(tmp,&s)&&dl_session_read(tmp,&t)&&!strcmp(t.image,s.image));
 strcpy(s.image,"../BAD");assert(!dl_session_write(tmp,&s));
 a.count=1;a.last=0;dl_profile_default(&a.p[0]);strcpy(a.p[0].title,"Windows");strcpy(a.p[0].drive,"SD:Apps");a.p[0].system=2;strcpy(a.p[0].image,"APPS.IMG");
 assert(dl_library_write(tmp,&a)&&dl_library_read(tmp,&b)&&!memcmp(&a.p[0],&b.p[0],sizeof(a.p[0])));
 a.p[0].network.enabled=1;a.p[0].network.manual=1;a.p[0].network.mtu=1200;strcpy(a.p[0].network.ip,"10.0.2.42");strcpy(a.p[0].network.dns,"10.0.0.1");
 assert(dl_library_write(tmp,&a)&&dl_library_read(tmp,&b)&&!memcmp(&a.p[0],&b.p[0],sizeof(a.p[0])));
 unlink(tmp);puts("PASS profiles/sessions, adapter/colour/renderer migration and validation");return 0;}
