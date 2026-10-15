/* GPL-3.0-or-later. Read the user's FAT16 DOS image without mounting fat95. */
struct di_reader {FILE*f;unsigned char*fat,*seen;unsigned clusters,spc;uint32_t data,root;unsigned root_bytes;};
static int di_source_read(struct di_reader*r,uint32_t off,void*p,size_t n){return !fseek(r->f,(long)off,SEEK_SET)&&fread(p,1,n,r->f)==n;}
static unsigned di_source_next(struct di_reader*r,unsigned c){return di_get16(r->fat+2*c);}
static int di_source_mark(struct di_reader*r,unsigned c){unsigned char mask;if(c<2||c>=r->clusters+2)return 0;mask=1u<<(c&7);if(r->seen[c/8]&mask)return 0;r->seen[c/8]|=mask;return 1;}
static int di_source_open(struct di_reader*r,const char*path,const unsigned char*boot,const unsigned char*mbr){
 uint32_t base=di_le32(mbr+454),part=di_le32(mbr+458),total=di_get16(boot+19);unsigned reserved=di_get16(boot+14),spf=di_get16(boot+22);long length;
 memset(r,0,sizeof(*r));r->spc=boot[13];r->root_bytes=di_get16(boot+17)*32;if(!total)total=di_le32(boot+32);
 if(!r->spc||r->spc>64||(r->spc&(r->spc-1))||!reserved||!spf||boot[16]!=2||!r->root_bytes||r->root_bytes>16384||r->root_bytes%512||total>part||total<=reserved+2*spf+r->root_bytes/512)return 0;
 r->clusters=(total-reserved-2*spf-r->root_bytes/512)/r->spc;
 if(r->clusters<4085||r->clusters>=65525||spf*256<r->clusters+2)return 0;
 r->root=(base+reserved+2*spf)*512;r->data=r->root+r->root_bytes;
 r->f=fopen(path,"rb");if(!r->f)return 0;
 if(fseek(r->f,0,SEEK_END)||(length=ftell(r->f))<0||(uint32_t)length<(base+total)*512)return 0;
 r->fat=malloc(spf*512);r->seen=calloc(1,(r->clusters+9)/8);
 return r->fat&&r->seen&&di_source_read(r,(base+reserved)*512,r->fat,spf*512);
}
static void di_source_close(struct di_reader*r){if(r->f)fclose(r->f);free(r->fat);free(r->seen);}
static unsigned char*di_source_dir(struct di_reader*r,unsigned c){unsigned char*b=calloc(1,16384);unsigned pos=0;if(!b)return NULL;
 if(!c){if(di_source_read(r,r->root,b,r->root_bytes))return b;}
 else{while(c<0xfff8){unsigned n=r->spc*512;if(pos+n>16384||!di_source_mark(r,c)||!di_source_read(r,r->data+(c-2)*n,b+pos,n))break;pos+=n;c=di_source_next(r,c);}if(c>=0xfff8)return b;}
 free(b);return NULL;
}
static const unsigned char*di_source_find(const unsigned char*dir,const char*name){unsigned char key[11];unsigned i;if(!di_name(key,name))return NULL;for(i=0;i<16384&&dir[i];i+=32)if(!memcmp(dir+i,key,11)&&dir[i+11]!=15&&!(dir[i+11]&8))return dir+i;return NULL;}
static void di_source_name(char*out,const unsigned char*e){unsigned i,n=0;for(i=0;i<8&&e[i]!=' ';i++)out[n++]=e[i];if(e[8]!=' '){out[n++]='.';for(i=8;i<11&&e[i]!=' ';i++)out[n++]=e[i];}out[n]=0;}
static int di_source_file(struct di_writer*w,struct di_reader*r,uint32_t dir,unsigned*slot,const unsigned char*e){
 char name[13];uint32_t size=di_le32(e+28),left=size,off;unsigned c=di_get16(e+26),dest;if(size>268435456)return 0;
 dest=di_alloc(w,(size+w->spc*512-1)/(w->spc*512));if(size&&!dest)return 0;off=size?di_cluster(w,dest):0;
 while(left){unsigned n=left>r->spc*512?r->spc*512:left;if(!di_source_mark(r,c)||!di_source_read(r,r->data+(c-2)*r->spc*512,w->buffer,n)||!di_write(w,off,w->buffer,n)||!di_tick(w,95,"Copying DOS files"))return 0;left-=n;off+=n;c=di_source_next(r,c);}
 if(size&&c<0xfff8)return 0;di_source_name(name,e);return di_entry(w,dir,slot,name,e[11]&39,dest,size);
}
static int di_source_tree(struct di_writer*w,struct di_reader*r,uint32_t parent,unsigned*slot,unsigned parent_cluster,const unsigned char*e,unsigned depth){
 unsigned cluster,entry=0,i;uint32_t offset;unsigned char*dir;char name[13];int ok=0;
 if(depth>8||di_get16(e+26)<2||di_get16(e+26)>=r->clusters+2)return 0;dir=di_source_dir(r,di_get16(e+26));if(!dir)return 0;
 cluster=di_alloc(w,(16384+w->spc*512-1)/(w->spc*512));if(!cluster)goto done;offset=di_cluster(w,cluster);di_source_name(name,e);
 if(!di_entry(w,parent,slot,name,16,cluster,0)||!di_entry(w,offset,&entry,".",16,cluster,0)||!di_entry(w,offset,&entry,"..",16,parent_cluster,0))goto done;
 for(i=0;i<16384&&dir[i];i+=32){const unsigned char*f=dir+i;if(f[0]==0xe5||f[0]=='.'||f[11]==15||(f[11]&8))continue;
  if(f[11]&16){if(!di_source_tree(w,r,offset,&entry,cluster,f,depth+1))goto done;}
  else if(!di_source_file(w,r,offset,&entry,f))goto done;
 }
 ok=1;
 done:free(dir);return ok;
}
static int di_source_copy(struct di_writer*w,const char*path,const unsigned char*boot,const unsigned char*mbr,unsigned*slot){struct di_reader r;unsigned char*root=NULL;const unsigned char*e;unsigned i;const char*names[]={"IO.SYS","MSDOS.SYS","COMMAND.COM"};int ok=0;
 if(!di_source_open(&r,path,boot,mbr))goto done;root=di_source_dir(&r,0);if(!root)goto done;
 for(i=0;i<3;i++){e=di_source_find(root,names[i]);if(!e||(e[11]&16)||!di_le32(e+28)||!di_source_file(w,&r,w->root,slot,e))goto done;}
 e=di_source_find(root,"DOS");if(!e||!(e[11]&16)||!di_source_tree(w,&r,w->root,slot,0,e,0))goto done;ok=1;
 done:free(root);di_source_close(&r);if(!ok&&!w->error)w->error="Cannot copy DOS files: source FAT16 image is incomplete or corrupt.";return ok;
}
