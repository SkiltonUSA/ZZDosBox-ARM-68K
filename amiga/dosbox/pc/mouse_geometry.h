/* GPL-3.0-or-later. Map the painted guest rectangle, excluding window borders. */
#pragma once
struct pc_mouse_point {unsigned x,y;int inside;};
static struct pc_mouse_point pc_mouse_map(int x,int y,int left,int top,unsigned width,unsigned height){
 struct pc_mouse_point p={0,0,0};if(width<2||height<2)return p;
 x-=left;y-=top;p.inside=x>=0&&y>=0&&(unsigned)x<width&&(unsigned)y<height;
 if(x<0)x=0;
 if(y<0)y=0;
 if((unsigned)x>=width)x=width-1;
 if((unsigned)y>=height)y=height-1;
 p.x=(unsigned)x*65535u/(width-1);p.y=(unsigned)y*65535u/(height-1);return p;
}
