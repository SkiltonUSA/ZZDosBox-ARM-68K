# Witness lifecycle control

Six physical startup/shutdown runs passed with `GLIDEWITNESS=lifecycle`.
Each run performed the existing Exec allocation, eight-word nonce mapping,
Core1 startup, cooperative return, SCTLR restoration and owned-memory release.
No GPU query, submit or pixel-read command was issued. The compiled worker
instruction model passed all 18 cases, including an idle worker stopped without
GPU reads or writes. Both strict probe builds passed; build hashes are attached.

A diagnostic-only entry wrapper makes stdout unbuffered before entering the
shared launcher. Production launchers are unchanged. This changes diagnostic
logging timing and helps expose the last stage reached in a future failure.
The six successes do not rule out an intermittent launch or teardown fault,
and do not resolve earlier GPU completion/readback discrepancies or freezes.
No rendering performance gain is claimed.

The clock remained held at 800 MHz. Final status reported Core1 idle and 57.8 C.
Post-run free Fast RAM was unchanged across all six runs (616684112 bytes),
then 616790768 after deleting the temporary probe. The bridge responded and
the exclusive reservation was released. No firmware or production build changed.
