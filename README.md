# KERobot

Private development baseline for an autonomous robotic welding platform. **Sprint 01 has started; this is an offline synthetic prototype, not a welding system.**

Original [architecture](docs/source/KERobot_Welding_Platform_Architecture.md) and [MVP roadmap](docs/source/MVP_Implementation_Roadmap.md) are preserved with source hashes in [manifest](docs/source/manifest.json). Read the [Sprint ledger](docs/SPRINT_01.md), [test report](docs/TEST_REPORT.md), [cloud handoff](docs/HANDOFF.md) and [release gates](docs/safety/RELEASE_GATES.md).

## Run the offline prototype

Requires a C++20 compiler and CMake 3.24+. No Qt/OCCT dependency is needed for the default build.

```sh
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build --config Debug
ctest --test-dir build -C Debug --output-on-failure
```

macOS: `./build/kerobot_offline build/my-drafts`

Windows (Visual Studio generator): `.\build\Debug\kerobot_offline.exe build/my-drafts`

The demo creates JOB-SYNTHETIC-001 with an explicitly synthetic 100 mm X-aligned seam, test-only WPS, 11 line samples, side/tangent and `release_eligible:false`. Identical replay succeeds; a changed or incomplete draft under the same JOBID is refused. The demo never reads STEP or starts hardware. Trusted local draft directory required; crash recovery is manual, with no durability/production ledger claim.

```sh
python -m pip install -r requirements-test.txt
python tests/test_offline.py build/kerobot_offline
# Windows: python tests/test_offline.py build/Debug/kerobot_offline.exe
```

## Implemented and proposed

| Capability | Current state |
|---|---|
| C++20 contract/geometry core and CLI | Implemented and locally tested |
| SI conversion, explicit intent, synthetic side/direction | Implemented subset; no CAD detector |
| Test WPS, mock refusal, draft persistence | Implemented and locally tested; no approved WPS/edge service |
| JSON schema and Protobuf | JSON tested; Protobuf proposed, not generated |
| Windows/macOS CI | Configured; results must be checked per revision |
| Qt status shell | Builds and starts offscreen on Linux; interactive workstation validation pending |
| Standalone OCCT XDE probe | Tested on Linux with a synthetic AP242 assembly; reports occurrence transforms and bounds. [Run the probe](workers/cad/README.md) |
| CAD viewport and face picking | Pending |
| Torch orientation, IK, collision, scan replay | Pending |
| RAPID/native verification, scanner, controller | Pending; hardware disabled |

The directories follow the roadmap monorepo structure. Placeholder modules contain explicit status documents. See [ADR 0001](docs/adr/0001-offline-foundation.md) for the deliberately limited prototype and target stack.

No release, motion, arc, scanner or network controller interface exists. Milestone acceptance requires the evidence and accountable reviewers in the original roadmap. No license grant is included pending the owner's licensing decision and dependency redistribution review.
