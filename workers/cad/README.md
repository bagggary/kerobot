# Offline STEP inspection

The standalone C++20 executable reads a trusted, local, single-file STEP solid assembly into an in-memory XDE document. It emits one JSON report after successful traversal. It shares no code path with draft generation, motion or release. Requires Qt Core and OCCT 7.8.1. The optional shell separately uses Qt Widgets.

From the repository root, with the development packages on the CMake search path, run the following commands.

```sh
cmake -S . -B build-cad -DKEROBOT_BUILD_STEP_PROBE=ON -DKEROBOT_BUILD_DESKTOP=ON -DBUILD_TESTING=ON
cmake --build build-cad --config Release
ctest --test-dir build-cad -C Release --output-on-failure
./build-cad/kerobot_step_probe fixtures/cad/t-coupon.step
```

For the Visual Studio generator use `build-cad/Release/kerobot_step_probe.exe`. Enable only the STEP option for headless import. Qt and OCCT can be located via `CMAKE_PREFIX_PATH`. Runtime libraries must be reachable by the executable.

The report binds its source SHA-256 to exactly the bytes passed to OCCT. Each leaf has a revision-local XDE definition ID, a root-to-occurrence path, a world-from-definition 3×3 matrix and translation in metres, and the world B-rep bounding box in metres. IDs belong to one import and are unsuitable as stable references across revisions. A diagnostic failure returns nonzero and produces no JSON report. OCCT messages go to stderr.

The tested corpus is one AP242 millimetre assembly with rigid placements and single solid leaves. The probe checks explicit SI millimetres on supported shape representation contexts, recognized external references, STEP model failures, transfer success, solid validity, finite bounds and traversal limits. Input is capped at 8 MiB, traversal at 1,024 nodes and 32 path entries. Separate process execution and these limits do not bound kernel memory or CPU during transfer. Use reviewed local fixtures only. Adversarial-input containment, complete transfer/healing diagnostics, mirrored-part handling, exporter coverage, persistent topology IDs, viewport selection and integration with planning remain open.
