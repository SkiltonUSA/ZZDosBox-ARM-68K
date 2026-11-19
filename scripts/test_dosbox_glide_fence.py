#!/usr/bin/env python3
"""Test upstream RasterIX parser fence semantics; no FPGA or Amiga access."""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / '.context/amiga/glide/fence-model'


def test_completion(simulate, parser):
    design = ROOT / 'amiga/dosbox/glide/experimental/completion.v'
    bench = ROOT / 'tests/amiga/dosbox/glide/completion.v'
    rc, text = simulate(parser, 'completion', bench, 'completion_test', (design,))
    if rc or text.count('PASS completion:') != 9:
        raise SystemExit('Completion mailbox scenarios failed')
    original = design.read_text()
    mutations = (
        ('completion-stale-idle', 'if (pending) begin', 'if (pending || drained) begin',
         'Premature acknowledgement'),
        ('completion-no-drain', 'if (drained) begin', "if (1'b1) begin",
         'Premature acknowledgement'),
        ('completion-wrong-epoch', 'ack_epoch <= token_epoch;', 'ack_epoch <= token_sequence;',
         'Wrong or unstable acknowledgement'),
        ('completion-error-leak', 'assign ack_valid = result_valid && !fault && !bus_error;',
         'assign ack_valid = result_valid;', 'Error allowed stale result consumption'),
        ('completion-lost-cancel', 'if (cancel) ack_cancelled <= 1;',
         'if (cancel) ack_cancelled <= 0;', 'Wrong or unstable acknowledgement'),
    )
    for name, old, new, failure in mutations:
        if original.count(old) != 1:
            raise SystemExit('Unexpected completion design; review mutation: ' + name)
        mutated = OUT / (name + '.v')
        mutated.write_text(original.replace(old, new))
        rc, text = simulate(parser, name, bench, 'completion_test', (mutated,))
        if not rc or failure not in text:
            raise SystemExit('Test did not detect mutation: ' + name)
        print('PASS mutation: ' + name)
    return {'scope': 'Experimental mailbox with upstream parser; synthetic drain/error inputs, no MMIO or CDC',
            'scenarios': 9, 'mutations_detected': [m[0] for m in mutations],
            'sources': {str(x.relative_to(ROOT)): hashlib.sha256(x.read_bytes()).hexdigest()
                        for x in (design, bench)}}


