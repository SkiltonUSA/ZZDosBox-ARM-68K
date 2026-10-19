/* GPL-3.0-or-later. Screen budget, independent of MUI and screen modes. */
#ifndef ZZ_LAUNCHER_LAYOUT_H
#define ZZ_LAUNCHER_LAYOUT_H
struct ui_layout { unsigned width,height; int compact; };
static struct ui_layout ui_layout_for_screen(unsigned width,unsigned height,unsigned bar){
 struct ui_layout l;
 /* Leave room for Intuition borders, the public-screen bar and placement. */
 l.width=width>32?width-32:1;
 l.height=height>bar+40?height-bar-40:1;
 if(l.width>940)l.width=940;
 if(l.height>500)l.height=500;
 l.compact=l.width<820||l.height<460;
 return l;
}
#endif
