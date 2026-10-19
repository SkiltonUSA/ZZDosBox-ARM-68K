/* GPL-3.0-or-later. Installation requirements; optional audio for Standard. */
#ifndef ZZ_INSTALL_DEPENDENCIES_H
#define ZZ_INSTALL_DEPENDENCIES_H
enum { ZZ_MUI=1, ZZ_OS=2, ZZ_RTG=4, ZZ_BOARD=8, ZZ_AUDIO=16, ZZ_IDLE=32 };
static unsigned zz_missing(unsigned available, int demos) {
    unsigned required=ZZ_MUI|ZZ_OS|ZZ_RTG|ZZ_BOARD|ZZ_IDLE;
    if(demos)required|=ZZ_AUDIO;
    return required & ~available;
}
#endif
