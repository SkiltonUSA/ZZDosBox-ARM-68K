#!/usr/bin/env python3
"""Exercise the production Workbench-volume resolver with mocked Amiga DOS."""
from pathlib import Path
import subprocess,tempfile
r=Path(__file__).resolve().parents[1]
s=(r/'amiga/dosbox/launcher/cd_ui.h').read_text()
s=s[s.index('static int cd_volume_device'):s.index('static void cd_controls')]
prefix=r'''
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
typedef void* APTR;typedef char*STRPTR;typedef unsigned char UBYTE;
#define LDF_DEVICES 1
#define LDF_READ 2
#define BADDR(x) (x)
struct Process {APTR pr_WindowPtr;} task;
struct DosList {void*dol_Task;UBYTE*dol_Name;struct DosList*next;};
struct DevProc {void*dvp_Port;} proc;
static UBYTE disk[]={4,'S','D','H','0'},cd[]={3,'C','D','0'};
static int p1,p2,freed,unlocked,fail_lock;
static struct DosList end={&p2,cd,0},first={&p1,disk,&end},head={0,0,&first};
static void*FindTask(void*x){(void)x;return &task;}
static struct DevProc*GetDeviceProc(char*name,void*x){(void)x;assert(task.pr_WindowPtr==(APTR)-1);if(!strncmp(name,"Disc:",5)){proc.dvp_Port=&p2;return &proc;}return 0;}
static struct DosList*LockDosList(int flags){assert(flags==3);return fail_lock?0:&head;}
static struct DosList*NextDosEntry(struct DosList*p,int flags){assert(flags==1);return p->next;}
static void UnLockDosList(int flags){assert(flags==3);unlocked++;}
static void FreeDeviceProc(struct DevProc*p){assert(p==&proc);freed++;}
'''
main=r'''
int main(void){char out[108];task.pr_WindowPtr=&p1;
 assert(cd_volume_device("Disc:",out,sizeof(out))&&!strcmp(out,"CD0:"));
 assert(cd_volume_device("Disc:INSTALL",out,sizeof(out))&&!strcmp(out,"CD0:"));
 assert(!cd_volume_device("Missing:",out,sizeof(out)));
 assert(!cd_volume_device("Disc:",out,4));
 fail_lock=1;assert(!cd_volume_device("Disc:",out,sizeof(out)));
 assert(freed==4&&unlocked==3&&task.pr_WindowPtr==&p1);
 puts("PASS CD volume/device resolution, subfolder, missing volume, bounds and cleanup");}
'''
with tempfile.TemporaryDirectory() as d:
 p=Path(d);(p/'test.c').write_text(prefix+s+main)
 subprocess.run(['clang','-std=c99','-Wall','-Wextra','-Werror','-fsanitize=address,undefined',p/'test.c','-o',p/'test'],check=True)
 subprocess.run([p/'test'],check=True)
