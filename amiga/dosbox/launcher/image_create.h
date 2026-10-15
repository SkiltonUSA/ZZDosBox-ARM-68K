/* GPL-3.0-or-later. Bounded FAT16 writer for user-supplied MS-DOS files.
 * No Microsoft boot code or system files are embedded in this source. */
#ifndef ZZ_IMAGE_CREATE_H
#define ZZ_IMAGE_CREATE_H
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include "image_mount.h"
typedef int (*di_progress)(unsigned percent, const char *phase, void *opaque);
struct di_writer {FILE *out;unsigned spc,spf,clusters,next;uint32_t root,data;unsigned char *fat,*buffer;di_progress progress;void *opaque;const char *error;};
static void di_put16(unsigned char*p,unsigned n){p[0]=n;p[1]=n>>8;}
static void di_put32(unsigned char*p,uint32_t n){di_put16(p,n);di_put16(p+2,n>>16);}
static unsigned di_get16(const unsigned char*p){return p[0]|((unsigned)p[1]<<8);}
static int di_write(struct di_writer*w,uint32_t off,const void*p,size_t n){if(fseek(w->out,(long)off,SEEK_SET)||fwrite(p,1,n,w->out)!=n){w->error="Cannot write image (check free space).";return 0;}return 1;}
static int di_tick(struct di_writer*w,unsigned percent,const char*phase){if(w->progress&&!w->progress(percent,phase,w->opaque)){w->error="Creation cancelled.";return 0;}return 1;}
static unsigned di_alloc(struct di_writer*w,unsigned count){unsigned first=w->next,i;if(!count)return 0;if(count>w->clusters+2-first){w->error="DOS files do not fit in the selected size.";return 0;}for(i=0;i<count;i++)di_put16(w->fat+(first+i)*2,i+1==count?0xffff:first+i+1);w->next+=count;return first;}
static uint32_t di_cluster(struct di_writer*w,unsigned cluster){return w->data+(cluster-2)*w->spc*512;}
static int di_name(unsigned char*out,const char*name){unsigned n=0,e=0;const char*p=name;memset(out,' ',11);while(*p&&*p!='.'){unsigned char c=*p++;if(c>='a'&&c<='z')c-=32;if(n==8||c<=32||strchr("\\/:*?\"<>|+[],;=",c))return 0;out[n++]=c;}if(*p=='.')p++;while(*p){unsigned char c=*p++;if(c>='a'&&c<='z')c-=32;if(e==3||c<=32||strchr(".\\/:*?\"<>|+[],;=",c))return 0;out[8+e++]=c;}return n!=0;}
static int di_entry(struct di_writer*w,uint32_t dir,unsigned *slot,const char*name,unsigned attr,unsigned cluster,uint32_t size){unsigned char e[32];if(*slot>=512){w->error="Too many files in a DOS directory.";return 0;}memset(e,0,sizeof(e));if(!strcmp(name,".")||!strcmp(name,"..")){memset(e,' ',11);memcpy(e,name,strlen(name));}else if(!di_name(e,name)){w->error="DOS source contains a non-8.3 filename.";return 0;}e[11]=attr;di_put16(e+24,33);di_put16(e+26,cluster);di_put32(e+28,size);return di_write(w,dir+32*(*slot)++,e,32);}
static int di_path(char*out,size_t n,const char*dir,const char*name){size_t l=strlen(dir);return snprintf(out,n,"%s%s%s",dir,l&&dir[l-1]!='/'&&dir[l-1]!=':'?"/":"",name)<(int)n;}
static int di_file(struct di_writer*w,uint32_t dir,unsigned*slot,const char*name,const char*source,unsigned attr){FILE*f;long bytes;unsigned cluster;uint32_t off,left;int ok=0;
 f=fopen(source,"rb");if(!f){w->error="Cannot read required DOS source file.";return 0;}
 if(fseek(f,0,SEEK_END)||(bytes=ftell(f))<0||bytes>268435456||fseek(f,0,SEEK_SET))goto done;
 cluster=di_alloc(w,((uint32_t)bytes+w->spc*512-1)/(w->spc*512));if(bytes&&!cluster)goto done;off=bytes?di_cluster(w,cluster):0;left=bytes;
 while(left){size_t n=left>16384?16384:left;if(fread(w->buffer,1,n,f)!=n||!di_write(w,off,w->buffer,n)||!di_tick(w,95,"Copying DOS files"))goto done;left-=n;off+=n;}
 ok=di_entry(w,dir,slot,name,attr,cluster,(uint32_t)bytes);
 done:if(!ok&&!w->error)w->error="Cannot copy DOS source file.";fclose(f);return ok;
}
static int di_dir(struct di_writer*w,uint32_t parent,unsigned*slot,unsigned parent_cluster,const char*name,const char*source,unsigned depth){unsigned cluster,entry=0;uint32_t offset;DIR*d=NULL;struct dirent*e;int ok=0;char child[512];struct stat st;
 if(depth>8){w->error="DOS directory nesting is too deep.";return 0;}
 cluster=di_alloc(w,(16384+w->spc*512-1)/(w->spc*512));if(!cluster)return 0;offset=di_cluster(w,cluster);
 if(!di_entry(w,parent,slot,name,16,cluster,0)||!di_entry(w,offset,&entry,".",16,cluster,0)||!di_entry(w,offset,&entry,"..",16,parent_cluster,0))return 0;
 if(!source)return 1;d=opendir(source);if(!d){w->error="Cannot read DOS directory.";return 0;}
 while((e=readdir(d))){if(!strcmp(e->d_name,".")||!strcmp(e->d_name,".."))continue;
  if(!di_path(child,sizeof(child),source,e->d_name)||stat(child,&st)){w->error="Cannot inspect DOS source file.";goto done;}
  if(S_ISDIR(st.st_mode)){if(!di_dir(w,offset,&entry,cluster,e->d_name,child,depth+1))goto done;}
  else if(S_ISREG(st.st_mode)){if(!di_file(w,offset,&entry,e->d_name,child,32))goto done;}
  else{w->error="Unsupported DOS source file type.";goto done;}
 }
 ok=1;
 done:closedir(d);return ok;
}
static int di_text(struct di_writer*w,unsigned*slot,const char*name,const char*text){unsigned n=strlen(text),c=di_alloc(w,(n+w->spc*512-1)/(w->spc*512));return c&&di_write(w,di_cluster(w,c),text,n)&&di_entry(w,w->root,slot,name,32,c,n);}
#include "image_source.h"
/* Output is a NEW temporary file owned by caller, never an existing image. */
static const char *di_create(FILE*out,const char*boot_image,const char*source,unsigned mb,di_progress progress,void*opaque){
 struct di_writer w;FILE*f=NULL;unsigned char mbr[512],boot[512];static unsigned char source_mbr[512],source_boot[512];uint32_t total,part,offset;unsigned slot=0,i;char file[512];const char*names[]={"IO.SYS","MSDOS.SYS","COMMAND.COM"};
 memset(&w,0,sizeof(w));w.out=out;w.progress=progress;w.opaque=opaque;w.next=2;
 if(mb!=32&&mb!=64&&mb!=128&&mb!=256)return "Choose 32, 64, 128 or 256 MB.";
 total=mb*2048;part=total-32;
 if(boot_image){f=fopen(boot_image,"rb");if(!f)return "MS-DOS source image is missing.";
 if(fread(mbr,1,512,f)!=512||!di_raw_fat_image(mbr,33554432u)||mbr[450]!=6||fseek(f,(long)di_le32(mbr+454)*512,SEEK_SET)||fread(boot,1,512,f)!=512){fclose(f);return "Expected the installed 32 MB FAT16 DOS source image.";}fclose(f);
 if(boot[510]!=0x55||boot[511]!=0xaa||di_get16(boot+11)!=512||memcmp(boot+54,"FAT16   ",8))return "Invalid DOS source boot sector.";
 for(i=62;i+22<=510;i++)if(!memcmp(boot+i,"IO      SYSMSDOS   SYS",22))break;
 if(i+22>510)return "DOS source needs a system-transferred MS-DOS boot sector.";
 memcpy(source_mbr,mbr,512);memcpy(source_boot,boot,512);
 if(source)for(i=0;i<3;i++){struct stat st;if(!di_path(file,sizeof(file),source,names[i])||stat(file,&st)||!S_ISREG(st.st_mode)||st.st_size<=0)return "Mount the installed MS-DOS disk first.";}
 }else{
 memset(mbr,0,sizeof(mbr));memset(boot,0,sizeof(boot));mbr[510]=boot[510]=0x55;mbr[511]=boot[511]=0xaa;
 boot[0]=0xeb;boot[1]=0x3c;boot[2]=0x90;memcpy(boot+3,"ZZDOS5  ",8);di_put16(boot+11,512);memcpy(boot+54,"FAT16   ",8);
 /* Data image for DOSBox's built-in DOS, not a bootable Microsoft DOS disk. */
 boot[62]=0xfa;boot[63]=0xf4;boot[64]=0xeb;boot[65]=0xfd;
 }
 w.spc=2;for(;;){w.spf=1;for(;;){unsigned need;w.clusters=(part-1-32-2*w.spf)/w.spc;need=((w.clusters+2)*2+511)/512;if(need<=w.spf)break;w.spf=need;}if(w.clusters<65525)break;w.spc*=2;}
 w.root=(32+1+2*w.spf)*512;w.data=w.root+16384;w.fat=calloc(w.spf,512);w.buffer=calloc(1,65536);if(!w.fat||!w.buffer){w.error="Not enough memory to prepare image.";goto done;}
 if(fseek(out,0,SEEK_SET)){w.error="Cannot start image.";goto done;}
 for(offset=0;offset<total*512;offset+=65536){if(fwrite(w.buffer,1,65536,out)!=65536){w.error="Cannot write image (check free space).";goto done;}if(!di_tick(&w,(unsigned)((offset/512)*90/total),"Preparing hard disk"))goto done;}
 memset(mbr+446,0,64);mbr[446]=boot_image?128:0;mbr[447]=1;mbr[448]=1;mbr[450]=6;mbr[451]=15;i=mb*4-1;mbr[452]=32|((i>>2)&192);mbr[453]=i;di_put32(mbr+454,32);di_put32(mbr+458,part);
 boot[13]=w.spc;di_put16(boot+14,1);boot[16]=2;di_put16(boot+17,512);di_put16(boot+19,part<65536?part:0);boot[21]=248;di_put16(boot+22,w.spf);di_put16(boot+24,32);di_put16(boot+26,16);di_put32(boot+28,32);di_put32(boot+32,part<65536?0:part);boot[36]=128;boot[37]=0;boot[38]=41;di_put32(boot+39,0x5a5a0000u+mb);memcpy(boot+43,"ZZDOSBOX   ",11);
 if(!di_write(&w,0,mbr,512)||!di_write(&w,16384,boot,512))goto done;di_put16(w.fat,0xfff8);di_put16(w.fat+2,0xffff);
 /* MS-DOS bootstrap requires IO.SYS and MSDOS.SYS first, contiguous. */
 if(boot_image&&source){for(i=0;i<3;i++){di_path(file,sizeof(file),source,names[i]);if(!di_file(&w,w.root,&slot,names[i],file,i<2?7:32))goto done;}
 if(!di_path(file,sizeof(file),source,"DOS")||!di_dir(&w,w.root,&slot,0,"DOS",file,0))goto done;}
 if(boot_image&&!source&&!di_source_copy(&w,boot_image,source_boot,source_mbr,&slot))goto done;
 if(!di_dir(&w,w.root,&slot,0,"TEMP",NULL,0))goto done;
 if(!di_text(&w,&slot,"AUTOEXEC.BAT",boot_image?"@ECHO OFF\r\nPATH C:\\DOS\r\nSET TEMP=C:\\TEMP\r\nPROMPT $P$G\r\nCLS\r\nECHO Welcome to ZZDosBox\r\nECHO MS-DOS 6.22\r\nECHO.\r\n":"@ECHO OFF\r\nPATH C:\\;Z:\\\r\nSET TEMP=C:\\TEMP\r\nECHO DOSBox DOS 5\r\n")||!di_text(&w,&slot,"CONFIG.SYS","FILES=40\r\nBUFFERS=20\r\nLASTDRIVE=Z\r\n"))goto done;
 if(!di_entry(&w,w.root,&slot,"ZZDOSBOX",8,0,0))goto done;
 for(i=0;i<2;i++)if(!di_write(&w,(33+i*w.spf)*512,w.fat,w.spf*512))goto done;
 if(fflush(out))w.error="Cannot finish writing the image.";else di_tick(&w,100,"Hard disk ready");
 done:free(w.fat);free(w.buffer);return w.error;
}
#endif