def test_registers(simulate):
    mailbox = ROOT / 'amiga/dosbox/glide/experimental/completion.v'
    design = ROOT / 'amiga/dosbox/glide/experimental/completion_axil.v'
    bench = ROOT / 'tests/amiga/dosbox/glide/completion_axil.v'
    rc, text = simulate(mailbox, 'completion-axil', bench, 'completion_axil_test', (design,))
    if rc or text.count('PASS AXI mailbox:') != 9:
        raise SystemExit('AXI-Lite mailbox scenarios failed')
    original = design.read_text()
    mutations = (
        ('axil-no-identity', '(&expected_valid) && expected_epoch==ack_epoch && expected_sequence==ack_sequence',
         '(&expected_valid)', 'Wrong or unstable write response'),
        ('axil-half-write', 'aw_held && w_held && !s_bvalid',
         '(aw_held || w_held) && !s_bvalid', 'Response before complete AXI write'),
        ('axil-stale-staging', '     expected_valid<=0;',
         '     expected_valid<=expected_valid;', 'Wrong or unstable write response'),
        ('axil-moving-snapshot', 'if(s_rvalid && s_rready)s_rvalid<=0;',
         'if(s_rvalid && s_rready)s_rvalid<=0;\n   if(s_rvalid && !s_rready)s_rdata<=read_data;',
         'AXI read snapshot changed while stalled'),
    )
    for name, old, new, failure in mutations:
        if original.count(old) != 1:
            raise SystemExit('Unexpected AXI-Lite design; review mutation: ' + name)
        mutated = OUT / (name + '.v')
        mutated.write_text(original.replace(old, new))
        rc, text = simulate(mailbox, name, bench, 'completion_axil_test', (mutated,))
        if not rc or failure not in text:
            raise SystemExit('Test did not detect mutation: ' + name)
        print('PASS mutation: ' + name)
    return {'scope': 'Single-clock AXI-Lite register wrapper and mailbox; injected token/drain/error, no CDC or board address',
            'scenarios': 9, 'mutations_detected': [m[0] for m in mutations],
            'sources': {str(x.relative_to(ROOT)): hashlib.sha256(x.read_bytes()).hexdigest()
                        for x in (mailbox, design, bench)}}


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--rtl', type=Path, required=True)
    p.add_argument('--iverilog', default='iverilog')
    p.add_argument('--vvp', default='vvp')
    p.add_argument('--ivl-dir', type=Path)
    p.add_argument('--axi', action='store_true', help='Also test bypass Coalescer write retirement')
    p.add_argument('--coalesced', action='store_true', help='Also test merged bursts at 32 and 64 bits')
    p.add_argument('--texture', action='store_true', help='Also test cached texture reads and replacement')
    p.add_argument('--completion', action='store_true', help='Also test experimental tagged completion mailbox')
    p.add_argument('--registers', action='store_true', help='Also test experimental AXI-Lite completion registers')
    a = p.parse_args()
    OUT.mkdir(parents=True, exist_ok=True)
    (OUT / 'validation.json').unlink(missing_ok=True)
    source = a.rtl.resolve() / 'CommandParser.v'
    definitions = a.rtl.resolve() / 'RegisterAndDescriptorDefines.vh'
    bench = ROOT / 'tests/amiga/dosbox/glide/fence.v'
    compiler = [a.iverilog]
    runtime = [a.vvp]
    if a.ivl_dir:
        compiler += ['-B', str(a.ivl_dir.resolve())]
        runtime += ['-M', str(a.ivl_dir.resolve())]

    def simulate(path, name, testbench=bench, top='fence_test', extra=(), flags=()):
        binary = OUT / (name + '.vvp')
        build = subprocess.run(compiler + list(flags) + ['-g2012', '-s', top, '-I', str(a.rtl.resolve()),
                                  '-o', str(binary), str(testbench), str(path)] +
                       [str(x) for x in extra], capture_output=True, text=True)
        build_log = (build.stdout + build.stderr).replace(str(ROOT) + '/', '')
        (OUT / (name + '-compile.txt')).write_text(build_log)
        if build.returncode:
            raise SystemExit(build_log)
        result = subprocess.run(runtime + [str(binary)], capture_output=True, text=True)
        text = (result.stdout + result.stderr).replace(str(ROOT) + '/', '')
        (OUT / (name + '.txt')).write_text(text)
        print(text, end='')
        return result.returncode, text

    rc, text = simulate(source, 'upstream')
    if rc or text.count('PASS parser fence:') != 4:
        raise SystemExit('Upstream fence scenarios failed')
    # Prove the test observes transaction retirement, not just command decoding.
    original = source.read_text()
    condition = 'if (!framebufferWritesPending && !textureMemoryTransactionsPending)'
    if original.count(condition) != 1:
        raise SystemExit('Unexpected upstream parser; review before mutation')
    mutated = OUT / 'without-wait.v'
    mutated.write_text(original.replace(condition, "if (1'b1)"))
    rc, text = simulate(mutated, 'without-wait')
    if not rc or 'Passed fence with transactions pending' not in text:
        raise SystemExit('Test did not detect removed retirement wait')
    print('PASS mutation: removing retirement wait is detected')
    record = {'scope': 'CommandParser simulation with injected pending signals; not full AXI or installed FPGA',
              'scenarios': 4, 'removed_wait_detected': True,
              'sources': {str(x.relative_to(ROOT)) if x.is_relative_to(ROOT) else x.name:
                          hashlib.sha256(x.read_bytes()).hexdigest()
                          for x in (source, definitions, bench, Path(__file__))}}
    if a.axi:
        coalescer = a.rtl.resolve() / 'Coalescer.v'
        tracker = a.rtl.resolve() / 'ValueTrack.v'
        axi_bench = ROOT / 'tests/amiga/dosbox/glide/fence_axi.v'
        rc, text = simulate(source, 'axi-upstream', axi_bench, 'fence_axi_test',
                            (coalescer, tracker))
        if rc or text.count('PASS AXI fence:') != 5:
            raise SystemExit('AXI write retirement scenarios failed')
        original = coalescer.read_text()
        condition = '| outstanding_write_active;'
        if original.count(condition) != 1:
            raise SystemExit('Unexpected bypass tracker; review before mutation')
        mutated = OUT / 'without-response-tracking.v'
        mutated.write_text(original.replace(condition, ';'))
        rc, text = simulate(source, 'without-response-tracking', axi_bench,
                            'fence_axi_test', (mutated, tracker))
        if not rc or 'Passed fence with transactions pending' not in text:
            raise SystemExit('Test did not detect removed response tracking')
        print('PASS mutation: removing AXI response tracking is detected')
        record['axi'] = {'scope': 'Parser plus bypass Coalescer and ValueTrack; simulated AXI slave',
                         'scenarios': 5, 'removed_response_tracking_detected': True,
                         'sources': {x.name: hashlib.sha256(x.read_bytes()).hexdigest()
                                     for x in (coalescer, tracker, axi_bench)}}
    if a.coalesced:
        testbench = ROOT / 'tests/amiga/dosbox/glide/fence_coalesced.v'
        dependencies = [a.rtl.resolve() / (name + '.v') for name in
                        ('Coalescer', 'CoalesceAddrGen', 'CoalesceFiFo', 'ValueTrack', 'sfifo')]
        for width in (32, 64):
            rc, text = simulate(source, f'coalesced-{width}', testbench,
                                'coalesced_fence_test', dependencies,
                                (f'-Pcoalesced_fence_test.DATA_WIDTH={width}',))
            if rc or text.count('PASS coalesced fence:') != 9:
                raise SystemExit(f'Coalesced write scenarios failed at {width} bits')
        fifo = dependencies[2].read_text()
        mutations = (
            ('coalesced-without-response-tracking', '| outstanding_write_active;', ';',
             'Passed fence with transactions pending'),
            ('coalesced-corrupt-pixel', 'm_mem_axi_wdata <= fifo_wdata;',
             "m_mem_axi_wdata <= fifo_wdata ^ 1'b1;", 'Corrupt data at pixel'),
        )
        for name, old, new, failure in mutations:
            if fifo.count(old) != 1:
                raise SystemExit('Unexpected upstream FIFO; review before mutation')
            mutated = OUT / (name + '.v')
            mutated.write_text(fifo.replace(old, new))
            sources = dependencies[:2] + [mutated] + dependencies[3:]
            rc, text = simulate(source, name, testbench, 'coalesced_fence_test', sources)
            if not rc or failure not in text:
                raise SystemExit('Test did not detect mutation: ' + name)
            print('PASS mutation: ' + name)
        record['coalesced'] = {
            'scope': 'Parser plus complete Coalescer and simulated AXI slave; not full GPU',
            'scenarios': 18, 'widths': [32, 64], 'mutations_detected': [m[0] for m in mutations],
            'sources': {x.name: hashlib.sha256(x.read_bytes()).hexdigest()
                        for x in dependencies + [testbench]}}
    if a.texture:
        testbench = ROOT / 'tests/amiga/dosbox/glide/fence_texture.v'
        names = ('StreamConcatFifo', 'TextureCacheDirectMapped', 'TextureCacheDirectMappedContext',
                 'TextureCacheDirectMappedController', 'TextureMMU', 'TextureReaderContext',
                 'TextureReaderController', 'axis_broadcast', 'sfifo', 'ValueTrack')
        dependencies = [a.rtl.resolve() / (name + '.v') for name in names]
        reader = a.rtl.resolve() / 'TextureReader.v'
        original = reader.read_text()
        # Icarus rejects constants as output destinations. These two bits are
        # unused; connect them to unused wires without changing datapath logic.
        old = ".m_stream_tdata({\n            1'b0,\n            1'b0,"
        declaration = '    wire                         fifoReady;'
        if original.count(old) != 1 or original.count(declaration) != 1:
            raise SystemExit('Unexpected TextureReader output wiring; review adaptation')
        adapted = original.replace(declaration, declaration + '\n    wire [1:0] unusedFifoOutputs;')
        adapted = adapted.replace(old, '.m_stream_tdata({\n            unusedFifoOutputs[1],\n            unusedFifoOutputs[0],')
        compatible = OUT / 'TextureReader-compatible.v'
        compatible.write_text(adapted)
        for width in (32, 64):
            rc, text = simulate(source, f'texture-{width}', testbench, 'texture_fence_test',
                                dependencies + [compatible],
                                (f'-Ptexture_fence_test.DATA_WIDTH={width}',))
            if rc or text.count('PASS texture fence:') != 8:
                raise SystemExit(f'Cached texture scenarios failed at {width} bits')
        mutations = (
            ('texture-premature-retirement',
             'm_axi_rvalid & m_axi_rready & m_axi_rlast', 'm_axi_rvalid & m_axi_rready',
             1, 'Lost outstanding texture read'),
            ('texture-stale-cache', '.invalidate(s_axis_tvalid)', ".invalidate(1'b0)",
             2, 'Stale or corrupt texture'),
        )
        for name, old, new, count, failure in mutations:
            if adapted.count(old) != count:
                raise SystemExit('Unexpected texture logic; review mutation: ' + name)
            mutated = OUT / (name + '.v')
            mutated.write_text(adapted.replace(old, new))
            rc, text = simulate(source, name, testbench, 'texture_fence_test', dependencies + [mutated])
            if not rc or failure not in text:
                raise SystemExit('Test did not detect mutation: ' + name)
            print('PASS mutation: ' + name)
        record['texture'] = {
            'scope': 'Cached TextureReader and parser; synthetic pixel stage and AXI slave, not full GPU',
            'scenarios': 16, 'widths': [32, 64], 'mutations_detected': [m[0] for m in mutations],
            'adaptation': 'Two unused FIFO output constants replaced with unused wires for Icarus',
            'sources': {x.name: hashlib.sha256(x.read_bytes()).hexdigest()
                        for x in dependencies + [reader, compatible, testbench]}}
    if a.completion:
        record['completion'] = test_completion(simulate, source)
    if a.registers:
        record['registers'] = test_registers(simulate)
    (OUT / 'validation.json').write_text(json.dumps(record, indent=2) + '\n')


if __name__ == '__main__':
    main()
