"""Patch the pinned BSP abort dispatch without changing unrelated exceptions."""
def patch_vectors(source):
    start = source.index('DataAbortHandler:')
    end = source.index('PrefetchAbortHandler:', start)
    body = source[start:end]
    old = '\tbl\tDataAbortInterrupt\t\t/*DataAbortInterrupt :call C function here */'
    if body.count(old) != 1:
        raise ValueError('Unexpected BSP data-abort handler')
    new = '''\tmov r0, sp
\tmrc p15, 0, r1, c5, c0, 0
\tmrc p15, 0, r2, c6, c0, 0
\tmrs r3, spsr
\tbl grx_mmio_abort
\tcmp r0, #0
\tbne .Lgrx_abort_return
''' + old + '\n.Lgrx_abort_return:'
    return source[:start] + body.replace(old, new) + source[end:]
