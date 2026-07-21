# Sequoia tooling programme — consolidated summary

**Last updated:** 2026-07-21 (added §6, sanitizer axis + prune workflow + dependency-analyser contract; earlier same day: §5, CI output-drift detection).
**What this is:** the single document summarizing all sequoia-side *build/test tooling* work discussed to date — everything here lands in the sequoia repo (`build_system/CMakePresetsCommon.json`, `scripts/generate_coverage_report.sh`, or the test framework itself), with only thin hooks on the avocet side. Detail lives in the linked memory files (`memory/sequoia/`); actionable status is tracked in `OPEN_ITEMS.md`. Framework-evolution plans (reflection-based registration, modules/prune, test-creation CLI, per-class materials paths) are deliberately out of scope here — see `memory/sequoia/project_roadmap.md`.

## 1. Coverage-pipeline fixes — done locally, awaiting upstream

Verified working in the WSL clone's `dependencies/sequoia` submodule, not yet committed to the sequoia repo:

- `-fprofile-update=atomic` in the `base-coverage` preset's `CXX_COVERAGE_FLAGS` — cured `lcov ERROR: (negative)` hit counts and run-to-run jitter from torn non-atomic gcov counters under TBB threading.
- `-T Coverage` dropped from the script's ctest invocation (CTest's own coverage pass printed a misleading "100%" figure that lcov never reads).
- `--ignore-errors inconsistent,inconsistent` on the `lcov --remove` line, so genhtml is the single loud reporter of each structural warning.

Detail: `memory/sequoia/project_coverage_tooling.md`.

## 2. clang/Mac coverage + Linux∪Mac union — assessed 2026-07-19, not started

