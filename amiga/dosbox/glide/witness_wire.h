/* Host command, release flag and ARM reply each occupy separate cache lines.
 * Only the existing Exec-owned mapped allocation contains this mailbox. */
#ifndef GRX_WITNESS_WIRE_H
#define GRX_WITNESS_WIRE_H
#include "protocol.h"
#include "layout.h"
#include "probe_scene.h"
#define GW_CMD (ZZ_CONTROL+0x800)
#define GW_GO (ZZ_CONTROL+0x880)
#define GW_REPLY (ZZ_CONTROL+0x900)
#define GW_EARLY (ZZ_CONTROL+0x940)
enum {GW_SEQ=0,GW_NONCE=4,GW_SCENE=8,GW_MARKER=12,GW_OFFSET=16,GW_COUNT=20,GW_IDLE=24,GW_REVERSE=28,GW_SAMPLE=32,GW_PUBLISH=36};
enum {GW_ARMED=0,GW_DONE=4,GW_ERROR=8,GW_BAD=12,GW_CHECKED=16,GW_POLLS=20,GW_TICKS=24,GW_FIRST_OFFSET=28,GW_FIRST_GOT=32,GW_FIRST_WANT=36,GW_VERIFY_TICKS=40,GW_SAMPLES=44};
enum {GW_EARLY_SEQ=0,GW_EARLY_ERROR=4,GW_EARLY_KIND=8,GW_EARLY_SAMPLES=12};
#endif
