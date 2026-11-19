# Translator validation — 18 November 2026

| Check | Result |
| --- | --- |
| Native C/C++ with ASan and UBSan | Passed |
| Triangle coverage and RGB interpolation | Matched analytic reference at every pixel of a 64×64 fixture |
| Upper/lower origins and perspective texture coordinates | Passed, including rectangular texture extents |
| Opaque/alpha/additive register mapping | Passed |
| Invalid/overlapping pool spans, depth requests, NaN, clipping and degenerate geometry | Rejected as specified |
| Packet capacity and finalisation lifecycle | Passed; no partial triangle on rejection/full |
| Actual upstream XACP submit validator | Accepted 306 batches and ten offscreen fixtures; rejected depth enable in all 306 batches |
| GCC 16.2.0 Cortex-A9 ARM32 compilation | Soft-float and softfp/VFPv3 objects built |
| Unicorn 2.1.4 Cortex-A9 execution | 12 soft-float and 12 softfp batches bit-identical to native packet output |
| Physical ZZ9000 | Five offscreen fixtures pass final readback; earlier completion-marker failures remain under investigation |
| DOS game / FPS | Not tested |

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

8. Built a separate 68k offscreen probe with generated wire fixtures, avoiding a
   floating-point dependency on the 68k. Tested clear, solid, texture and blending
   through Core0 on the physical ZZ9000. Preserved failed runs as well as passes.
9. Compared scissored-memset and same-state triangle markers, two framebuffer
   heights, and logging placement. Added pre-submit marker and firmware CRC
   checks. Final guarded runs pass, but the earlier failure is not isolated.
10. Re-ran sanitizer, firmware-validator and A9 instruction-model checks. Saved
    source/build hashes, raw logs and actual framebuffer captures. No runtime or
    release promotion.
11. Reproduced the marker failure by disabling the pre-submit read. Fresh marker
    colours and triangle markers did not cure it. Cache-only preparation failed;
    read-only and full preparation passed their recorded series. Added tested
    marker retagging and packet CRC diagnostics; retained all negative results in
    the [isolation report](marker-isolation/README.md).

12. Added a read-only Core1 witness and a shared pixel oracle. Recorded 49
    physical runs with idle/delayed observer controls; one scene disagreed between
    ARM and 68k readback. Preserved the failure and confounders in the
    [Core1 report](core1-witness/README.md). Six compiled-worker model checks pass.

13. Completed 72 rotated preparation-control runs and eight first-read trace
    runs after restoring the Mac bridge. Unrelated reads fail; yield and busy-wait
    controls pass. Incorrect samples retain prior-scene colours. Recorded the
    [results](preparation-controls/README.md) without promoting a production wait.

14. Compared ordered and concurrent observers, then reverse ARM traversal:
    45 completed runs, 178 scenes and one interrupted run. Preserved all outcomes
    in the [observer-order report](observer-order/README.md). Nine worker model
    checks pass; hardware testing stopped with cleanup unverified.

15. After reboot, eight [early-sample runs](early-samples/README.md) completed
    cleanly. Two concurrent runs captured new ARM pixels and previous-scene
    host pixels at identical offsets; all four ordered controls passed. Twelve
    compiled-worker model cases pass. No production fence promoted.

See the [hardware report](hardware/README.md) for pixel evidence and its limits.
This is not proof of Glide compatibility, audible quality or game performance.
