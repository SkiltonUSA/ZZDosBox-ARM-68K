"""Zynq boot-container checks adapted from the ZZUAE Core0 packager.

Payload equality checks only; this does not validate hardware behaviour.
"""
import hashlib
import struct

BOOT_HEADER_BYTES = 0x8c0


def sha(data):
    return hashlib.sha256(data).hexdigest()


def partitions(data):
    def word(offset):
        return struct.unpack_from("<I", data, offset)[0]
    if word(0x20) != 0xaa995566 or word(0x24) != 0x584c4e58:
        raise ValueError("Not a Zynq boot image")
    if sum(struct.unpack_from("<11I", data, 0x20)) & 0xffffffff != 0xffffffff:
        raise ValueError("Invalid BootROM checksum")
    count = word(word(0x98) + 4)
    first = word(0x9c)
    if not 1 <= count <= 16 or first + count * 64 > len(data):
        raise ValueError("Invalid partition table extent")
    result = []
    for index in range(count):
        w = struct.unpack_from("<16I", data, first + index * 64)
        if sum(w) & 0xffffffff != 0xffffffff:
            raise ValueError("Invalid partition checksum")
        length, offset = w[0] * 4, w[5] * 4
        if not length or w[0] != w[1] or w[0] != w[2] or offset + length > len(data):
            raise ValueError("Unsupported encrypted, padded or truncated partition")
        result.append((w[3], w[4], w[6], data[offset:offset+length]))
    return result


def elf_segments(data):
    if data[:7] != b"\x7fELF\x01\x01\x01":
        raise ValueError("Expected little-endian ELF32")
    header = struct.unpack_from("<HHIIIIIHHHHHH", data, 16)
    if header[0:2] != (2, 40) or header[3] != 0x100000 or header[8] != 32:
        raise ValueError("Expected Cortex-A9 firmware executable at 0x00100000")
    result = []
    ranges = []
    for index in range(header[9]):
        kind, offset, va, pa, size, mem, flags, alignment = struct.unpack_from(
            "<8I", data, header[4] + index * 32)
        if kind != 1 or size == 0:
            continue
        if pa != va or offset + size > len(data) or size > mem or size % 4:
            raise ValueError("Unsupported firmware segment")
        if not (0x100000 <= pa and pa + mem <= 0x1f0000) and not (pa == 0x3fcf0000 and mem == 8192):
            raise ValueError("Firmware segment exceeds its reserved area")
        if any(pa < end and start < pa + mem for start, end in ranges):
            raise ValueError("Overlapping firmware segments")
        ranges.append((pa, pa + mem))
        result.append((pa, header[3] if not result else 0, 0x10, data[offset:offset+size]))
    # Depending on alignment, GNU ld merges the low code/data LOAD segments.
    # Both layouts must retain the entry payload and the complete boot ROM.
    if (len(result) not in (2, 3) or result[0][0] != header[3] or
            result[-1][0] != 0x3fcf0000 or len(result[-1][3]) != 8192):
        raise ValueError("Expected low firmware payload(s) and the 8 KiB boot ROM")
    return result


def wrap_bitstream(raw):
    # Only restore the .bit container header and big-endian word presentation.
    # Bootgen converts it back; payload equality is required below.
    def pas(value):
        return struct.pack(">H", len(value)) + value
    header = pas(bytes.fromhex("0ff00ff00ff00ff000")) + b"\x00\x01"
    for key, value in ((b"a", b"preserved-XX19d-RIX\0"), (b"b", b"7z020clg400\0"),
                       (b"c", b"2026/11/16\0"), (b"d", b"00:00:00\0")):
        header += key + pas(value)
    return header + b"e" + struct.pack(">I", len(raw)) + b"".join(
        raw[i:i+4][::-1] for i in range(0, len(raw), 4))



def bit_payload(data):
    """Read a 7z020clg400 .bit file as Bootgen's little-endian FPGA payload."""
    position = 0

    def take(count):
        nonlocal position
        if position + count > len(data):
            raise ValueError('Truncated bitstream container')
        value = data[position:position + count]
        position += count
        return value

    def short_field():
        return take(struct.unpack('>H', take(2))[0])

    if short_field() != bytes.fromhex('0ff00ff00ff00ff000') or take(2) != b'\x00\x01':
        raise ValueError('Invalid bitstream header')
    for key in b'abcd':
        if take(1) != bytes([key]):
            raise ValueError('Unexpected bitstream field')
        value = short_field()
        if key == ord('b') and value.rstrip(b'\x00') != b'7z020clg400':
            raise ValueError('Bitstream targets a different FPGA')
    if take(1) != b'e':
        raise ValueError('Missing bitstream payload')
    size = struct.unpack('>I', take(4))[0]
    payload = take(size)
    if not size or size % 4 or position != len(data):
        raise ValueError('Invalid bitstream extent')
    return b''.join(payload[i:i+4][::-1] for i in range(0, size, 4))
