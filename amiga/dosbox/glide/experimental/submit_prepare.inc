/* Experimental firmware integration unit. Include after the existing
 * xacpgpu_validate_submit definition; its validator and ABI stay unchanged.
 * Keep this packet in firmware-private storage, not the caller's shared stage.
 * Caller supplies the identity, serializes submission, and retains the packet
 * and resource references until verified retirement or recovery. */
/* Pinned XX19d-RIX gpu_cmd_fifo C_TX_FIFO_DEPTH; checked against the BD in tests. */
enum { GRX_TAGGED_FIFO_WORDS=512 };
typedef struct {
    uint32_t words;
    XACPGPU_SafeState next_state;
    uint32_t wire[GRX_TAGGED_FIFO_WORDS];
} GRX_TaggedSubmission;

static int grx_prepare_tagged(GRX_TaggedSubmission *packet,
                             const volatile uint32_t *source, uint32_t count,
                             uint32_t epoch, uint32_t sequence,
                             uint32_t *bad_index, uint32_t *detail)
{
    packet->words=0;
    /* One FIFO frame, including the firmware-only seven-word suffix. */
    if (count<3 || count>GRX_TAGGED_FIFO_WORDS-7) {
        *bad_index=0; *detail=count;
        return 0x4920;
    }
    for (uint32_t i=0;i<count;++i) packet->wire[i]=source[i];
    int error=xacpgpu_validate_submit(packet->wire,count,&packet->next_state,
                                      bad_index,detail);
    if (error) return error;
    /* Existing envelope is unchanged. Privileged operations are separate
     * envelopes in the same FIFO frame, just as in the renderer tests. */
    packet->wire[count+0]=be32(0x90000004);
    packet->wire[count+1]=0;
    packet->wire[count+2]=be32(0x60000000);
    packet->wire[count+3]=be32(0x50000008);
    packet->wire[count+4]=0;
    packet->wire[count+5]=be32(epoch);
    packet->wire[count+6]=be32(sequence);
    packet->words=count+7;
    return 0;
}
