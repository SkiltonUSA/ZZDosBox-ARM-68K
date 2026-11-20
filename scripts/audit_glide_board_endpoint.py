#!/usr/bin/env python3
"""Check the generated endpoint topology against its baseline board design."""
import argparse
import hashlib
import json
from pathlib import Path


def connection(design, pin, interface=False):
    group, field = ('interface_nets', 'interface_ports') if interface else ('nets', 'ports')
    matches = [v[field] for v in design[group].values() if pin in v[field]]
    if len(matches) != 1:
        raise SystemExit('Missing or ambiguous connection: ' + pin)
    return sorted(matches[0])


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('--baseline', required=True, type=Path)
    p.add_argument('--candidate', required=True, type=Path)
    a = p.parse_args()
    old, new = [json.loads(p.read_text())['design'] for p in (a.baseline, a.candidate)]
    old_map, new_map = [d['addressing']['/processing_system7_0']['address_spaces']['Data']['segments'] for d in (old, new)]
    assert all(new_map[k] == v for k, v in old_map.items()), 'Existing address changed'
    assert set(new_map) - set(old_map) == {'SEG_grx_completion_0_reg0'}
    assert new_map['SEG_grx_completion_0_reg0'] == {
        'address_block': '/grx_completion_0/S_AXI/reg0', 'offset': '0x83C80000', 'range': '64K'}
    assert old['addressing']['/RasterIX_0'] == new['addressing']['/RasterIX_0'], 'GPU aperture changed'
    assert old['components']['processing_system7_0']['parameters'] == new['components']['processing_system7_0']['parameters'], 'PS7 settings changed'
    assert old['components']['RasterIX_0']['parameters'] == new['components']['RasterIX_0']['parameters'], 'Renderer settings changed'
    checks = [('grx_completion_0/aclk', 'processing_system7_0/FCLK_CLK0', False),
              ('grx_completion_0/aresetn', 'proc_sys_reset_0/peripheral_aresetn', False),
              ('ps7_0_axi_periph/M09_ACLK', 'processing_system7_0/FCLK_CLK0', False),
              ('ps7_0_axi_periph/M09_ARESETN', 'proc_sys_reset_0/peripheral_aresetn', False),
              ('grx_completion_0/gpu_idle', 'RasterIX_0/gpu_idle', False),
              ('grx_completion_0/gpu_fault', 'RasterIX_0/gpu_fault', False),
              ('grx_completion_0/S_AXI', 'ps7_0_axi_periph/M09_AXI', True),
              ('grx_completion_0/S_RESP_AXIS', 'RasterIX_0/m_cmd_resp_axis', True)]
    nets = {}
    for pin, target, interface in checks:
        nets[pin] = connection(new, pin, interface)
        assert target in nets[pin], 'Wrong endpoint wiring: ' + pin
    periph = new['components']['ps7_0_axi_periph']
    converter = periph['components']['m09_couplers']['components']['auto_cc']
    assert converter['vlnv'] == 'xilinx.com:ip:axi_clock_converter:2.1'
    report = {'scope': 'Generated topology, not physical operation or timing signoff',
              'baseline_sha256': hashlib.sha256(a.baseline.read_bytes()).hexdigest(),
              'candidate_sha256': hashlib.sha256(a.candidate.read_bytes()).hexdigest(),
              'existing_addresses_preserved': True, 'ps7_settings_preserved': True,
              'gpu_aperture_preserved': True, 'connections': nets,
              'converter': converter}
    print(json.dumps(report, indent=2))


if __name__ == '__main__':
    main()
