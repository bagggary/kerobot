**Engineering execution baseline · 24 November 2026 · Revision 0.1**

Source: KERobot_Welding_Platform_Architecture.md (23 November 2026), especially §§ 3–12 and 15; recent autonomous welding product requirements (CAD-driven programming, scanning, digital twin, cross-platform desktop and multi-vendor ambition). This page translates the proposed baseline into actionable work, not a certification or a report of existing software.

## 1. Scope and status

**MVP target:** one guarded ABB IRB 4600-20/2.50 / IRC5 / Güdel track / Lincoln Power Wave R450 cell; track parked at a verified station; one fixture family, straight single-pass T-fillet, one approved WPS, fixed arc-off scanner. Operator confirms weld intent, reviews and approves exact native artifact; authorized local operator starts at commissioned controls. Simulation-first milestone uses recorded or synthetic data and mock devices, with no live motion.

| Capability | Proposed MVP | Evidence status today |
|---|---|---|
| STEP / joint selection | Exact B-rep assembly, candidate T-joints and human confirmation | Proposed; no code/test evidence supplied |
| Path / WPS | Single stringer, signed work/travel angles, approved machine mapping | Proposed; no KERobot validation |
| Vision | Replay then fixed pre-weld scan, bounded correction | Proposed; sensor unselected |
| Simulation | Generic twin plus exact native verification | Proposed; matching tooling unverified |
| Controller | RAPID export, controlled staging, local start, monitoring | Candidate hardware; integration unverified |
| Multi-brand, arc-on tracking, coordinated rail, multi-pass, curves | Separate future qualifications | Deferred / disabled |

**Status policy:** Proposed → Implemented (merged code + build) → Tested offline (test report + revision) → Native verified (matching vendor tool + artifact hash) → HIL verified (actual hardware witness) → Site accepted / released (signed limits). A feature is never promoted merely because a design exists. Existing physical equipment or previous standalone welding trials do not prove KERobot functionality. All milestone boxes start unchecked.

## 2. Architecture and technology decisions

| Module | Recommended stack | Windows x64 | macOS Apple silicon |
|---|---|---|---|
| Desktop / viewport | Qt 6 Widgets, C++, OCCT AIS | Native | Native |
| CAD worker | OCCT XDE/STEP, CMake | Native | Native |
| Planning / twin | Eigen, validated IK, FCL; OMPL later if needed | Native | Native |
| Vision replay | Open3D C++ CPU, native captures + portable point clouds | Native | Native |
| Edge orchestration | Supported .NET LTS, Protobuf/gRPC, HTTPS artifacts, SQLite | Pilot industrial service | Client/mock only for MVP |
| Vendor verification | RobotStudio + matching RobotWare VC | Supported Windows host, license dependent | Review evidence remotely |
| Controller / scanner | Isolated ABB PC SDK bridge if option supported; vendor scanner SDK | Windows edge subject to exact SDK matrix | No direct vendor SDK assumption |
| Packaging | CMake pinned dependencies, signed MSI, notarized app, SBOM | CI + clean install | CI + clean install |

Use STEP AP242 preferentially, AP214/AP203 only as demonstrated. IGES repair later; STL/OBJ obstacles, glTF previews, never mesh-derived weld intent without qualification. Choose geometric algorithms before ML. Confirm redistribution rights, OS/SDK support and versions in spikes, not by assumption. No cloud or desktop process in safety or servo timing. Offline job authoring allowed; authoritative release belongs to cell edge.

## 3. Dependency graph and contracts

```text
P0 Contracts / cell discovery
  ├─ P1 CAD import → P2 joint intent ─┐
  ├─ P3 WPS / machine mapping ───────┼→ P4 nominal plan
  ├─ P5 vision replay / calibration ─┘       ↓
  └─ Cell model / mock adapter ──→ corrected plan → P6 twin + native verification
                                             ↓
                                   P7 staging / hardware pilot
                                             ↓
                                   P8 site release / hardening
```

P5 can develop in parallel against frozen schemas; P6 generic twin can start with nominal paths; real acquisition and live interfaces wait for approved hardware access. Cross-platform UI and edge storage evolve throughout.

