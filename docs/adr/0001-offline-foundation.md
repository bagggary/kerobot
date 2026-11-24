# ADR 0001 — Offline C++ foundation

Accepted for this increment, 24 November 2026. Product architecture remains proposed.

Use dependency-free C++20 for the first executable contract/geometry slice. Qt 6 Widgets and OCCT XDE remain the desktop/CAD target. The optional Qt shell is a status window, not a CAD viewport; it has not been built here. Do not substitute a mesh or synthetic line for STEP import evidence. Production edge remains proposed .NET, not this file-backed draft demonstration.

Internal lengths are metres; boundaries accept explicit m or mm only. Fixture seam lies along X; side is a fixed fixture Y sign independent of travel. Reversing travel reverses tangent, not the selected physical side. No quaternion or torch work/travel angle is computed in this increment. Later T_A_B maps B into A, right-handed quaternion [x,y,z,w]. Freeze pose conventions with independent left/right oracles before implementing orientation.

Draft IDs use a restricted alphabet. Atomic directory reservation prevents two writers from overwriting the same JOBID. Exact replay succeeds; differing bytes or incomplete reservations fail closed. This does not implement durable fsync, signed artifacts, symlink-resistant storage, an edge ledger, or release authorization. Store in a trusted local directory only. Physical execution is absent.

Protobuf describes proposed v1 boundaries; there are no generated bindings or RPC servers. The executable JSON draft schema is the implemented interchange subset. WPS values are test data; numeric limits are synthetic software bounds, not qualified welding settings.
