"""Read the actual XX19d-RIX submit validator for offline integration tests."""
import re


def validator_source(path):
    text = path.read_text()
    start = text.index('typedef struct {\n    uint32_t color_base;')
    end = text.index('static int xacpgpu_fifo_send_be', start)
    defines = '\n'.join(re.findall(r'^#define XACPGPU_.*$', text, re.M))
    return defines, text[start:end]
