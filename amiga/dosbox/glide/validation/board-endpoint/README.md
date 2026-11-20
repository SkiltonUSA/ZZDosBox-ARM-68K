# Board endpoint evidence

- `register-results.json`: nine register scenarios, eleven endpoint scenarios,
  and detection of a deliberately truncated address decoder.
- `register-regression.json`: original register regression and mutation checks.
- `cdc-results.json`: six cases through the generated M09 AXI clock converter
  and AMD XPM implementation. Source hashes include the local vendor inputs.
- `topology.json`: existing address map, PS7 and renderer settings preserved;
  candidate endpoint clock/reset, response stream and error/drain connections.
- `staging.json`: source inventory and staged source hashes.
- `source-rejection.json`: extra/changed renderer files and nested output rejected.
- `integration-messages.txt`: retained Vivado warnings and validation marker.

The full Vivado logs and generated project stay local. No proprietary AMD IP,
bitstream or installed firmware is included. These results establish neither
physical GPU completion nor gameplay FPS. See [the report](../../experimental/board-endpoint.md).
