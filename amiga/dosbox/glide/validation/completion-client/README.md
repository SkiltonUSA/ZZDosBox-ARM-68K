# Completion reader validation

`results.json` records the tested C reader, RTL, fixture and runner hashes.
`test.log` covers 20 scenarios. `mutant.log` shows the post-consume fault check
rejecting a reader with its final status observation removed. `restored-test.log`
records the restored reader passing again. `soft.log` and `softfp.log` are empty
successful ARM compile logs; the result records these as compile-only checks.

`firmware-source-review.json` identifies the inspected pinned MMIO and exception
sources. Those sources are not patched by this test.

See [the reader contract and limitations](../../experimental/completion-client.md).
This is not physical MMIO validation, firmware installation or an FPS result.
