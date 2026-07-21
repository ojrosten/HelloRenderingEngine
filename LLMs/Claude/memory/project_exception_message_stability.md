---
name: avocet-exception-message-stability
description: "How check_exception_thrown output stays diff-stable in versioned files — throw-site generic-path discipline + sequoia's default root-strip postprocessor + platform-suffixed forks; no custom postprocessor in the repo"
metadata: 
  node_type: memory
  type: project
  originSessionId: f58cc83f-8059-4d66-a2ba-ed07ba4821c1
  modified: 2026-07-21T10:49:01.542Z
---

Surveyed 2026-07-21. Full detail: the 2026-07-21 addendum in `LLMs/Claude/reviews/sequoia_review_2026-07-16.md`; open findings E1–E3 indexed in `LLMs/Claude/OPEN_ITEMS.md` §4.

Exception messages caught by `check_exception_thrown` are laid down in version-controlled files (`output/DiagnosticsOutput/**/*_Exceptions*.txt`), so they must be identical across platforms *and* checkout locations. Sequoia provides a postprocessor hook for this — a constrained template parameter `invocable_r<std::string, project_paths, std::string>` on `check_exception_thrown` (`FreeCheckers.hpp:324, :804`) — but **no call site in this repo supplies a custom one**. Stability comes from three cooperating layers:

1. **Throw sites normalize proactively**: avocet formats every path appearing in a message with `generic_string()` — `avocet::to_string(source_location)` adds `lexically_normal()` to launder MSVC's backslashed `file_name()` (`Source/avocet/Core/Formatting/Formatting.cpp:17`); same discipline at `ShaderProgram.cpp:137,143` and `Image.cpp:33,46`. This upholds an *implicit* contract: sequoia's default postprocessor only matches the forward-slash form of the project root (finding E2 — undocumented as of 2026-07-21).
2. **Sequoia's `default_exception_message_postprocessor`** (`FreeCheckers.hpp:299`) strips the first occurrence of `project_root().generic_string()`, turning absolute generic paths repo-relative — which also buys checkout-location independence, not just cross-platform stability. A custom postprocessor becomes necessary only when a message embeds text avocet doesn't control (e.g. a third-party `.what()` carrying native-separator paths).
3. **Genuinely platform-dependent text** (driver-authored error strings) is not scrubbed but forked: curlew's `output_discriminator()` suffixes the exceptions file per OS/renderer/GL-version — see [[avocet-sequoia-usage]] for the discriminator machinery.

A fourth data point completes the taxonomy (comment at `Errors.cpp:46-52`): when a *defaulted `source_location`* argument differed between platforms (libc++'s missing `std::generator` shifting where `gl_function` was invoked from), the right fix was restructuring the code so the location is platform-stable — neither postprocessing nor forking.
