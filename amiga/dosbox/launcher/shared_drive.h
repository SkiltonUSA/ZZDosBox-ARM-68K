/* A mounted Amiga drawer is the embedded DOS hard drive, without an image. */
#ifndef DL_SHARED_DRIVE_H
#define DL_SHARED_DRIVE_H
#include "model.h"
static int dl_shared_profile(struct dl_profile*p,const char*folder,const char*name){
 if(!*folder||strlen(folder)>=sizeof(p->drive)||!dl_plain(folder)||!*name||strlen(name)>=sizeof(p->title)||!dl_plain(name))return 0;
 dl_profile_default(p);strcpy(p->drive,folder);strcpy(p->title,name);
 p->category=DL_OS;p->memory=8;p->sound=2;p->speaker=1;p->ems=1;
 return dl_profile_valid(p);
}
#endif
