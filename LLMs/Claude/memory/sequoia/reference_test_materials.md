---
name: sequoia-test-materials
description: Sequoia test-materials machinery — TestMaterials layout, WorkingCopy/Prediction/Auxiliary trichotomy, .seqpat regex preprocessing, update-materials/soft_update mechanics
metadata:
  type: reference
---

How sequoia handles version-controlled test materials (reviewed in depth 2026-07-20; code refs are `dependencies/sequoia/Source/sequoia/TestFramework/`). Runner context: [[sequoia-test-framework-core]]; checking machinery: [[sequoia-checks-semantics]]; avocet's (partial) usage: [[avocet-test-materials-usage]]. Narrative doc: `TestFrameworkPhilosophy.dox` §Malleability.

**Location rule**: test source `Tests/Foo/Bar.cpp` → materials at `TestMaterials/Foo/Bar` (`individual_materials_paths`, `IndividualTestPaths.cpp:98` — rebases the source path off the tests repo).

**Two modes, switched solely by whether a `Prediction` subdirectory exists** (`IndividualTestPaths.cpp:112-141`):
- *Flat / consumption-only*: no `Prediction` — the whole directory is the working material (read-only inputs for the test).
- *Trichotomous*: `Prediction` present → siblings `WorkingCopy` (scratch; created empty on the fly if absent — normal for generate-then-compare tests) and `Auxiliary` (setup material never compared) become meaningful. Only `Prediction` is required.

**Staging** (`set_materials`, `TestRunner.cpp:275`): before each run, `output/TestsTemporaryData/<rel-path>` is wiped and `WorkingCopy` + `Auxiliary` (or the whole flat dir) are copied in. Tests mutate only the temp copies; predictions are read in place from the repo. Tests sharing one source file share one materials dir — staged once (dedupe keyed on the working path; same key dedupes updates in `test_tracker`).

**In-test API** (`test_base`, `FreeTestCore.hpp:72-88`): `working_materials()` / `predictive_materials()` / `auxiliary_materials()`. In flat mode the latter two return `""`. Canonical idiom:
`check(equivalence, "", working_materials() /= "Foo.txt", predictive_materials() /= "Foo.txt");`

**Comparison** (`value_tester<std::filesystem::path>`, `ConcreteTypeCheckers.hpp:407`):
- Path *type* checked first; directories compared by sorted entry lists (count, first mismatch by relative path, then per-entry recursion); files by content with first-difference-at-line-N diagnostics.
- `equivalence` also matches the final path token at every level; `weak_equivalence` ignores names — so a whole-tree WorkingCopy-vs-Prediction comparison **must** use `weak_equivalence` (final tokens differ); same-named file/subtree checks use plain `equivalence`.
- Directory iteration excludes `.DS_Store`, `.keep` (git placeholder for empty dirs), and `*.seqpat`.
- File comparison is pluggable: `general_file_checker<DefaultComparer, Comparers...>` picks a comparer by **file extension** via `object::factory` ([[sequoia-core-utilities]]), defaulting to `string_based_file_comparer`. Example: `PathFreeDiagnostics.cpp:19-31` registers a vacuous comparer for `.ignore`.

**`.seqpat` preprocessing** (`get_reduced_file_content`, `FileEditors.cpp:217`): a prediction file may be accompanied by a same-stem file with extension `.seqpat` holding one ECMAScript regex per line; each is `regex_replace`d to the empty string on **both** working and prediction content before comparison. Strips run-to-run noise (timings `\[.*\]`, absolute paths, etc.). Lives on the prediction side by necessity: the working copy is scrubbed every run, and the prediction file is the only durable artefact 1:1 with the output (user confirmed 2026-07-20). Files whose own extension is `.seqpat` are exempt from reduction. **Gotcha**: a blank line mid-file silently stops processing of subsequent regexes (open question logged in the 2026-07-20 review addendum).

**`update-materials` / `u`** (`update_mode::soft`): only tests with soft failures and both dirs present are enqueued; updates run once after the whole run; suppressed with a warning during instability analysis. `soft_update(working → prediction)` (`MaterialsUpdater.cpp`) is a sorted two-sequence merge: matched files overwritten **only if reduced contents differ** (seqpat-masked noise never dirties VCS diffs); working-only entries copied; prediction-only entries deleted. `copy_special_files_back` first transplants prediction-side `.seqpat`/`.keep` into the working copy so the merge doesn't delete them — a `.seqpat` survives only while a matching-stem output file exists (stale seqpats die with their file). Runs per matched directory pair via recursion, so nested seqpats are preserved.

**Example menu** (sequoia `TestMaterials/`): flat — `PathFreeDiagnostics`; WorkingCopy+Prediction — `Streaming/StreamingFreeTest`; Auxiliary+Prediction (WorkingCopy on the fly) — `TestFramework/TestRunnerTest`; all three — `MaterialsUpdaterFreeTest` (self-referential: uses the materials system to test the updater); flagship — `TestRunnerEndToEndFreeTest` (Auxiliary holds an entire fake project incl. nested TestMaterials; ~20 seqpat-guarded predictions).

Known open findings on this machinery (incl. a past-the-end deref in the merge) live in `LLMs/Claude/reviews/sequoia_review_2026-07-16.md` — check there before re-raising.
