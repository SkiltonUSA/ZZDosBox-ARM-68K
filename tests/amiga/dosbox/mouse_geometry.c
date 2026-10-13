#include <assert.h>
#include <stdio.h>
#include "mouse_geometry.h"
int main(void){
 struct pc_mouse_point p=pc_mouse_map(5,20,5,20,640,480);assert(p.inside&&!p.x&&!p.y);
 p=pc_mouse_map(644,499,5,20,640,480);assert(p.inside&&p.x==65535&&p.y==65535);
 p=pc_mouse_map(4,19,5,20,640,480);assert(!p.inside&&!p.x&&!p.y);
 p=pc_mouse_map(1000,1000,5,20,640,480);assert(!p.inside&&p.x==65535&&p.y==65535);
 p=pc_mouse_map(165,140,5,20,321,241);assert(p.inside&&p.x==32767&&p.y==32767);
 p=pc_mouse_map(0,0,0,0,0,0);assert(!p.inside);
 puts("PASS guest mouse bounds, scaled client, title/border exclusion and clamped drag");
}
