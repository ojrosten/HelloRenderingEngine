---
name: sequoia-windows-clang-toolchain
description: Assessed plan (2026-07-19) for a Windows + clang-front-end preset column — Route A (VS generator + ClangCL toolset) vs Route B (Ninja + standalone LLVM, GNU driver); Route B carries the coverage leg
metadata:
  type: project
---

Assessed 2026-07-19 at the user's request; not started. Question: what would a CMake workflow look like on Windows using the clang front end? Two routes, dictated by the existing preset architecture in sequoia's `build_system/CMakePresetsCommon.json` (`msvc` = VS generator, no compiler cache vars; `base-llvm` chain = GNU-driver clang flag world: `-fcolor-diagnostics`, clang `WARNING_SUPPRESSIONS`, env-composed `CMAKE_CXX_FLAGS` via `base-unix`, `base-asan-unix`'s `-fsanitize=address`).

**Route A — VS generator + ClangCL toolset** (two-line smoke-test column; VS-bundled clang, which lags LLVM):

```json
{
  "name": "windows-clang-cl",
  "displayName": "Visual Studio [ClangCL]",
  "generator": "Visual Studio 18 2026",
  "toolset": "ClangCL",
  "inherits": ["base-binary"]
}
```

Operationally identical to `msvc` (multi-config, no developer shell). Subtlety: CMake reports `CMAKE_CXX_COMPILER_ID=Clang`, `CMAKE_CXX_SIMULATE_ID=MSVC`, and **the `MSVC` variable is TRUE** — clang-cl silently inherits `if(MSVC)` branches (convenient for flags like `/W4`, a trap for anything genuinely cl-specific).

**Route B — Ninja + standalone LLVM (official installer/winget — the Windows analog of the Homebrew-LLVM policy), GNU driver** — the durable fourth toolchain column. `clang++` on Windows targets `x86_64-pc-windows-msvc` by default, uses MSVC STL, auto-detects VS/SDK (vcvars usually unnecessary; developer shell is belt-and-braces):

```json
{
  "name": "base-windows-llvm",
  "hidden": true,
  "cacheVariables": {
    "CMAKE_C_COMPILER": "C:/Program Files/LLVM/bin/clang.exe",
    "CMAKE_CXX_COMPILER": "C:/Program Files/LLVM/bin/clang++.exe"
  },
  "inherits": ["base-llvm"]
}
```

plus `windows-llvm{,-release,-asan,-coverage}` workflow trios mirroring the Mac set. Naming wrinkle: `base-llvm` inherits `base-unix`, a misnomer here not an obstacle (Ninja + env-composed flags work on Windows); consider renaming to e.g. `base-ninja-flags`.

**Why Route B pays rent — the coverage tie-in:** `clang++ -fprofile-instr-generate -fcoverage-mapping` works on the MSVC target, the driver links `clang_rt.profile` itself, and `llvm-cov export -format=lcov` emits union-pipeline-format tracefiles ([[sequoia-coverage-tooling]], clang/Mac union plan). The Windows box is the **Intel Arc** machine, so a `windows-llvm-coverage` leg brings the "permanent hardware-gated residue" (`CapabilitiesConfiguration.cpp` Arc compensation, Intel `message_id{2}`) into the union properly — **superseding the OpenCppCoverage one-off idea** in the MSVC assessment (that remains the only option for measuring *cl-compiled* code, immaterial for coverage purposes).

**Decision points before building either:**
- Audit where sequoia's build system keys flags off `MSVC`/compiler ID: Route A lands `MSVC==TRUE`, Route B `MSVC==FALSE` + `Clang` (same branch as Mac clang — least-surprise fit).
- **Type-name tidying will misfire on both routes** (finding F1, 2026-07-21 part-II addendum in `LLMs/Claude/reviews/sequoia_review_2026-07-16.md`): `Output.cpp`'s tidy dispatch keys on compiler, but under the MS ABI clang produces MSVC-format `typeid` names → clang massaging applied to MSVC text, and `demangle()`'s inner `#ifndef _MSC_VER` silently no-ops. Fix (union tidy pass, demangle keyed on ABI) should land before or with this column.
- The deliberate `cxx_std_23` Windows split: if keyed on `MSVC`, Route A stays 23, Route B gets 26 — decide intent (B at 26 matches the other platforms).
- ASan under Route B uses the `base-asan-unix` spelling as-is; the `clang_rt.asan_dynamic-x86_64.dll`-on-PATH gotcha ([[project-vulkan-branch-merges]]) applies to the standalone-LLVM runtime too.

**Recommendation as assessed:** Route A as a cheap "does clang's front end like our code against MSVC STL" check; Route B as the real column and the carrier of the Windows coverage leg. All pieces land in sequoia's build system — same upstream vehicle as the coverage fixes ([[sequoia-coverage-tooling]]). Programme summary: `LLMs/Claude/SEQUOIA_TOOLING.md`.
