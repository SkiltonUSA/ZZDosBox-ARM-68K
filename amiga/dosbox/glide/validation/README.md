# Translator validation — 18 November 2026

| Check | Result |
| --- | --- |
| Native C/C++ with ASan and UBSan | Passed |
| Triangle coverage and RGB interpolation | Matched analytic reference at every pixel of a 64×64 fixture |
| Upper/lower origins and perspective texture coordinates | Passed, including rectangular texture extents |
| Opaque/alpha/additive register mapping | Passed |
| Invalid/overlapping pool spans, depth requests, NaN, clipping and degenerate geometry | Rejected as specified |
| Packet capacity and finalisation lifecycle | Passed; no partial triangle on rejection/full |
| Actual upstream XACP submit validator | Accepted 306 batches; rejected depth enable in all 306 |
| GCC 16.2.0 Cortex-A9 ARM32 compilation | Soft-float and softfp/VFPv3 objects built |
| Unicorn 2.1.4 Cortex-A9 execution | 12 soft-float and 12 softfp batches bit-identical to native packet output |
| Physical ZZ9000 / DOS game / FPS | Not tested |

The firmware test uses the validator extracted from XACP commit
`5516effbad59acf6bef44ea1cc3ca4393c8ec9e0`, `firmware/source/XX19d-RIX/arm/ZZ9000_proto.sdk/ZZ9000OS/src/main.c`.
Hashes are recorded in [native.json](native.json) and
[arm-model.json](arm-model.json). Generated binaries and copied firmware test
source remain outside Git.

## Work log

1. Read the supplied Glide 3.0 guide, including window-space vertices, texture
   coordinates and origin semantics. PDF SHA-256:
   `3a1101111e7434d9123f33227562c1493bee60b6f19e6608754ee420d84d7efb`.
2. Checked DOSBox-X's interception architecture, the XACP submit header and
   actual firmware parser. Found the Core0 ABI's deliberate depth rejection.
3. Read Xanxi's existing librixgpu descriptor and transport code. Kept the
   tested descriptor layout; did not import direct MMIO or fixed clock timing.
4. Wrote the native behaviour tests first; the first build failed because the
   translator did not yet exist. Implemented the standalone encoder, then
   passed coverage, perspective, rejection and bounded-packet checks.
5. Compiled the real firmware validator separately and checked all 306 batch
   combinations. Firmware source is test input, not linked into the encoder.
6. Built A9 soft-float and VFP variants. The instruction model caught a host/ARM
   enum-storage difference in the configuration structure; changed origin and
   blend storage to explicit 32-bit fields. Both models then produced matching
   packets for every fixture.
7. Documented limitations and the hardware/guest integration gates. No Amiga
   connection, firmware change, installation or release promotion occurred.

This is correctness evidence for a command encoder, not proof of Glide
compatibility, rendered GPU pixels, audible quality or performance.
