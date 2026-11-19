#!/usr/bin/env python3
"""Extract the pinned XX19d-RIX clock topology; no FPGA or board changes."""
import argparse
import hashlib
import json
from pathlib import Path

SHA256 = '09b9c8c3e960deab271c828d46d97ad6f151412d3e07046da92104b0c71f7867'
COMMIT = '6e59edbaf24e0d8d2f29cdd31667a10e16c5b283'
SOURCE = ('firmware/source/XX19d-RIX/fpga/XACPGPU_G4C/ZZ9000_proto/'
          'ZZ9000_proto.srcs/sources_1/bd/zz9000_ps/zz9000_ps.bd')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('block_design', type=Path)
    args = parser.parse_args()
    data = args.block_design.read_bytes()
    if hashlib.sha256(data).hexdigest() != SHA256:
        raise SystemExit('Different block design: review the source before applying this audit')
    design = json.loads(data)['design']
    components = design['components']
    periph = components['ps7_0_axi_periph']
    coupler = periph['components']['m08_couplers']

    def net(scope, pin, interface=False):
        group, field = ('interface_nets', 'interface_ports') if interface else ('nets', 'ports')
        matches = [(name, value[field]) for name, value in scope[group].items()
                   if pin in value[field]]
        if len(matches) != 1:
            raise SystemExit('Expected one connection for ' + pin)
        name, pins = matches[0]
        return {'name': name, 'pins': pins}

    clocks = {pin: net(design, pin) for pin in (
        'RasterIX_0/aclk', 'gpu_cmd_fifo/s_axi_aclk',
        'ps7_0_axi_periph/ACLK', 'processing_system7_0/M_AXI_GP1_ACLK')}
    resets = {pin: net(design, pin) for pin in (
        'RasterIX_0/resetn', 'gpu_cmd_fifo/s_axi_aresetn',
        'ps7_0_axi_periph/ARESETN')}
    paths = {pin: net(design, pin, True) for pin in (
        'processing_system7_0/M_AXI_GP1', 'gpu_cmd_fifo/S_AXI',
        'gpu_cmd_fifo/AXI_STR_TXD', 'RasterIX_0/m_axi',
        'processing_system7_0/S_AXI_HP3')}
    ps_parameters = components['processing_system7_0']['parameters']
    segments = design['addressing']['/processing_system7_0']['address_spaces']['Data']['segments']
    report = {
        'repository': 'Xanxi-Amiga/XACP-ZZ9000', 'commit': COMMIT,
        'source': SOURCE, 'sha256': SHA256,
        'scope': 'Published block-design connectivity only; not installed-bitstream or timing verification',
        'clocks': clocks, 'resets': resets, 'interfaces': paths,
        'clock_hz': {key: ps_parameters[key]['value'] for key in ('PCW_CLK0_FREQ', 'PCW_CLK1_FREQ')},
        'clock_converter': coupler['components']['auto_cc']['vlnv'],
        'converter_outer_connections': {
            pin: net(periph, pin) for pin in ('m08_couplers/S_ACLK', 'm08_couplers/M_ACLK',
                                            'm08_couplers/S_ARESETN', 'm08_couplers/M_ARESETN')},
        'converter_inner_connections': coupler['nets'],
        'peripheral_outputs': periph['parameters']['NUM_MI']['value'],
        'existing_gpu_registers': {key: value for key, value in segments.items() if 'gpu_' in key},
        'gpu_memory_segment': design['addressing']['/RasterIX_0'],
        'gpu_scalar_ports': sorted(components['RasterIX_0']['ports']),
        'command_response_ready': net(design, 'RasterIX_0/m_cmd_resp_axis_tready'),
        'command_response_interfaces': [name for name, value in design['interface_nets'].items()
                                        if 'RasterIX_0/m_cmd_resp_axis' in value['interface_ports']],
        'fifo_receive_enabled': components['gpu_cmd_fifo']['parameters']['C_USE_RX_DATA']['value'],
    }
    print(json.dumps(report, indent=2))


if __name__ == '__main__':
    main()
