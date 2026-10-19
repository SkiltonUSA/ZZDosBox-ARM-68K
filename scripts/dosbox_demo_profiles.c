/* GPL-3.0-or-later. Fresh, relocatable demo library; no personal profiles. */
#include "amiga/dosbox/launcher/model.h"
#include <assert.h>
static struct dl_library library, check;
int main(int argc, char **argv) {
    const char *titles[] = {"Corridor 7 Demo", "Doom 1.9 Shareware", "King's Quest VII Demo"};
    const char *dirs[] = {"CORR7", "DOOM19", "KQ7"};
    const char *commands[] = {"CORR7.EXE", "DOOM.EXE", "SIERRA.EXE"};
    assert(argc == 2);
    library.count = 3;
    for (unsigned i = 0; i < library.count; i++) {
        struct dl_profile *p = &library.p[i];
        dl_profile_default(p);
        strcpy(p->title, titles[i]);
        snprintf(p->drive, sizeof(p->drive), "PROGDIR:Games/%s", dirs[i]);
        strcpy(p->program, commands[i]);
        p->category = DL_GAMES;
        p->memory = 8;
        p->speed = i == 2 ? 1 : 2; /* 3000 / 6000 cycles */
        p->machine = i == 2 ? 3 : 0; /* S3/VESA / VGA */
        p->sound = i == 1 ? 0 : 1; /* SB Pro2 matches Sierra drivers */
        p->ems = i == 0;
        p->display = i == 1; /* retain tested windowed Corridor/KQ7 */
    }
    assert(dl_library_write(argv[1], &library));
    assert(dl_library_read(argv[1], &check) && check.count == 3);
    for (unsigned i = 0; i < check.count; i++) {
        const struct dl_profile *p = &check.p[i];
        assert(dl_profile_valid(p));
        assert(!strcmp(p->drive, library.p[i].drive));
        assert(!strcmp(p->program, commands[i]));
        assert(p->sound == library.p[i].sound && p->ems == library.p[i].ems);
        assert(dl_profile_cycles(p) == (i == 2 ? 3000 : 6000));
        assert(!strcmp(dl_profile_player(p), i == 1 ? "ZZDosBox" : "ZZDosBox-Compat"));
    }
    return 0;
}
