---
name: sequoia-coverage-tooling
description: Sequoia-side coverage tooling — 2026-07-18 pipeline determinism fixes awaiting upstream, planned clang/Mac coverage preset + Linux∪Mac union, script parameterization, MSVC/Windows assessment
metadata:
  type: project
---

Everything here lands in **sequoia** (`build_system/CMakePresetsCommon.json` + `scripts/generate_coverage_report.sh`) — split out 2026-07-20 from [[project-coverage-report-generation]], which keeps the HelloRenderingEngine-side recipe for the committed report. Programme summary: `LLMs/Claude/SEQUOIA_TOOLING.md`.

## Fixes applied 2026-07-18 — verified working, awaiting upstream

Live uncommitted in the WSL clone's `dependencies/sequoia` submodule; belong in the sequoia repo:

- `-fprofile-update=atomic` added to `CXX_COVERAGE_FLAGS` in the `base-coverage` preset. Cured an `lcov ERROR: (negative)` hit count and run-to-run hit jitter — gcov's default non-atomic counters were torn by threaded instrumented code (TBB inside sequoia; tests already run `--serial` under coverage via `sequoia_add_coverage_options`, so test-level concurrency wasn't the culprit).
- Dropped `-T Coverage` from the script's ctest line — CTest's own coverage pass printed a misleading "100% of 87 LOC" and lcov never reads its output.
- `--ignore-errors inconsistent,inconsistent` (doubled = suppress display) on the `lcov --remove` line; genhtml keeps the warnings loud, so each appears once, not twice.

## Planned: clang/Mac coverage + Linux∪Mac union (assessed 2026-07-19, not started)

Goal: a Mac (Homebrew LLVM, OpenGL 4.1) coverage report whose union with the Linux one empirically verifies the platform-gated false positives (`Errors.cpp` glGetError path + libcpp_workaround, object-labels-unavailable branches, curlew OS branches — category 1 of [[project-coverage-analysis-2026-07]]) and isolates the true hardware-only residue.

**Presets (direct adaptation):** add a `macos-homebrew-llvm-coverage` configure/build/workflow trio to `CMakePresetsCommon.json`, inheriting `macos-homebrew-llvm` + `base-coverage`. Clang supports both `-coverage` and `-fprofile-update=atomic` (since ~clang 12), so `CXX_COVERAGE_FLAGS` works verbatim; `sequoia_add_coverage_options` and the `--serial` test registration are compiler-agnostic. Clang's `--coverage` emits `.gcno`/`.gcda` like gcc. Output dir is automatic via `Setup.txt` → `coverage_reports/TestAll/macos-homebrew-llvm-coverage/Apple_<renderer>_OpenGL_4_1_Debug/`.

**Script (`generate_coverage_report.sh`) — parameterize, don't fork:**
- The one gcc-specific hinge: `--gcov-tool /usr/bin/gcov-15` must become `llvm-cov gcov` on Mac (Homebrew's, `$(brew --prefix llvm)/bin/llvm-cov`, version-matched to the compiler). It's a two-word invocation: lcov 2.x is tolerant, but a two-line wrapper (`exec llvm-cov gcov "$@"`) is belt-and-braces. Select per `uname` (or an optional script argument).
- Extend the `lcov --remove` patterns on Mac: `'/opt/homebrew/*'` (Homebrew libc++), `'/Library/Developer/*'`, `'/Applications/Xcode.app/*'` — else libc++ internals flood the report.
- Archive `coverage.info` next to the HTML output (it currently dies with the build dir) so unions can be computed on either machine without regenerating.
- `brew install lcov` (may be newer than Ubuntu's 2.0 — same flag syntax, possibly different default warning behavior).

**Union step (new, small):** normalize `SF:` path prefixes across machines (`/home/ojrosten/…` vs `/Users/…`) via lcov 2.x `--substitute` or sed, then `lcov -a linux.info -a mac.info -o union.info` and genhtml. Expect minor gcc/llvm-cov attribution seams (a few lines counted by only one tool for structural reasons) — fine for the purpose.

**First Mac run:** budget a warning-triage pass — llvm-cov's gcov emulation attributes lines/exceptional edges differently, so the `inconsistent`-artifact profile will differ from Linux's (taxonomy in [[project-coverage-analysis-2026-07]] transfers).

**Phase 2 option (only if gcov-mode quality disappoints):** clang source-based coverage — `-fprofile-instr-generate -fcoverage-mapping`, run with `LLVM_PROFILE_FILE`, `llvm-profdata merge`, `llvm-cov export -format=lcov` (output still genhtml/union-compatible). More precise (regions, real branch coverage, per-instantiation), but a bigger pipeline delta.

Natural to combine with upstreaming the 2026-07-18 fixes above.

## MSVC assessment (2026-07-19, informational — no action decided)

MSVC has no `--coverage`-style instrumentation. Options: **OpenCppCoverage** (free, debugger/PDB-based, zero build changes, HTML + Cobertura out; line-only — no FN/FNDA — so a weak union member); VS Enterprise dynamic coverage (license-gated); **clang-cl + source-based coverage** (`llvm-cov export -format=lcov` — cleanest pipeline fit but a new toolchain, not MSVC); commercial instrumenters (out of scope). Key ROI insight: the Windows box is the **Intel Arc** machine — the only environment that can ever execute the `CapabilitiesConfiguration.cpp` Intel Arc compensation and Intel `message_id{2}` branches (the "permanent hardware-gated residue"). Pragmatic play if desired: a one-off OpenCppCoverage run scoped to confirming those branches execute, rather than full pipeline integration; Linux∪Mac stays the systematic instrument. **Superseded in part (same day):** a `windows-llvm-coverage` leg via the planned Windows clang column ([[sequoia-windows-clang-toolchain]], Route B) would bring the Intel Arc branches into the union in proper lcov format — OpenCppCoverage remains relevant only if measuring *cl-compiled* code specifically matters.

Related: [[project-coverage-report-generation]] (HRE recipe), [[sequoia-ide-clickable-failures]] (same upstream vehicle), [[avocet-sequoia-usage]].
