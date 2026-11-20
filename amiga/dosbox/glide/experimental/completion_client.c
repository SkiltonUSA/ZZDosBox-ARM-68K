#include "completion_client.h"

enum { STATUS=0, EPOCH=4, SEQUENCE=8, EXPECT_EPOCH=12,
       EXPECT_SEQUENCE=16, CONSUME=20 };
enum { VALID=1, FAULT=4, CANCELLED=8 };

grx_completion_state grx_completion_poll(const grx_completion_io *io,
                                        grx_completion_batch *batch)
{
    uint32_t status, epoch, sequence;
    grx_completion_state result;
    if (batch->state != GRX_COMPLETION_PENDING) return batch->state;

    if (!io->read(io->context, STATUS, &status) || (status & FAULT)) goto quarantine;
    if (!(status & VALID)) return GRX_COMPLETION_PENDING;
    if (!io->read(io->context, EPOCH, &epoch) ||
        !io->read(io->context, SEQUENCE, &sequence) ||
        !io->read(io->context, STATUS, &status)) goto quarantine;
    if ((status & FAULT) || !(status & VALID) ||
        epoch != batch->epoch || sequence != batch->sequence) goto quarantine;
    result = (status & CANCELLED) ? GRX_COMPLETION_CANCELLED : GRX_COMPLETION_RETIRED;

    if (!io->write(io->context, EXPECT_EPOCH, epoch) ||
        !io->write(io->context, EXPECT_SEQUENCE, sequence) ||
        !io->write(io->context, CONSUME, 1)) goto quarantine;
    /* A queued OKAY write response can predate a GPU fault. With exclusive
     * submission ownership, successful consumption must leave an empty,
     * nonfaulted mailbox. Never release resources on the write response alone. */
    if (!io->read(io->context, STATUS, &status) || status != 0) goto quarantine;
    batch->state = result;
    return result;

quarantine:
    batch->state = GRX_COMPLETION_QUARANTINED;
    return batch->state;
}
