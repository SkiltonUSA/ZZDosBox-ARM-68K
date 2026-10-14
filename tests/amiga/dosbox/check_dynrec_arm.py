#!/usr/bin/env python3
"""Check the separately built ARMv7 recompiler in the instruction model.

This does not establish physical cache coherency or game speed; run the
generated-code witness and game benchmark on the ZZ9000 as well.
"""
import argparse,json
from pathlib import Path

import check_pc_arm as pc


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--out', type=Path, default=Path('.context/amiga/dosbox-dynrec'))
    args = parser.parse_args()
    pc.OUT = args.out.resolve()
    build=json.loads((pc.OUT/'build.json').read_text())
    assert build.get('dynrec'), 'Build the optional dynrec target first'
    # Profile mode supplies the model clock, also used by the optional Doom
    # observer. The actual execution/graphics expectations are shared with
    # the interpreter suite.
    for mode in ('cpu', 'smc', 'paging', 'fixture', 'copy-stop', 'guest-bound',
                 'read-stop', 'fs-stop', 'boot', 'mouse', 'hdd'):
        pc.run(0x12000040, mode, True, options=(8, 0, 3000))
    if build.get('scanline_test'):
        for mode in ('scan-full','scan-skip','scan-transition','scan-stop','scan-soft','scan-gradient'):
            pc.run(0x12000040,mode,True,options=(8,0,3000))
        for mode in ('scan-skip','scan-soft','scan-gradient'):
            pc.run(0x150020c0,mode,True,options=(8,0,3000))
    if build.get('presentation_test'):
        results=[pc.run(0x12000040,'fixture',True,options=(8,0,3000),presentation=mode) for mode in (0,2,1)]
        assert len({(r['source'],r['guest_ms'],r['output']) for r in results})==1,results
        print('PASS identical guest EGA/PIT output, source frames and emulated time across presentation modes',flush=True)
        pc.run(0x150020c0,'fixture',True,options=(8,0,3000),presentation=2)
        pc.run(0x12000040,'present-stop',True,options=(8,0,3000),presentation=2)
        for mode in ('mouse','midi','resize','scan-transition'):
            if mode!='scan-transition' or build.get('scanline_test'):
                pc.run(0x12000040,mode,True,options=(8,0,3000),presentation=2)
    if build.get('neon'):
        pc.run(0x12000040, 'neon-denied', True, options=(8, 0, 3000))
    for mode in ('cpu', 'smc'):
        pc.run(0x150020c0, mode, True, options=(8, 0, 3000))


if __name__ == '__main__':
    main()
