# Initial traceability

| Source requirement | Increment | Test/evidence | Remaining |
|---|---|---|---|
| Architecture §§3,7 explicit units | m/mm conversion, finite rejection | core tests 1–2 | STEP unit discovery/drawing check |
| §§3,5 confirmed intent / direction | confirmed flag, explicit side, tangent | analytic endpoint, side, reverse, unconfirmed tests | B-rep detector, UI, signed angles |
| §§7,8 distinct travel/WFS | separate named fields and bounds | speed-domain tests | approved immutable WPS and proven source map |
| §§5,7 job identity | immutable draft JOBID and replay/conflict | persistence/replay/incomplete tests | edge lease, ledger, durable events/hashes |
| §§5,9,12 no unintended motion | no live adapter; start throws | mock start test | native verification/HIL/site authority |
| Roadmap S1-03 cross-platform | CMake + CI matrix | see TEST_REPORT.md | both-OS accepted Qt/OCCT demonstration |

No source requirement is considered fully satisfied solely by these partial implementations.
