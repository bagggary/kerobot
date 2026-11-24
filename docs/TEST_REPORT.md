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

## Cloud continuation on Debian 13 x86-64

Baseline `f0d63586b0de488bc5dace8a9a9003a299d4d6eb` was reconstructed from verified Git objects in a running Linux workspace. Parent `7c018e8b0c23f2e937ee2333de4e443762844ab0`, root tree `6ed1395b792aa07dcc13c8374bfacf9367de3290` and all three preserved SHA-256 source hashes matched. A clean default build reproduced 16 C++ cases, 2 CTest checks and 4 Python integration cases.

GitHub's push job records on this baseline showed Windows passing in both workflows, Ubuntu passing in the foundation workflow, and both macOS 14 jobs cancelled before steps ran. Both overall runs concluded cancelled. GitHub retired the macOS 14 Actions image on 2 November 2026. [Foundation CI](https://github.com/bagggary/kerobot/actions/runs/45417361723) · [Native jobs](https://github.com/bagggary/kerobot/actions/runs/45417361741) · [Runner retirement](https://github.com/actions/runner-images/issues/13518)

The continuation tree containing this section was built clean with GCC 14.2, CMake 4.4.5, Qt 6.8.2 and OCCT 7.8.1. Both optional targets compiled. The Qt status process remained alive with the offscreen plugin until the two-second smoke harness terminated it. No visual inspection or viewport test was performed.

| Check | Result on the continuation tree |
|---|---|
| C++ case runner and existing draft integration | 16/16 C++ and 4/4 Python passed |
| CTest with both optional targets enabled | 3/3 passed; includes STEP process suite |
| STEP suite run directly | 8/8 passed. Drawing-derived dimensions, shared definitions, paths, nested rotation/translation, hash and SI output. Refused missing, malformed, truncated, oversize, unsupported-unit, mixed-unit and missing-length inputs. |

These results apply to the synthetic fixture and the tested Linux build. Independent technical/QA review, customer drawing reconciliation, all industrial acceptance gates and the M00–M14 milestones remain open.
