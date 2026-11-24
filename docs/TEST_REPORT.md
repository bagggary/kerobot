# Sprint 01 local verification

24 November 2026. Source revision: the implementation commit containing this report; immutable Git commit/tree binds source and report. CI results are separate evidence.

Host: macOS 14.5 build 23F79, Apple silicon arm64. Compiler: Apple Clang 16.0.0 (clang-1600.0.26.6). C++20. CMake 3.31.6, Ninja 1.11.1.3. Python 3.14.6, jsonschema 4.23.0. Tools installed in an isolated local task environment.

| Check | Actual result |
|---|---|
| CMake configure and Ninja build, default offline targets | PASS |
| Core build with -Wall -Wextra -Wpedantic -Werror | PASS |
| C++ case runner | 16 passed, 0 failed |
| CTest | 2/2 passed (core suite + persisted demo) |
| Python JSON Schema/CLI integration | 4/4 passed |

The C++ suite checks m/mm conversion; unknown/nonfinite units; analytic endpoints/midpoint; side and reversed tangent; missing confirmation; unsupported/degenerate/nonfinite/overflow geometry; side/sample bounds; non-test WPS rejection; distinct speed domains; invalid IDs; non-release labeling; idempotent persistence, changed/incomplete draft refusal; nonfinite serialization; and mock start refusal.

The integration suite parses actual CLI output and persisted JSON, validates draft schema, compares all 11 points against an analytic oracle, verifies replay, refuses tampering, and rejects release/unit/speed/ID mutations. Analytic expected coordinates are independently expressed in fixtures/expected, but have not received independent human QA approval.

Reproduce with README commands. Windows and macOS GitHub Actions are configured; do not infer their results from local execution. Qt shell, Protobuf compilation, OCCT/STEP, signed installers, detector, torch orientation, IK/clearance, vision, native verification, HIL and physical welding were not tested or implemented by this increment. Passing software tests does not close original M00–M14.

## CI follow-up

Implementation commit f0d63586b0de488bc5dace8a9a9003a299d4d6eb: Windows configure/build/CTest/JSON integration passed in [run 45417361723](https://github.com/bagggary/kerobot/actions/runs/45417361723). The macos-14 job was cancelled for unsupported runner configuration. A redundant initial workflow had the same retired image problem. Follow-up consolidates to ci.yml, selects macos-15 and updates actions to Node 24 versions. GitHub's [official runner image table](https://github.com/actions/runner-images/blob/main/README.md) documents retirement of macOS 14 and supported macOS 15 labels. Follow-up run must pass before claiming both hosted platforms.
