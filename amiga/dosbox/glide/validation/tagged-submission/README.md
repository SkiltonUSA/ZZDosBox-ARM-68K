# Tagged submission preparation evidence

- `results.json`: validator, helper, fixture and runner identities; sanitizer
  and compile-only scope.
- `test.log`: accepted scenes, bounds, private-copy behavior and rejected client
  control attempts.
- `commands.hex`: output matching the existing ordinary full-renderer generator
  byte for byte; contains five frames, each ending with the firmware-added tag.
- `legacy-regression.log` and `legacy-validation.json`: unchanged-validator
  regression after sharing its source extraction helper.
- `soft.log` / `softfp.log`: successful ARM compile logs (empty output).

No physical transport or framebuffer measurement is included. See the
[integration contract](../../experimental/tagged-submission.md).
