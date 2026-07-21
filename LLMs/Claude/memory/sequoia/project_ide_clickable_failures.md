---
name: sequoia-ide-clickable-failures
description: Assessed plan (2026-07-19) for clickable sequoia test failures in IDEs — VS Error List (MSVC dialect), Xcode Issue Navigator and CLion (GNU dialect), emitter + run-as-build-step; includes the Xcode+Homebrew-clang carrier preset
metadata:
  type: project
---

Assessed 2026-07-19 at the user's request; not started. Goal: sequoia test failures appear in the Visual Studio Error List with clickable links to the failing line(s); a single failure may report multiple locations (origination point + first failing check — the `report(...)` chain).

**Mechanism:** the Error List is populated by MSBuild's output parser, which recognizes the canonical diagnostic shape in any build-step output: `absolute\path\file.cpp(line): error|warning CODE: single-line message`. (This is how doctest/Catch2 do MSVC integration.) Two independent ingredients:

**(a) Sequoia-side emitter** (sequoia owns `std::source_location` per check + the multi-location `report(...)` structure): an opt-in runner flag (e.g. `--diagnostics-format msvc`) emitting, per **unexpected** failure:
- one canonical `error` line at the failing check's location, message flattened to a single line (full detail stays in normal output/summary files);
- auxiliary locations either as cl-idiom `path(line): note: reported from here` (click-navigable in the Output window) or as additional `error` lines sharing a grouping tag — **settle empirically**: hand-write both shapes into a post-build `echo` and observe the Error List's treatment of `note` severity before committing to one.
- **FN-mode nuance:** hook the same expected/unexpected classification the summaries use — in false-negative mode failures are the passing condition; naive emission would spray phantom errors.
- **Versioned-output safety:** console-only + opt-in ⇒ no ripple into committed summaries/diagnostics.

**(b) Run as a build step** so stdout flows through MSBuild's parser: CMake custom target (`TestAllVS`: `COMMAND $<TARGET_FILE:TestAll> --diagnostics-format msvc`) built on demand, or an option-gated post-build event on `TestAll` (2-second suite makes always-on viable). **Run the exe directly, not via ctest** — ctest's `N: ` line prefixes defeat the parser. VS-generator compiles use absolute paths, so `source_location::file_name()` is already fully qualified (same property the Demo path tripwire relies on).

**Prototype route (zero sequoia changes):** post-build PowerShell wrapper regexing sequoia's existing failure output (already prints file/line) into canonical lines — proves the UX in an afternoon and answers the note-vs-error question; the native emitter then replaces it (only the framework can reliably associate one failure's multiple locations).

**Rejected as disproportionate:** a real VS Test Adapter (C# VSIX, Test Explorer integration). **Side benefit of the canonical format:** VS Code problem matchers and MSBuild CI logs parse the same lines — not VS-lock-in.

## Xcode counterpart (added same day)

Xcode's Issue Navigator scrapes **Run Script build phase** output for the **GCC/clang dialect**: `abs/path:line: error|warning|note: message` → clickable issue + inline editor annotation at that line (the SwiftLint mechanism). So the emitter grows a second dialect — `--diagnostics-format gnu` — and multi-location failures use clang's native idiom here: `error:` followed by `note:` lines. Hook: with the Xcode generator, `add_custom_target(TestAllXcode COMMAND $<TARGET_FILE:TestAll> --diagnostics-format gnu)` materializes as a script phase; build that scheme.

**Carrier preset** (no Xcode column exists — Mac presets are Ninja). `CMAKE_CXX_COMPILER` doesn't work with the Xcode generator; the standard mechanism is build-setting attributes:

```json
{
  "name": "macos-xcode-homebrew-llvm",
  "displayName": "Xcode [Homebrew LLVM]",
  "generator": "Xcode",
  "cacheVariables": {
    "CMAKE_XCODE_ATTRIBUTE_CC":  "/opt/homebrew/opt/llvm/bin/clang",
    "CMAKE_XCODE_ATTRIBUTE_CXX": "/opt/homebrew/opt/llvm/bin/clang++",
    "CMAKE_XCODE_ATTRIBUTE_COMPILER_INDEX_STORE_ENABLE": "NO"
  },
  "inherits": ["base-llvm"]
}
```

(Own `generator` field overrides the inherited Ninja; flag composition + clang `WARNING_SUPPRESSIONS` carry over.) Caveats: **(1) configure-vs-build compiler split** — CMake's detection under the Xcode generator still runs Apple clang, so `CMAKE_CXX_COMPILER_ID=AppleClang` while builds use Homebrew's; the code's capability gating is preprocessor-based so adapts correctly, and sequoia's suppressions are preset-fed not detection-fed, but future compiler-ID-keyed CMake logic would see AppleClang. The override is load-bearing: the project historically outruns AppleClang, so an unmodified Xcode preset may not compile at all. **(2)** Xcode's indexing/editor squiggles use Apple clang regardless (hence disabling the index store). **(3)** Xcode is a multi-config generator — `CMAKE_BUILD_TYPE` doesn't apply; build presets select `--config` (same pattern as `msvc`). Heavyweight fallback if attributes misbehave: custom `.xctoolchain` via `TOOLCHAINS`.

## CLion counterpart (added same day) — no third dialect needed, **and no CMake-side additions either**

Unlike Xcode, CLion needs no carrier preset: it consumes `CMakePresets.json` natively (configure presets appear as CMake profiles; `--workflow` presets simply aren't its entry point), invokes whatever `CMAKE_C(XX)_COMPILER` a preset sets under Ninja (no attribute hack), and its clangd follows the real compiler's flags (no Apple-vs-Homebrew editor split-brain). The custom target and `add_test` registration it needs already exist / are shared with the VS plan.

Three tiers, all served by the `gnu` dialect:
- **Zero-effort (once `gnu` emitter exists):** JetBrains consoles auto-hyperlink `path:line` patterns **anywhere in a line** (unlike MSBuild/Xcode's line-anchored parsers) — so failures in any Run-configuration console are clickable with no setup, and ctest's `N: ` prefixes *don't* break it. CLion's native **CTest integration** already gives a coarse pass/fail node for `TestAll` plus that clickable console for free.
- **Build-window tier (the Error-List analog):** the same run-as-build-step custom target — CLion's build output parses GCC-dialect lines into clickable, error-styled entries in the Build tool window.
- **Test-tree UI tier: disproportionate** — CLion's test-runner tree is hardcoded to blessed frameworks (GTest/Catch2/Boost.Test/doctest); admitting sequoia means a Kotlin CLion plugin (JetBrains' TeamCity service-message protocol is the underlying lingua franca — the hook if that ever changes). Same verdict as the VS Test Adapter.

**Summary across IDEs: two dialects cover everything** — `msvc` for the VS Error List; `gnu` for Xcode Issue Navigator, CLion (console + build window), VS Code problem matchers, vim/emacs, CI logs.

Emitter belongs in **sequoia** (same upstream vehicle as the coverage fixes and toolchain plans); the custom targets/post-build hooks and the Xcode carrier preset are the only avocet/build-system-side pieces. Related: [[sequoia-windows-clang-toolchain]], [[sequoia-coverage-tooling]], [[feedback-testing-vs-dependencies-tradeoff]] (FN-mode semantics). Programme summary: `LLMs/Claude/SEQUOIA_TOOLING.md`.
