/* GPL-3.0-or-later. All combinations of required/optional installation checks. */
#include <assert.h>
#include "dependencies.h"
int main(void) {
    for(unsigned available=0;available<64;available++) {
        unsigned base=ZZ_MUI|ZZ_OS|ZZ_RTG|ZZ_BOARD|ZZ_IDLE;
        assert((zz_missing(available,0)==0)==((available&base)==base));
        assert((zz_missing(available,1)==0)==(available==63));
    }
    return 0;
}
