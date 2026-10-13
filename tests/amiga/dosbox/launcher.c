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
 unlink(tmp);puts("PASS profile/session round trip, malformed input, quoted paths, default/edited AUTOEXEC and CRLF");return 0;}
