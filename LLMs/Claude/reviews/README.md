# Reviews — index

Navigational index only. **Findings are tracked in `../OPEN_ITEMS.md`** (the consolidated, maintained index with its own update contract); this file just says which review document covers what. Reviews are deliberately *not* merged: each pins a reviewed-state date and method, and OPEN_ITEMS, the memory files, and the reviews themselves cross-reference by filename + section/finding-ID — stable identifiers that merging would churn.

| Document | Date(s) | Scope | Finding-ID series |
|---|---|---|---|
| `sequoia_review_2026-07-16.md` | 2026-07-16 + addenda 07-17/07-20/07-21 | Full sequoia strengths/weaknesses/gaps (six agents; tiered correctness findings, design themes, process gaps) | Tiers 1–3 unlettered; addenda: **A** (graph-based testing), **M** (test materials), **T** (test-materials part II), **E** (`check_exception_thrown` postprocessor), **F** (type-name canonicalization) |
| `coverage_analysis_2026-07-18.md` | 2026-07-18 | avocet/curlew coverage report analysis: false-positive taxonomy, false-negative sweep, agreed dispositions | numbered items |
| `sequoia_allocators_review_2026-07-21.md` | 2026-07-21 | `assignment_helper` + allocation-testing machinery (two agents, key claims orchestrator-verified); Part III overall assessment incl. UDLs; PR 116641 / libc++ #47783 record | **AS** (assignment), **AT** (allocation testing), **U** (UDLs), **R** (re-enablement) |
| `sequoia_performance_review_2026-07-21.md` | 2026-07-21 | Performance-testing support: heuristic-as-mathematics, statistical assessment, hypothesis-testing replacement recommendation (paired log-ratios + calibrated group-sequential), coder findings + swap coupling map | **PS** (statistics), **P** (code) |

Conventions: a new *focused* review gets its own dated file plus an OPEN_ITEMS entry; small follow-ups within an existing review's scope go in as dated addenda to that file. Letter prefixes are unique across the folder so bare IDs ("T3", "AS1") are unambiguous.