- New `macos-homebrew-llvm-coverage` configure/build/workflow trio inheriting `macos-homebrew-llvm` + `base-coverage` — direct adaptation; clang accepts `-coverage` and `-fprofile-update=atomic` verbatim.
- Parameterize `generate_coverage_report.sh` rather than forking: `--gcov-tool` → `llvm-cov gcov` on Mac (Homebrew's, version-matched); Mac-specific `lcov --remove` patterns (`/opt/homebrew/*`, `/Library/Developer/*`, Xcode paths); archive `coverage.info` beside the HTML so unions can be computed anywhere.
- Union step: normalize `SF:` path prefixes, then `lcov -a linux.info -a mac.info`. Purpose: empirically verify the platform-gated coverage false positives (GL 4.1 vs ≥4.3 error paths, libc++ workaround, object-labels branches) and isolate the true hardware-only residue.
- Phase-2 fallback if gcov-mode quality disappoints: clang source-based coverage (`-fprofile-instr-generate` → `llvm-cov export -format=lcov`, still union-compatible).

Detail: `memory/sequoia/project_coverage_tooling.md`.

## 3. Windows + clang front end preset column — assessed 2026-07-19, not started

- **Route A — VS generator + `"toolset": "ClangCL"`**: two-line smoke-test column ("does clang's front end like our code against the MSVC STL"); VS-bundled clang lags LLVM; subtlety: CMake reports `MSVC==TRUE`, so clang-cl silently inherits `if(MSVC)` branches.
- **Route B — Ninja + standalone LLVM, GNU driver**: the durable fourth toolchain column (Windows analog of the Homebrew-LLVM policy). `base-windows-llvm` inheriting `base-llvm` nearly verbatim, plus `windows-llvm{,-release,-asan,-coverage}` trios mirroring the Mac set.
- **Why Route B pays rent:** the `windows-llvm-coverage` leg (source-based coverage → lcov export) brings the **Intel Arc hardware-gated residue** (`CapabilitiesConfiguration.cpp` Arc compensation, Intel `message_id{2}`) into the coverage union — superseding the earlier OpenCppCoverage one-off idea (now relevant only if measuring *cl-compiled* code specifically matters).
- Decision points: audit `if(MSVC)`/compiler-ID flag keying; the deliberate `cxx_std_23` Windows split (Route B at 26 matches other platforms?); the ASan `clang_rt.asan_dynamic-x86_64.dll`-on-PATH gotcha applies to the standalone-LLVM runtime.
- Recommendation as assessed: A as the cheap front-end check, B as the real column and coverage carrier. Naming nit en route: `base-llvm` inherits `base-unix` — a misnomer once Windows joins (consider e.g. `base-ninja-flags`).

Detail: `memory/sequoia/project_windows_clang_toolchain.md`.

## 4. Clickable test failures in IDEs — assessed 2026-07-19, not started

A sequoia diagnostics emitter with **two dialects covering everything**:

- Opt-in runner flag (e.g. `--diagnostics-format msvc|gnu`) emitting canonical single-line diagnostics per **unexpected** failure only (hooked into the summaries' expected/unexpected classification, so false-negative mode doesn't spray phantom errors). Console-only ⇒ no ripple into versioned outputs.
- **`msvc` dialect** (`path(line): error CODE: msg`) → VS Error List via a run-as-build-step custom target (run the exe directly, not ctest — `N:` prefixes defeat MSBuild's parser). Open empirical question: auxiliary locations as `note:` lines vs grouped `error` lines.
- **`gnu` dialect** (`path:line: error:` + `note:` for the multi-location `report(...)` chain) → Xcode Issue Navigator (script phase), CLion (console auto-hyperlinks work with zero setup; build window via the same custom target), VS Code problem matchers, CI logs.
- Includes the `macos-xcode-homebrew-llvm` carrier preset (`CMAKE_XCODE_ATTRIBUTE_CC/CXX`, index store disabled) since no Xcode column exists — AppleClang configure-vs-build split caveat applies.
- Ruled disproportionate: a real VS Test Adapter; a CLion test-tree plugin. Prototype needing zero sequoia changes: post-build regex wrapper over existing failure output.

Detail: `memory/sequoia/project_ide_clickable_failures.md`.

## 5. CI output-drift detection — designed 2026-07-21, not started

Platform-dependence defects in versioned output (`DiagnosticsOutput`, `TestSummaries`; `Prune` is gitignored) manifest as **git diffs, not test failures** — locally a human referees desired diffs (intended changes, reviewed and committed) from undesired ones (platform leaks); CI has no referee. The desired/undesired distinction is already encoded twice: *temporally* (desired diffs are pre-push; a post-clone CI run should end byte-clean) and *spatially* (the discriminator system declares which files may vary by platform). Classification of post-run dirt:

- **shared (undiscriminated) file changed** → platform-dependence leak → hard fail;
- **discriminated file for the runner's own platform changed** → stale fork the pusher couldn't have regenerated → harvest (artifact upload / bot PR — elevates CI to *fork maintainer* for CI-covered platforms);
- **new discriminated file** → uncovered platform combination → harvest.

Mechanism ladder: `git diff --exit-code -- output/` (blunt baseline) → workflow-side suffix parsing (runner's discriminator from `Setup.txt`) → **sequoia-native drift report** (recommended: runner byte-compares before each versioned write, classifies via its own discriminator knowledge, opt-in CI flag exits nonzero on shared drift). Prerequisite for Windows runners: the E4 binary-write fix, else CRLF rewrites drown the signal; Linux runners usable immediately. Complements the F-strategy parity battery (in-lab, type names, sequoia's suite) as the field-wide net (client repos, everything).

Detail: `memory/sequoia/project_ci_output_drift.md`.

## 6. Sanitizer axis + prune workflow + dependency-analyser contract — designed 2026-07-21, not started

Organizing principle: **each axis lives at the latest pipeline stage it affects** — sanitizers at configure (codegen), prune at the build-target stage (same binary). Only configure-stage axes multiply build trees; the feared combinatorial explosion mostly dissolves under this rule.

- **Sanitizers**: generalize the existing env-slot pattern (`base-unix` already splices `$env{CXX_ASAN_FLAGS}`...) with `CXX_UBSAN_FLAGS`/`CXX_TSAN_FLAGS` slots + per-sanitizer environment-only mixins. One slot per sanitizer is load-bearing: preset multiple inheritance unions disjoint env maps, so mixins compose (`inherits: [toolchain, base-asan-unix, base-ubsan-unix]`). Compatibility matrix kills the space: ASan⊥TSan (compiler-enforced), MSVC = ASan only, UBSan composes with all ⇒ blessed legs are **asan+ubsan** and **tsan(+ubsan)**, Linux/Mac only. UBSan must carry `-fno-sanitize-recover=undefined` (default report-and-continue is silent here — stderr isn't versioned); TSan is already loud (non-zero exit fails the `run` target). Ad-hoc combos via `$env{}` process-environment fallback, no preset needed (caveat: lands in the plain preset's tree). The TSan leg supplies the FN-mode evidence channel the race-amplification harness needs.
- **Prune**: workflows have no test step — tests run via the `run` custom target. A configure-level `EXEC_ARGS=prune` preset is doubly self-defeating (presetName-keyed binaryDir → fresh tree → full rebuild *and* empty ledger, since the prune ledger is per-build-dir). Right shape: a second custom target `prune` beside `run` in `sequoia_set_run_target`, plus per-toolchain build presets + workflows reusing existing configure presets. Composes freely with the sanitizer axis (per-tree ledgers). Cautions: the 2026-07-16 **prune quartet** (silent under-selection) is elevated by blessing this workflow — fix before trusting it; confirm fresh-tree = run-everything; `${EXEC_ARGS}` needs semicolon-separated args (or `separate_arguments()`); `exectuable` typo at `Utilities.cmake:70`.
- **Dependency-analyser contract** (user TODOs): (1) warn when a cpp is not dependent on its associated same-stem hpp — turns the association heuristic into a checked contract (a non-including cpp signals spurious pairing or a layout the analyser can't reason about); (2) document the hitherto-unwritten client precondition: *definitions for declarations in an hpp live in that hpp or the same-stem cpp* (this licenses the furnishing step). Extension families already `{.h,.hpp,.hxx} × {.cpp,.cc,.cxx}` (`DependencyAnalyzer.cpp:77,84`); generalizes under the modules migration (roadmap item 2).

Detail: `memory/sequoia/project_sanitizer_prune_workflows.md`.

## 7. Smaller / adjacent

- **Windows ASan DLL workflow fix** — sequoia build system; handled offline by the user.

## Sequencing

Items 1–3 share files (`CMakePresetsCommon.json`, `generate_coverage_report.sh`): natural order is upstream the pipeline fixes first, then the Mac coverage trio, then the Windows column. Item 4 is independent of the coverage work but shares the upstream vehicle. Item 5's runner-side piece is independent code but its Windows leg sequences after the E4 binary-write fix; its Actions leg belongs to the parked CI-roadmap item. Item 6's sanitizer half shares `CMakePresetsCommon.json` with items 1–3 (same upstream vehicle); its prune half wants the prune quartet fixed first, and the analyser warning naturally rides along with that fix (same code region as Tier-1 #2).