Core contracts: PartRevision → GeometryArtifact → JointIntent; WPSRevision + MachineScheduleMap; CellRevision + FrameGraph + CalibrationRevision; CaptureSet → Registration → CorrectionProfile; PlanRevision → ProgramArtifact → VerificationReport → ReleaseManifest; Job → RunAttempt → SegmentEvent → WeldRecord → Inspection. SI internally; explicit shop units at boundaries; right-handed quaternion [x,y,z,w], T_A_B maps B to A; distinguish flange/TCP, linear/rotary rail, WFS/travel speed. Immutable hashes, revision IDs, typed errors, capability manifests, idempotency keys, durable events and one edge execution lease. Any relevant setup change invalidates release.

## 4. Practical implementation phases

| Phase / owner | Build deliverables | Acceptance and evidence |
|---|---|---|
| P0 Discovery / lead, controls, quality | Nominate part, WPS, reference RAPID, fixture survey, controller options, risk register, frozen v1 contracts, baseline pendant timings | Signed intended use, test plan and tolerance owners; unknowns recorded, hardware blocked |
| P1 STEP / CAD lead | Isolated importer with limits, XDE occurrences, units, transforms, B-rep, display tessellation, picking, SHA-256 | Golden assembly dimensions and occurrences match drawings on both OSs; corrupt/oversize/ambiguous units fail or require review; mirrored instances correct |
| P2 Joint detection / geometry lead | Face adjacency/proximity, planar T-fillet candidates, endpoints, side/direction, trim/reject, revision-scoped identity | Compare to independently annotated joints; no contact-only automatic weld intent; mirrored, gapped, ambiguous and revised parts require confirmation |
| P3 WPS / welding authority + backend | Immutable draft/released/withdrawn WPS, qualification reference, material/thickness/position limits, process phases, explicit units, cell-specific schedule mapping | Out-of-range values and unsupported source modes rejected; unapproved/withdrawn WPS blocks release; WFS and travel speed never conflated; real map requires witness |
| P4 Path / robotics lead | Seam frame, direction-aware work/travel angles, CTWD, approach/weld/retract/transfers, sequence, continuous IK, limit/singularity checks | Analytic golden endpoints and orientation for both sides; continuous valid branch and bounded clearance; unreachable or singular path rejected; no fixed 45° override of WPS |
| P5 Vision / metrology lead | Replay first; datum-seeded registration, face/seam fit, endpoints, uncertainty, coverage, clamps; fixed scanner acquisition after selection | Held-out metrology; approved error budget; symmetry, occlusion, drift, out-of-range fit-up and unknown clearance fail closed; corrected path regenerated and revalidated |
| P6 Simulation / robotics + independent QA | Measured robot/tool/fixture/track twin, swept clearance, native RAPID exporter from known-good routine, RobotStudio verification | Exact artifact checks for frames, configs, speed, blends, arc phases and external group; native discrepancies within signed tolerances; generic preview not sufficient |
| P7 Controller pilot / controls + integrator | Capability manifest, mock then isolated Windows bridge or witnessed manual transfer, stage/readback, local start, source feedback, run history | HIL and fault matrix pass, no accidental start or blind restart; arc-inhibited motion then supervised coupons; actual source schedule verified |
| P8 Hardening / QA, OT, site | Signed installs, auth, audit, backup/restore, offline operation, monitoring, training, FAT/SAT and shift trials | Signed SAT, coupon/first article quality, restore/rollback rehearsal, documented support and production limits |

Each phase exits by evidence rather than date. Numeric tolerances (CAD error, seam error, coverage, clearance, timing and quality) must be predeclared by the appropriate authority using representative parts and metrology; unset limits disable physical release. Measure manual vs assisted programming time, first-article acceptance, reject rate and intervention count.

## 5. Proposed monorepo layout

