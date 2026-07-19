---
name: project-coverage-report-generation
description: How to generate the committed lcov coverage report (WSL, linux-pkg-gcc-coverage workflow, sequoia script), warning taxonomy, and the 2026-07-18 fixes
metadata:
  type: project
---

The committed coverage report (`coverage_reports/`, updated via "Update coverage report" commits) is generated on **WSL Ubuntu**, where a clone lives at `~/HelloRenderingEngine`:

1. `cd ~/HelloRenderingEngine/TestAll && cmake --workflow linux-pkg-gcc-coverage` — configure + build + run tests.
2. From the repo root: `dependencies/sequoia/scripts/generate_coverage_report.sh build/TestAll/linux-pkg-gcc-coverage`

The script zeroes lcov counters, runs ctest, captures with `lcov` (pinned `--gcov-tool /usr/bin/gcov-15`, matching g++-15), strips `/usr/*`, runs `genhtml --demangle-cpp`. Output dir = `coverage_reports/<path-after-build/>/<first line of Setup.txt>` → `.../Linux_llvmpipe_OpenGL_4_5_Debug/`.

**Fixes applied 2026-07-18** (report regenerated with them was committed; the sequoia-side changes remain uncommitted in the WSL clone's `dependencies/sequoia` submodule, pending upstream to the sequoia repo):
- `-fprofile-update=atomic` added to `CXX_COVERAGE_FLAGS` in `build_system/CMakePresetsCommon.json` (base-coverage preset). Cured an `lcov ERROR: (negative)` hit count and run-to-run hit jitter — gcov's default non-atomic counters were torn by threaded instrumented code (TBB inside sequoia; tests already run `--serial` under coverage via `sequoia_add_coverage_options`, so test-level concurrency wasn't the culprit).
- Dropped `-T Coverage` from the ctest line — CTest's own coverage pass printed a misleading "100% of 87 LOC" and lcov never reads its output.
- `--ignore-errors inconsistent,inconsistent` (doubled = suppress display) on the `lcov --remove` line; genhtml keeps the warnings loud, so each appears once, not twice.

