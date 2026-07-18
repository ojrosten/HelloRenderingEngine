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

Related: [[avocet-sequoia-usage]] (committed output as outer test layer), [[feedback-testing-vs-dependencies-tradeoff]].