```text
kerobot/
  apps/desktop/                 # Qt shell, viewport, review
  libs/contracts/               # proto, schemas, generated bindings
  libs/geometry/                # frames, units, B-rep helpers
  workers/cad/                  # STEP import and joint candidates
  workers/planner/              # seam poses, IK, trajectory
  workers/vision/               # replay, registration, corrections
  libs/simulation/              # collision, clearance, twin
  services/edge/                # .NET jobs, releases, events, auth
  adapters/mock/                # offline device contracts
  adapters/abb/                 # RAPID post and isolated SDK bridge
  adapters/scanner/             # vendor acquisition boundary
  cell_profiles/pilot-01/       # sanitized versioned cell configuration
  tests/{unit,contract,golden,replay,native,hil,sat}/
  fixtures/{cad,scans,wps,expected}/
  docs/{adr,requirements,safety,commissioning}/
  packaging/{windows,macos}/
  .github/workflows/            # native CI, license/SBOM checks
  CMakeLists.txt
  CMakePresets.json
```

Human technical lead owns integration and shared contracts; CODEOWNERS per module. No live cell credentials in repository or agent access. Keep proprietary binaries and sensitive backups in approved storage; reference hashes. Independent QA owns golden expected outputs; schema and fixture changes require consumer review. Use small PRs, reproducible dependency locks, native CI and artifact manifests.

## 6. First development sprint — 10 working days (proposed)

**Sprint objective:** a runnable Windows/macOS offline foundation: import one T-joint STEP assembly, pick faces, confirm seam and show a nominal line; attach a clearly labeled test WPS and persist a JOBID. No real scanner, native arc execution or production claim. This combines contract-first discovery with a small vertical slice; full architecture first-30-day demo remains a later target.

| ID / days | Task and accountable role | Deliverable / acceptance |
|---|---|---|
| S1-01 / 1–2 | Lead + controls + welding | Collect sanitized CAD/drawing, reference RAPID, WPS evidence, controller options and fixture information; record blockers |
| S1-02 / 1–3 | Lead + QA | ADRs for Qt/OCCT, units/frames, seam identity and job states; v1 schemas and analytic fixture oracle |
| S1-03 / 2–4 | Build + desktop | CMake presets, Qt shell, CI on Windows x64 and Apple silicon; reproducible clean build |
| S1-04 / 3–6 | CAD | Import XDE assembly, show tree and viewport, select faces; drawing dimension and transforms verified |
| S1-05 / 5–8 | Geometry + planner | Straight T-joint candidate, manual side/direction confirmation, nominal line preview; left/right golden tests |
| S1-06 / 5–8 | Backend + welding | Test-only WPS schema, unit validation, mock adapter and persisted job draft; unsupported values rejected |
| S1-07 / 9–10 | QA + lead | Both OS demo, corrupt/units/mirror negative cases, report hashes and open issues; no release eligibility |

Definition of done: reviewed PRs, clean native builds, reproducible test fixtures, independent expected results, documented limitations, demo and evidence links. If a real approved WPS or representative CAD is unavailable, use explicitly synthetic fixtures and keep industrial validation blocked. No task marked done based on generated code alone.

## 7. Industrial safety and release gates

| Gate | Accountable approval | Required evidence |
|---|---|---|
| G0 Intended use / risk | Integrator + EHS + welding authority | Hazard log, risk assessment, safety requirements, response matrix, isolation/maintenance and responsibility allocation |
| G1 Cell + WPS | Integrator + welding engineer | Surveyed frames/TCP with independent witness, fixed rail station assurance, fixture, approved WPS and proven source mapping |
| G2 Exact program | Authorized engineering reviewer | Confirm joint side, calibration, coverage, corrections, swept clearance, native artifact validation, signed immutable release |
| G3 Local run | Local operator + controller/PLC | Staged identity/readback, fresh setup, matching part and station, process permissives, validated protective controls, deliberate local start |
| G4 Physical / quality | Integrator + welding engineer + quality + site owner | Arc-inhibited dry run, HIL fault proofs, coupons and first article meeting governing code, SAT and signed envelope |

