#include "Vgrx_completion_axil.h"
extern "C" {
#include "amiga/dosbox/glide/experimental/completion_client.h"
}
#include <cstdio>
#include <cstdlib>

static void require(bool ok, const char *message)
{
    if (!ok) { std::fprintf(stderr, "FAIL %s\n", message); std::exit(1); }
}

struct Bus {
    Vgrx_completion_axil rtl;
    unsigned transactions = 0, fail_at = 0, fault_after = 0;
    void tick() {
        rtl.clk=0; rtl.eval(); rtl.clk=1; rtl.eval();
        rtl.clk=0; rtl.eval();
    }
    Bus() {
        rtl.resetn=0; tick(); tick(); rtl.resetn=1; tick();
    }
    void token(uint32_t epoch, uint32_t sequence) {
        require(rtl.token_ready, "token not ready");
        rtl.token_epoch=epoch; rtl.token_sequence=sequence;
        rtl.token_valid=1; tick(); rtl.token_valid=0; tick();
    }
    void drain() { rtl.drained=1; tick(); tick(); }
    void fault() { rtl.bus_error=1; tick(); }
    int finish(bool success) {
        ++transactions;
        if (transactions==fault_after) fault();
        return success && transactions!=fail_at;
    }
    int read(unsigned offset, uint32_t *value) {
        require(rtl.s_arready, "read address not ready");
        rtl.s_araddr=offset; rtl.s_arvalid=1; tick(); rtl.s_arvalid=0;
        require(rtl.s_rvalid, "read response missing");
        *value=rtl.s_rdata;
        bool ok=rtl.s_rresp==0;
        tick(); tick(); // Exercise retained read response under backpressure.
        require(rtl.s_rvalid && rtl.s_rdata==*value, "read response changed");
        rtl.s_rready=1; tick(); rtl.s_rready=0;
        return finish(ok);
    }
    int write(unsigned offset, uint32_t value) {
        rtl.s_awaddr=offset; rtl.s_wdata=value; rtl.s_wstrb=15;
        // Exercise both independent AXI write-channel arrival orders.
        if (transactions & 1) {
            require(rtl.s_awready, "write address not ready");
            rtl.s_awvalid=1; tick(); rtl.s_awvalid=0;
            rtl.s_wvalid=1; tick(); rtl.s_wvalid=0;
        } else {
            require(rtl.s_wready, "write data not ready");
            rtl.s_wvalid=1; tick(); rtl.s_wvalid=0;
            rtl.s_awvalid=1; tick(); rtl.s_awvalid=0;
        }
        tick();
        require(rtl.s_bvalid, "write response missing");
        bool ok=rtl.s_bresp==0;
        tick(); tick();
        rtl.s_bready=1; tick(); rtl.s_bready=0;
        return finish(ok);
    }
    grx_completion_io io() {
        return {this,
            [](void *p,unsigned a,uint32_t *v){return static_cast<Bus *>(p)->read(a,v);},
            [](void *p,unsigned a,uint32_t v){return static_cast<Bus *>(p)->write(a,v);}};
    }
};

static grx_completion_batch batch() { return {17,29,GRX_COMPLETION_PENDING}; }
static void expect(Bus &bus, grx_completion_batch &b, grx_completion_state state)
{
    auto io=bus.io();
    require(grx_completion_poll(&io,&b)==state,"wrong completion state");
    if (state!=GRX_COMPLETION_PENDING) {
        auto count=bus.transactions;
        require(grx_completion_poll(&io,&b)==state,"terminal state lost");
        require(count==bus.transactions,"terminal state retried hardware");
    }
}

int main()
{
    {
        Bus bus; auto b=batch();
        bus.rtl.drained=1; bus.tick();
        expect(bus,b,GRX_COMPLETION_PENDING); // Stale idle before any token.
        bus.rtl.drained=0; bus.token(17,29);
        for (unsigned i=0;i<20;++i) expect(bus,b,GRX_COMPLETION_PENDING);
        bus.drain(); expect(bus,b,GRX_COMPLETION_RETIRED);
        uint32_t status; require(bus.read(0,&status) && status==0,"result not consumed");
        std::puts("PASS completion client: idle, delayed drain, retirement and no retry");
    }
    {
        Bus bus; auto b=batch(); bus.token(17,29);
        require(bus.write(12,17) && bus.write(16,29) && bus.write(24,1),"cancel rejected");
        expect(bus,b,GRX_COMPLETION_PENDING); bus.drain();
        expect(bus,b,GRX_COMPLETION_CANCELLED);
        std::puts("PASS completion client: cancellation still waits for drain");
    }
    for (unsigned which=0;which<2;++which) {
        Bus bus; auto b=batch(); bus.token(which?17:18,which?30:29); bus.drain();
        expect(bus,b,GRX_COMPLETION_QUARANTINED);
        uint32_t status; require(bus.read(0,&status) && (status&1),"wrong identity consumed");
    }
    std::puts("PASS completion client: wrong epoch and sequence retained");
    {
        Bus bus; auto b=batch(); bus.token(17,29); bus.drain(); bus.fault();
        expect(bus,b,GRX_COMPLETION_QUARANTINED);
        std::puts("PASS completion client: existing fault quarantines");
    }
    for (unsigned n=1;n<=7;++n) {
        Bus bus; auto b=batch(); bus.token(17,29); bus.drain(); bus.fault_after=n;
        expect(bus,b,GRX_COMPLETION_QUARANTINED);
    }
    std::puts("PASS completion client: GPU fault between observations, including after consume");
    for (unsigned n=1;n<=8;++n) {
        Bus bus; auto b=batch(); bus.token(17,29); bus.drain(); bus.fail_at=n;
        expect(bus,b,GRX_COMPLETION_QUARANTINED);
        require(bus.transactions==n,"transport failure ignored or not reached");
    }
    std::puts("PASS completion client: failure at each of eight transactions");
    return 0;
}
