---
name: project-coverage-analysis-2026-07
description: Conclusions of the 2026-07 avocet/curlew coverage analysis — real coverage levels, false-positive taxonomy, false-negative classes (incl. invisible uninstantiated templates), agreed dispositions
metadata:
  type: project
---

Full report: `LLMs/Claude/reviews/coverage_analysis_2026-07-18.md` (analyzed `start_lecture_51`, Linux llvmpipe GL 4.5 Debug; revised 2026-07-19). Check it before re-raising coverage findings.

**Terminology (user's framing — get this right):** the coverage report is a diagnostic test for gaps. A *positive* = a flagged gap (uncovered code). So **false positive** = flagged but not a real gap; **false negative** = real gap the report doesn't flag — including code it cannot see at all.

**Real levels** (the global 48.9% is dominated by dependencies): `Source/avocet` **90.5%** lines, `TestingUtilities/curlew` **83.8%**, Tests+TestAll 96.5%.

**False-positive taxonomy** — classify before flagging uncovered avocet/curlew code:
1. **Platform-gated** (Mac GL 4.1/libc++ run would cover): `Errors.cpp` libcpp_workaround + `to_string(error_code)`/glGetError path (on 4.5 error handling flows via debug messages instead), object-labels-unavailable branches, curlew Win/Apple OS branches. Explains `Errors.cpp`'s 41.1%.
2. **Hardware/driver-gated** (no CI platform covers; irreducible residue): Intel Arc StencilFunc compensation (also the sole instantiator of `gl_function<void(GLuint,GLint,GLuint)>` — hence unhit GLFunction/DecoratedContext aliases), Intel/NVIDIA message-id branches, notification printing, non-llvmpipe renderer rows, Linux 4.1→4.6 retry quirk.
3. **Intentional tripwires** (uncovered by policy): all post-exhaustive-switch throws + `init_debug` inconsistency + null-resource + out-of-range. Fully explains `Formatting.hpp`'s 41.4% functions: `error_message<E>` production instantiations are reachable *only* from tripwires.
4. **Scope**: `get_framebuffer_extent`/`update_viewport` are consumed by `Demo/DemoMain.cpp`, which isn't under the TestAll harness.
5. **gcov attribution artifacts**: zero-count `}`/init-list/return lines inside executing code (exception-cleanup blocks surfaced by `geninfo_unexecuted_blocks=1`); in Tests/, always-throwing lambda bodies.

**False-negative classes** — real gaps the report hides:
- **Uninstantiated templates are invisible** (not merely uncounted — absent from the denominator). Flagship: `Polygon.hpp` reports 100% yet never instantiated are: the 0-texture and 1-texture ctor overloads (tests only use the span ctor), the 0-texture and single-`texture_unit` `draw` overloads (only span draw used), `operator==` (defaulted friend, never emitted), move ctor/assignment; axes limited to N ∈ {3,4}, dimensionality {2,3} (constraints admit N ≤ 87, dim 4). User: addressable with improved test coverage. Read any template-heavy header's "100%" as "100% of what was instantiated".
- **Systematic sweep 2026-07-19** (method: code-like lines with no `DA:` record at all, plus files with no `SF:` record) found two more: **`GLGetters.hpp`** — five getter families (`get` over `int64_names`/`paired_int_names`/`paired_float_names`/`quadruple_bool_names`/`quadruple_int_names`) never instantiated, name-enums have zero consumers anywhere; **`unique_glad_context` move ctor/assignment** (`ContextBase.hpp:38-45`) — hand-written bodies, never executed. Note: non-trivial *defaulted* members DO emit when odr-used (Polygon's dtor appears), so these absences are meaningful. Also: coroutine bodies (`std::generator` `get_errors`/`get_messages`) lack DA records despite being called (91×/4632×) — attribution, not disuse.
- Line-hit ≠ branch-taken (single-line `if(x) return y;` idiom; branch coverage not collected).
- Alias-counting distortions (lcov 2.x FNL/FNA), fixture traffic (execution ≠ assertion), Debug-only view (`has_ndebug()` branches dark).

**Tests/ analysis (2026-07-19):** 96.5% lines, no genuine gaps. Flagged remainder = always-throwing check_exception_thrown lambdas (dominant; explains `ArithmeticCastsFreeTest` 71.4%), aggregate-init attribution (`CapabilityManagerFreeTest:339-352`), and **failure branches of passing tests** (`UniqueImageThreadingFreeTest:73`, `TestAllMain` catch blocks) — dark is the healthy state for those. The 46 record-less Tests/*.hpp are declaration-only.

**Dispositions (user, 2026-07-19):** incomplete-FBO strings (`Framebuffer.cpp` 33.3%) — *deferred*, bare-minimum FBO will evolve. Reachable validation throws (`Image.cpp:38` >4 channels, `Textures.hpp:86` alignment >8, `GLFWWrappers.hpp:32` `num_samples{0}`) — *worth addressing, pending*. `to_string(gl_capability)` 68.9% — *keep as-is*: the covered subset is a useful signal implicitly tracking how many GL capabilities are actually in use; don't destroy it with a round-trip test. `Polygon.hpp` uninstantiated surface — *addressable via improved tests*. New from the sweep: `unique_glad_context` moves — **resolve by deletion, not testing** (user 2026-07-19: "almost certainly want to delete"; binary-level confirmation of the parked delete-context-hierarchy-moves resolution in [[project-deferred-review-findings]] item 1 — full analysis rewritten there). Distinct from Polygon moves, which are on the resources-keep-their-moves side and resolve by exercising. `GLGetters.hpp` five getter families — groundwork-vs-test decision pending, zero customers today.

Related: [[project-coverage-report-generation]] (how the report is made), [[sequoia-coverage-tooling]] (pipeline fixes, clang/Mac + union plans), [[feedback-defensive-throws]], [[feedback-testing-vs-dependencies-tradeoff]], [[feedback-opengl-source-of-truth-in-tests]].
