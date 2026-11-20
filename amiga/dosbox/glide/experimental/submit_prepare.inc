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

static int grx_finish_tagged(GRX_TaggedSubmission *packet, uint32_t count,
                             uint32_t epoch, uint32_t sequence,
                             uint32_t *bad_index, uint32_t *detail)
{
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

static int grx_prepare_tagged(GRX_TaggedSubmission *packet,
                             const volatile uint32_t *source, uint32_t count,
                             uint32_t epoch, uint32_t sequence,
                             uint32_t *bad_index, uint32_t *detail)
{
    packet->words=0;
    if (count<3 || count>GRX_TAGGED_FIFO_WORDS-7) {
        *bad_index=0; *detail=count;
        return 0x4920;
    }
    for (uint32_t i=0;i<count;++i) packet->wire[i]=source[i];
    return grx_finish_tagged(packet,count,epoch,sequence,bad_index,detail);
}

/* Two render envelopes, one retirement. Preserve their payload order and
 * commits. Validate the combined state transitions using the existing parser.
 * Copy before inspecting: the submitted bytes are the validated snapshot. */
static int grx_prepare_tagged_pair(GRX_TaggedSubmission *packet,
                                  const volatile uint32_t *left,uint32_t nl,
                                  const volatile uint32_t *right,uint32_t nr,
                                  uint32_t epoch,uint32_t sequence,
                                  uint32_t *bad_index,uint32_t *detail)
{
    packet->words=0;*bad_index=0;*detail=0;
    if(nl<3 || nr<3 || nl>GRX_TAGGED_FIFO_WORDS-7 || nr>GRX_TAGGED_FIFO_WORDS-7 ||
       nl+nr-2>GRX_TAGGED_FIFO_WORDS-7)return 0x4920;
    for(uint32_t i=0;i<nl;++i)packet->wire[i]=left[i];
    for(uint32_t i=0;i<nr;++i)packet->wire[nl+i]=right[i];
    uint32_t offsets[2]={0,nl},sizes[2]={nl,nr};
    for(unsigned i=0;i<2;++i){
        *bad_index=offsets[i];*detail=be32(packet->wire[offsets[i]]);
        if(*detail!=(0x90000000u|((sizes[i]-2)*4)))return 0x4921;
        *bad_index=offsets[i]+1;*detail=be32(packet->wire[*bad_index]);
        if(*detail)return 0x4922;
    }
    for(uint32_t i=2;i<nr;++i)packet->wire[nl+i-2]=packet->wire[nl+i];
    uint32_t count=nl+nr-2;
    packet->wire[0]=be32(0x90000000u|((count-2)*4));
    return grx_finish_tagged(packet,count,epoch,sequence,bad_index,detail);
}
