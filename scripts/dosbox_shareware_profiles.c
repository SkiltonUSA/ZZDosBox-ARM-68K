/* GPL-3.0-or-later. Generate a fresh starter library, never an owner's library. */
#include "amiga/dosbox/launcher/model.h"
#include <assert.h>
static struct dl_library library, readback;
int main(int argc, char **argv) {
    unsigned i;
    const char *titles[] = {"Doom 1.9 (shareware)", "Duke Nukem 3D (shareware)", "Descent (shareware)"};
    const char *dirs[] = {"DOOM19", "DUKE3DS", "DCNTSHR"};
    const char *commands[] = {"DOOM.EXE", "DUKE3D.EXE", "DESCENT.BAT"};
    assert(argc == 3);
    library.count = !strcmp(argv[2], "personal") ? 3 : 2;
    for (i = 0; i < library.count; i++) {
        struct dl_profile *p = &library.p[i];
        dl_profile_default(p);
        strcpy(p->title, titles[i]);
        snprintf(p->drive, sizeof(p->drive), "PROGDIR:Games/%s", dirs[i]);
        strcpy(p->program, commands[i]);
        p->category = DL_GAMES;
        p->memory = i == 2 ? 16 : 8;
        p->speed = 2;
        p->display = 1;
        p->ems = i != 0;
        p->cpu_model = i == 1 ? DB_CPU_486 : DB_CPU_386;
        p->sound = i == 1 ? 2 : i == 2 ? 1 : 0;
    }
    assert(dl_library_write(argv[1], &library));
    assert(dl_library_read(argv[1], &readback));
    assert(readback.count == library.count);
    for (i = 0; i < library.count; i++) {
        const struct dl_profile *p = &readback.p[i];
        assert(dl_profile_valid(p));
        assert(!strcmp(p->drive, library.p[i].drive));
        assert(p->memory == library.p[i].memory);
        assert(!strcmp(dl_profile_player(p), i ? "ZZDosBox-Expanded" : "ZZDosBox"));
    }
    return 0;
}
