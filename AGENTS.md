# KERobot continuation

Read README.md, docs/SPRINT_01.md, docs/HANDOFF.md and preserved source documents before editing.

- Distinguish proposed, implemented, locally tested, CI-tested, native-verified, HIL and site-accepted evidence.
- Keep this increment offline. No live controller/scanner clients, remote-start APIs, credentials or production release claims.
- Preserve original source snapshots; add design changes separately.
- Use C++20 core, Qt 6 Widgets/OCCT target, proposed .NET edge boundaries. Do not claim the scaffold implements these dependencies.
- Every geometry/WPS contract change needs meaningful negative tests and consumer review.
- Run cmake configure/build and ctest, plus Python JSON/schema integration tests when available. Record OS/compiler/revision and limitations.
- Sprint 01 next: OCCT dependency/license spike; sanitized STEP/drawing fixture and independent dimension/occurrence oracle; real import diagnostics; Qt assembly viewport and face selection; side/direction review. Do not mark M01/M02 complete before both required OSs and independent review.
