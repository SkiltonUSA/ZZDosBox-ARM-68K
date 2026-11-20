#!/usr/bin/env python3
"""Stage an isolated Vivado project; never modifies the baseline or programs hardware."""
import argparse
import hashlib
import json
import shutil
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
EXP = ROOT / 'amiga/dosbox/glide/experimental'


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--fpga', required=True, type=Path)
    parser.add_argument('--renderer', required=True, type=Path)
    parser.add_argument('--output', required=True, type=Path)
    args = parser.parse_args()
    if any(args.output.resolve().is_relative_to(source.resolve())
           for source in (args.fpga, args.renderer)):
        parser.error('Output must be outside both input trees')
    hashes = json.loads((EXP / 'board-completion-sources.json').read_text())
    expected = json.loads((EXP / 'board-source-manifest.json').read_text())['source_sha256']
    for name, pair in hashes.items():
        expected[name.replace('/rtl/', '/', 1)] = pair['after']
    actual = {}
    for component in ('RasterIX', 'Float'):
        source = args.renderer / component / 'rtl'
        actual.update({component + '/' + str(p.relative_to(source)):
                       hashlib.sha256(p.read_bytes()).hexdigest()
                       for p in source.rglob('*') if p.is_file()})
    if actual != expected:
        raise SystemExit('Prepared renderer inventory or hash mismatch')
    args.output.mkdir(parents=True)
    shutil.copytree(args.fpga, args.output / 'fpga')
    rtl = args.output / 'fpga/RasterIX_G1/RasterIX/rtl'
    shutil.rmtree(rtl)
    shutil.copytree(args.renderer / 'RasterIX/rtl', rtl)
    top = rtl / 'RasterIX/RasterIX.v'
    original = top.read_text()
    text = original.replace('    // Performance signals', '''    // Experimental completion observation, in the GPU clock domain.
    output wire gpu_idle,
    output wire gpu_fault,

    // Performance signals''', 1)
    text = text.replace('    generate\n', '''    grx_axi_response_fault completionFault (
        .clk(aclk), .resetn(resetn),
        .bvalid(m_axi_bvalid), .bresp(m_axi_bresp),
        .rvalid(m_axi_rvalid), .rresp(m_axi_rresp), .fault(gpu_fault)
    );
    generate
''', 1)
    text = text.replace('            RasterIX_IF #(', '            assign gpu_idle = 0; // Completion is supported only for EF.\n            RasterIX_IF #(', 1)
    marker = '            ) rixef (\n'
    if text.count(marker) != 1:
        raise SystemExit('Unexpected RasterIX top')
    text = text.replace(marker, marker + '                .idle(gpu_idle),\n')
    top.write_text(text)
    target = args.output / 'completion'
    target.mkdir()
    for name in ['completion.v', 'completion_axil.v', 'completion_endpoint.v',
                 'response_token.v', 'axi_response_fault.v', 'integrate-board.tcl', 'synthesize-board.tcl']:
        shutil.copyfile(EXP / name, target / name)
    report = {'scope': 'Isolated project only; no bitstream or hardware access',
              'top_before_sha256': hashlib.sha256(original.encode()).hexdigest(),
              'top_after_sha256': hashlib.sha256(text.encode()).hexdigest(),
              'input_renderer_sha256': actual,
              'project_inputs_sha256': {str(p.relative_to(args.fpga)): hashlib.sha256(p.read_bytes()).hexdigest()
                                        for p in args.fpga.rglob('*')
                                        if p.is_file() and p.suffix in ('.xpr', '.bd', '.xdc')},
              'endpoint_sha256': {p.name: hashlib.sha256(p.read_bytes()).hexdigest()
                                  for p in target.iterdir()}}
    (args.output / 'staging.json').write_text(json.dumps(report, indent=2) + '\n')
    print('Staged isolated project for Vivado validation.')


if __name__ == '__main__':
    main()
