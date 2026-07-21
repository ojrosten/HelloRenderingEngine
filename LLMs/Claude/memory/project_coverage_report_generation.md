---
name: project-coverage-report-generation
description: How to generate the committed lcov coverage report (WSL, linux-pkg-gcc-coverage workflow, sequoia script) and the expected-warning taxonomy; sequoia-side pipeline tooling split out to sequoia-coverage-tooling
metadata:
  type: project
---

The committed coverage report (`coverage_reports/`, updated via "Update coverage report" commits) is generated on **WSL Ubuntu**, where a clone lives at `~/HelloRenderingEngine`:

1. `cd ~/HelloRenderingEngine/TestAll && cmake --workflow linux-pkg-gcc-coverage` — configure + build + run tests.
2. From the repo root: `dependencies/sequoia/scripts/generate_coverage_report.sh build/TestAll/linux-pkg-gcc-coverage`

The script zeroes lcov counters, runs ctest, captures with `lcov` (pinned `--gcov-tool /usr/bin/gcov-15`, matching g++-15), strips `/usr/*`, runs `genhtml --demangle-cpp`. Output dir = `coverage_reports/<path-after-build/>/<first line of Setup.txt>` → `.../Linux_llvmpipe_OpenGL_4_5_Debug/`.

**Pipeline state:** the 2026-07-18 determinism fixes (atomic profile counters, `-T Coverage` dropped from ctest, lcov `inconsistent` dedup — detail in [[sequoia-coverage-tooling]]) live **uncommitted in the WSL clone's `dependencies/sequoia` submodule**, pending upstream to the sequoia repo. Don't reset/clean that submodule until they land upstream, or the report regresses to nondeterministic counts.

**Expected residual warnings (structural gcov artifacts, persist on fresh builds — don't chase):** 8 genhtml `inconsistent` warnings, all lambdas: (a) "hit but no contained lines hit" = always-throwing `check_exception_thrown` lambdas (exceptional edge not instrumented); (b) "not hit but line is hit" = never-invoked lambdas defined on executed lines. Sole run-to-run diff besides Test-Date timestamps: `sequoia TestFramework/Summary.cpp` duration-formatting branch counts (ms-vs-µs thresholds depend on wall-clock test times) — honest nondeterminism.

**Fresh start is NOT needed routinely**: `--zerocounters` wipes all `.gcda`, and workflow-before-script guarantees build freshness. Delete the build dir only after a gcc apt upgrade (`.gcno` version mismatch). Historic "inconsistencies fixed by nuking the build dir" were almost certainly that.

Figures as of 2026-07-18 (start_lecture_51, post-fix, stable across runs): **48.9% lines (5446/11141), 82.5% functions (6035/7316), 194 source files** — includes dependencies (sequoia, stb_image) since only `/usr/*` is filtered.

**Planned extensions are sequoia-side tooling** — clang/Mac coverage preset + script parameterization, the Linux∪Mac union step, the Windows-llvm coverage leg: see [[sequoia-coverage-tooling]] (and `LLMs/Claude/SEQUOIA_TOOLING.md` for the whole programme). What the union buys *this* repo: empirical verification of the platform-gated false positives and isolation of the hardware-only residue ([[project-coverage-analysis-2026-07]]).

Related: [[avocet-sequoia-usage]] (committed output as outer test layer), [[feedback-testing-vs-dependencies-tradeoff]].
