---
name: project-coverage-analysis-2026-07
description: Conclusions of the 2026-07-18 avocet/curlew coverage analysis — real coverage levels, the five-way false-negative taxonomy, false-positive cautions, actionable shortlist
metadata:
  type: project
---

Full report: `LLMs/Claude/reviews/coverage_analysis_2026-07-18.md` (analyzed `start_lecture_51`, Linux llvmpipe GL 4.5 Debug). Check it before re-raising coverage findings.

**Real levels** (the report's global 48.9% is dominated by dependencies): `Source/avocet` **90.5%** lines, `TestingUtilities/curlew` **83.8%**, Tests+TestAll 96.5%.

**Five-way taxonomy of uncovered avocet/curlew code** — when reading this project's coverage report, classify before flagging:
1. **Platform-gated** (Mac GL 4.1/libc++ run would cover): `Errors.cpp` libcpp_workaround + `to_string(error_code)`/glGetError path (on 4.5 error handling flows via debug messages instead), object-labels-unavailable branches, curlew Win/Apple OS branches. Explains `Errors.cpp`'s 41.1%.
2. **Hardware/driver-gated** (no CI platform covers; irreducible residue): Intel Arc StencilFunc compensation (also the sole instantiator of `gl_function<void(GLuint,GLint,GLuint)>` — hence unhit GLFunction/DecoratedContext aliases), Intel/NVIDIA message-id branches, notification printing, non-llvmpipe renderer rows, Linux 4.1→4.6 retry quirk.
3. **Intentional tripwires** (uncovered by policy): all post-exhaustive-switch throws + `init_debug` inconsistency + null-resource + out-of-range. Fully explains `Formatting.hpp`'s 41.4% functions: `error_message<E>` production instantiations are reachable *only* from tripwires.
4. **Scope**: `get_framebuffer_extent`/`update_viewport` are consumed by `Demo/DemoMain.cpp`, which isn't under the TestAll harness.
5. **gcov attribution artifacts**: zero-count `}`/init-list/return lines inside executing code (exception-cleanup blocks surfaced by `geninfo_unexecuted_blocks=1`); in Tests/, always-throwing lambda bodies.

**False-positive cautions**: line-hit ≠ branch-taken (single-line `if(x) return y;` idiom — branch coverage not collected); lcov 2.x function coverage counts template/lambda **aliases**, distorting both ways (`CapableContext.hpp` 83% deflated by one deliberately-aborting test instantiation; 100%-of-499 claims mean "each instantiation entered once"); fixture traffic (curlew window per test) executes context code without asserting on it; Debug-only view (`has_ndebug()` branches dark).

**Actionable shortlist agreed 2026-07-18** (small, llvmpipe-reachable; user to decide): incomplete-FBO status test (`Framebuffer.cpp` 33.3%); `check_exception_thrown` for reachable validation throws `Image.cpp:38` (>4 channels), `Textures.hpp:86` (alignment >8), `GLFWWrappers.hpp:32` (`num_samples{0}`); round-trip test pinning the 30-entry `to_string(gl_capability)` name table.

Related: [[project-coverage-report-generation]] (how the report is made, pipeline fixes), [[feedback-defensive-throws]], [[feedback-testing-vs-dependencies-tradeoff]].
