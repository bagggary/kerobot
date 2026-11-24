# Sprint 01 implementation ledger

24 November 2026. This starts the sprint; it does not satisfy its full definition of done.

| Item | Actual evidence / state |
|---|---|
| S1-01 discovery | Source baseline recovered; actual sanitized STEP, approved WPS, native routine, options and surveyed fixture remain blocked |
| S1-02 contracts | C++ subset + draft JSON Schema implemented; proposed Protobuf boundary; units, left/right, reversed travel and error tests run locally; human review pending |
| S1-03 build/UI | CMake/presets and Windows/macOS CI configured; Qt status shell and OCCT probe built on Linux; [dependency/platform/license spike](adr/0002-step-dependency-spike.md); target-OS evidence tracked per revision |
| S1-04 STEP | Separate C++ XDE probe traverses nested occurrences and measures solid B-reps on TC-01; representative CAD, viewport and picking pending |
| S1-05 path | Tested synthetic X-line sampling and explicit side/tangent only; no joint detector, orientation, IK or clearance |
| S1-06 WPS/jobs | Test-only value validation, mock start refusal and file-backed draft replay/conflict implemented; approved WPS management/edge SQLite pending |
| S1-07 acceptance | Local C++/JSON evidence; both-OS review, mirror/corrupt STEP corpus and signed QA pending |

- [x] Preserve architecture and roadmap as source snapshots
- [x] Create private repository
- [x] Start runnable offline foundation and negative tests
- [x] Exercise one synthetic AP242 assembly against drawing-derived dimensions and occurrence frames on Linux
- [ ] Review and freeze v1 contracts with technical lead and QA
- [ ] Import nominated STEP assembly and verify drawing dimensions/occurrences
- [ ] Qt viewport, face selection, explicit seam confirmation UI
- [ ] Both-OS native build/demo and independent acceptance

Original M00–M14 remain unchecked in the preserved roadmap. Partial software evidence is not acceptance of those larger milestones. Numeric physical tolerances, WPS/machine binding, native verification, HIL and safety/site gates remain unapproved.