Independent safety PLC/controller owns protective stops, guarding, E-stop, monitored limits and restart behavior per risk assessment. UI collision visualization and ordinary camera/heartbeat are not personnel-protection functions. No desktop remote start; reset only restores eligibility. Faults after ignition enter indeterminate/disposition until reconciled; no automatic retry of an uncertain weld. Preserve completed/suspect/untouched segments; quality approves repairs. Include arc light, extraction, fire, electrical, laser, pneumatic, track, dress pack and clamp hazards. Validate worst-case stopping and safeguarding through integrator.

Standards applicability follows source §9: Saudi SASO machinery requirements; ISO 12100, ISO 10218-1/-2, ISO 13849-1 or IEC 62061, IEC 60204-1, IEC 60825-1 where relevant, ISO 15609-1/15614-1/14732 or AWS D1.1 where contractually applicable, IEC 62443 and NIST SP 800-82. Site specialists must confirm editions, conformity route, code and required PL/SIL; no compliance claim here. Maintain clause-to-test traceability.

## 8. Living milestone checklist

- [ ] M00 — Intended use, exact tuple, responsible integrator, risk register and evidence plan approved (P0/G0)

- [ ] M01 — v1 contracts and native Windows/macOS CI builds verified (Sprint 1)

- [ ] M02 — STEP import and selection golden corpus passes both OSs (P1)

- [ ] M03 — Straight T-joint candidate and side/direction tests pass (P2)

- [ ] M04 — WPS validation and approved machine schedule mapping proven (P3)

- [ ] M05 — Nominal path and continuous IK/clearance tests pass (P4)

- [ ] M06 — Replay registration, rejection and uncertainty tests pass (P5)

- [ ] M07 — End-to-end offline accepted/rejected jobs demonstrated both OSs (simulation MVP)

- [ ] M08 — Exact RAPID artifact verified with matching RobotWare (P6/G2)

- [ ] M09 — Scanner selected and independently calibrated; cell survey and WPS approved (G1)

- [ ] M10 — Staging/readback and HIL fault suite witnessed (P7)

- [ ] M11 — Safety controls and local start procedure commissioned (G3)

- [ ] M12 — Arc-inhibited dry runs and coupon/first-article inspection accepted (G4)

- [ ] M13 — Signed SAT, training, backup/restore, rollback and production envelope (P8)

- [ ] M14 — Expansion feature separately prioritized and qualified (future)

**Maintenance protocol:** at each PR/sprint review, update milestone status, owner, date, commit/build hash, test ID, evidence location, reviewer, limits and next blocker. Check a box only when its stated evidence is independently accepted. Reopen affected milestone on changed firmware, fixture, scanner, WPS, calibration, postprocessor or OS. Suggested issue states: Backlog / In progress / Review / Blocked / Evidence accepted; milestone checklist is a living page, not automatic telemetry.

## 9. Open decisions and immediate blockers

| Decision / missing evidence | Owner | Effect |
|---|---|---|
| Exact IRC5 RobotWare/options, PC Interface entitlement, reference RAPID and I/O | Controls | Blocks real adapter/native qualification |
| Actual R450 feeder, firmware, mode package and WPS machine binding | Welding + controls | Blocks arc-on |
| Scanner SDK, steel finish coverage, working distance, ground truth | Vision/metrology | Blocks physical correction; replay can continue |
| Track station assurance, TCP/frame witness, clamp/dress pack geometry | Integrator | Blocks motion release |
| Signed quality, clearance and uncertainty limits; governing code | Quality + integrator | Blocks physical release |
| OS version support, license review, approved OT network and backup objectives | IT/lead | Blocks installer/site acceptance |

## 10. Source traceability and next review

Architecture file §§ 5–8 support technology, contracts and hardware boundaries; §§ 9 and 12 support gates and tests; §§ 10–11 and 15 support sequencing and agent ownership. Recent product ambitions for multi-vendor robots, adaptive paths and monitoring remain roadmap items beyond the narrow pilot. Proposed additions here are the P0–P8 work breakdown, monorepo directories, ten-day sprint, issue IDs and checklist governance. These are engineering recommendations, not verified implementation facts. Start S1-01 and S1-02 before expanding modules. Review at sprint end with the technical lead and independent QA.
