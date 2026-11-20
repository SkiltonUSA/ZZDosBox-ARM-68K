#!/usr/bin/env python3
"""Check boot parsing against a real image and reject damaged boot inputs."""
import argparse
import struct
from pathlib import Path
from glide_boot_image import partitions, bit_payload, wrap_bitstream


def rejected(function, data):
    try:
        function(data)
    except (ValueError, struct.error):
        return
    raise AssertionError('Damaged input was accepted')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--reference', type=Path, required=True)
    parser.add_argument('--bitstream', type=Path)
    parser.add_argument('--boot', type=Path)
    args = parser.parse_args()
    data = args.reference.read_bytes()
    original = partitions(data)
    assert len(original) == 5 and original[1][2] == 0x20
    bit = wrap_bitstream(original[1][3])
    assert bit_payload(bit) == original[1][3]
    for broken in (data[:1024], data[:0x48] + bytes([data[0x48] ^ 1]) + data[0x49:]):
        rejected(partitions, broken)
    for broken in (bit[:50], bit[:-1], bit + b'extra', bit.replace(b'7z020clg400', b'7z030clg400')):
        rejected(bit_payload, broken)
    if bool(args.bitstream) != bool(args.boot):
        parser.error('Use --bitstream and --boot together')
    if args.bitstream:
        assert bit_payload(args.bitstream.read_bytes()) == partitions(args.boot.read_bytes())[1][3]
        print('PASS actual Vivado bitstream versus Bootgen FPGA payload, including DMA padding')
    print('PASS reference partitions, FPGA word order, truncated/checksum/part/container rejection')


if __name__ == '__main__':
    main()