**Expected residual warnings (structural gcov artifacts, persist on fresh builds — don't chase):** 8 genhtml `inconsistent` warnings, all lambdas: (a) "hit but no contained lines hit" = always-throwing `check_exception_thrown` lambdas (exceptional edge not instrumented); (b) "not hit but line is hit" = never-invoked lambdas defined on executed lines. Sole run-to-run diff besides Test-Date timestamps: `sequoia TestFramework/Summary.cpp` duration-formatting branch counts (ms-vs-µs thresholds depend on wall-clock test times) — honest nondeterminism.

**Fresh start is NOT needed routinely**: `--zerocounters` wipes all `.gcda`, and workflow-before-script guarantees build freshness. Delete the build dir only after a gcc apt upgrade (`.gcno` version mismatch). Historic "inconsistencies fixed by nuking the build dir" were almost certainly that.

Figures as of 2026-07-18 (start_lecture_51, post-fix, stable across runs): **48.9% lines (5446/11141), 82.5% functions (6035/7316), 194 source files** — includes dependencies (sequoia, stb_image) since only `/usr/*` is filtered.

## Planned: clang/Mac coverage + Linux∪Mac union (assessed 2026-07-19, not started)

Goal: a Mac (Homebrew LLVM, OpenGL 4.1) coverage report whose union with the Linux one empirically verifies the platform-gated false positives (`Errors.cpp` glGetError path + libcpp_workaround, object-labels-unavailable branches, curlew OS branches — category 1 of [[project-coverage-analysis-2026-07]]) and isolates the true hardware-only residue.

**Presets (direct adaptation):** add a `macos-homebrew-llvm-coverage` configure/build/workflow trio to sequoia's `CMakePresetsCommon.json`, inheriting `macos-homebrew-llvm` + `base-coverage`. Clang supports both `-coverage` and `-fprofile-update=atomic` (since ~clang 12), so `CXX_COVERAGE_FLAGS` works verbatim; `sequoia_add_coverage_options` and the `--serial` test registration are compiler-agnostic. Clang's `--coverage` emits `.gcno`/`.gcda` like gcc. Output dir is automatic via `Setup.txt` → `coverage_reports/TestAll/macos-homebrew-llvm-coverage/Apple_<renderer>_OpenGL_4_1_Debug/`.

**Script (`generate_coverage_report.sh`) — parameterize, don't fork:**
- The one gcc-specific hinge: `--gcov-tool /usr/bin/gcov-15` must become `llvm-cov gcov` on Mac (Homebrew's, `$(brew --prefix llvm)/bin/llvm-cov`, version-matched to the compiler). It's a two-word invocation: lcov 2.x is tolerant, but a two-line wrapper (`exec llvm-cov gcov "$@"`) is belt-and-braces. Select per `uname` (or an optional script argument).
- Extend the `lcov --remove` patterns on Mac: `'/opt/homebrew/*'` (Homebrew libc++), `'/Library/Developer/*'`, `'/Applications/Xcode.app/*'` — else libc++ internals flood the report.
- Archive `coverage.info` next to the HTML output (it currently dies with the build dir) so unions can be computed on either machine without regenerating.
- `brew install lcov` (may be newer than Ubuntu's 2.0 — same flag syntax, possibly different default warning behavior).

**Union step (new, small):** normalize `SF:` path prefixes across machines (`/home/ojrosten/…` vs `/Users/…`) via lcov 2.x `--substitute` or sed, then `lcov -a linux.info -a mac.info -o union.info` and genhtml. Expect minor gcc/llvm-cov attribution seams (a few lines counted by only one tool for structural reasons) — fine for the purpose.

**First Mac run:** budget a warning-triage pass — llvm-cov's gcov emulation attributes lines/exceptional edges differently, so the `inconsistent`-artifact profile will differ from Linux's (taxonomy in [[project-coverage-analysis-2026-07]] transfers).

**Phase 2 option (only if gcov-mode quality disappoints):** clang source-based coverage — `-fprofile-instr-generate -fcoverage-mapping`, run with `LLVM_PROFILE_FILE`, `llvm-profdata merge`, `llvm-cov export -format=lcov` (output still genhtml/union-compatible). More precise (regions, real branch coverage, per-instantiation), but a bigger pipeline delta.

All pieces land in sequoia (presets + script) — natural to combine with upstreaming the 2026-07-18 pipeline fixes above.

**MSVC assessment (2026-07-19, informational — no action decided):** MSVC has no `--coverage`-style instrumentation. Options: **OpenCppCoverage** (free, debugger/PDB-based, zero build changes, HTML + Cobertura out; line-only — no FN/FNDA — so a weak union member); VS Enterprise dynamic coverage (license-gated); **clang-cl + source-based coverage** (`llvm-cov export -format=lcov` — cleanest pipeline fit but a new toolchain, not MSVC); commercial instrumenters (out of scope). Key ROI insight: the Windows box is the **Intel Arc** machine — the only environment that can ever execute the `CapabilitiesConfiguration.cpp` Intel Arc compensation and Intel `message_id{2}` branches (the "permanent hardware-gated residue"). Pragmatic play if desired: a one-off OpenCppCoverage run scoped to confirming those branches execute, rather than full pipeline integration; Linux∪Mac stays the systematic instrument. **Superseded in part (same day):** a `windows-llvm-coverage` leg via the planned Windows clang column ([[project-windows-clang-toolchain]], Route B) would bring the Intel Arc branches into the union in proper lcov format — OpenCppCoverage remains relevant only if measuring *cl-compiled* code specifically matters.

Related: [[avocet-sequoia-usage]] (committed output as outer test layer), [[feedback-testing-vs-dependencies-tradeoff]].
