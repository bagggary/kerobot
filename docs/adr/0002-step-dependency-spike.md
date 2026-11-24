# Offline STEP and Qt dependency spike

24 November 2026 · development evaluation

Use OCCT 7.8.1 XDE to retain the distinction between assembly occurrences and shared definitions. Qt Core supplies JSON, Unicode file paths and hashing in the standalone probe. Qt Widgets supplies the separate status window. The default C++20 core has neither dependency. Use shared libraries during this spike. The exact OCCT CMake version protects this tested API and toolkit layout while the dependency evaluation is open.

| Environment | Configuration | Evidence |
|---|---|---|
| Debian 13 x86-64 | GCC 14.2, CMake 4.4.5, Debian Qt 6.8.2 and OCCT 7.8.1 | Both optional targets built. Headless shell startup and XDE corpus exercised. |
| Windows x64 | Candidate Qt 6.8.2, OCCT 7.8.1, MSVC 2022 Release, matching 64-bit dependency prefix | conda-forge lists both package versions for win-64. Native branch jobs must demonstrate the combined build and corpus. |
| macOS Apple silicon | Candidate Qt 6.8.2, OCCT 7.8.1, Xcode/Apple Clang on macOS 15 arm64 | Both package versions listed for osx-arm64. Native branch jobs must demonstrate the combined build and corpus. |

Qt documents Windows x64/MSVC 2022 and macOS arm64 configurations for its 6.8 series. Matching architecture, deployment target, shared-library search paths and Qt platform plugins matter at packaging time. The existing status shell has no AIS viewport. A build or offscreen smoke test cannot establish picking, DPI behavior or GPU performance on a nominated workstation. [Qt 6.8 platform matrix](https://doc.qt.io/qt-6.8/supported-platforms.html) · [Deployment](https://doc.qt.io/qt-6.8/deployment.html) · [OCCT package inventory](https://api.anaconda.org/release/conda-forge/occt/7.8.1) · [Qt package inventory](https://api.anaconda.org/release/conda-forge/qt6-main/6.8.2)

## Redistribution review

Qt offers commercial terms and open-source terms that vary by module. Evaluate Qt Base Core, Gui, Widgets and the platform plugin actually bundled. For an LGPLv3 route, inventory licenses and notices, provide corresponding library sources or a compliant offer and preserve the recipient's ability to replace the libraries. Review how those rights work with signed Windows and macOS packages. Dynamic linking is one design choice. It does not by itself complete the obligations. [Qt licensing](https://doc.qt.io/qt-6.8/licensing.html) · [Qt LGPL guidance](https://www.qt.io/development/open-source-lgpl-obligations)

OCCT is distributed under LGPL 2.1 with an additional exception for material incorporated from headers. Review the exception, notices, library source availability and other applicable LGPL obligations against the exact build. Inventory transitive libraries and plugins separately. Before distributing installers, the owner must choose the Qt license route, review the package SBOM and rights, and validate replacement, signing and relocation on clean machines. This spike supplies no installer or redistribution approval. [OCCT license and exception](https://occt3d.com/dev/doc/overview/html/occt_public_license.html)

The Linux evidence used Debian development packages extracted to a private prefix. Debian's absolute OCCT CMake paths were rebased in that temporary prefix. No dependency binaries or modified package metadata are shipped. Package availability on each OS is separate from a successful build, an independently reviewed geometry result and permission to redistribute.
