---
name: sequoia-allocation-testing-found-pr116641
description: "Sequoia's allocation testing discovered libstdc++ PR 116641 — std::string move assignment wrongly consulted POCCA"
metadata: 
  node_type: memory
  type: reference
  originSessionId: 4063fb5f-9f73-474e-943f-1680bec10281
  modified: 2026-07-21T12:57:45.213Z
---

Sequoia's allocation-testing machinery ([[sequoia-overview]]) discovered a real libstdc++ bug: **GCC PR 116641** (https://gcc.gnu.org/bugzilla/show_bug.cgi?id=116641), "[12/13/14/15 Regression] std::string move assignment incorrectly depends on POCCA". Filed by Jonathan Wakely (2024-09), crediting Oliver Rosten with the discovery; fixed by calling `_M_assign(str)` instead of `assign(str)` in the move-assignment deep-copy fallback.

The bug: on move assignment with non-propagating, unequal allocators, libstdc++'s deep-copy fallback called `assign(str)`, which consults **POCCA** — so POCCA==true/POCMA==false caused a move assignment to propagate the allocator per the *copy* trait. Regression from the LWG 2579 implementation (r10-327).

Why it matters as calibration: the failing combination (POCCA true, POCMA false) was dismissed in the patch discussion as "unlikely/useless" — sequoia tests it anyway, which is exactly how the bug surfaced. Strong evidence for the framework's exhaustive-propagation-matrix approach; the user cites it as the fidelity benchmark for the allocation checkers.

The libc++ half is **the same bug, found independently by the user in both libraries**: libc++ bugzilla 48439 = github **llvm/llvm-project#47783** ("Allocator propagation broken for std::string move assignment", reported by ojrosten) — identical shape to PR 116641 (move assignment delegating to copy-assign logic under POCCA=1/POCMA=0); both libraries made the same LWG-2579-adjacent mistake. #47783 is **still unfixed in libc++ main, confirmed at source level 2026-07-21** (`__move_assign(false_type)` with unequal allocators → `assign(str)` → `*this = str` → `__copy_assign_alloc` → POCCA propagation on a move). The disabled `perfectly_stringy_beast` test (`Tests/TestFramework/RegularAllocationTestFalsePositiveDiagnostics.cpp` ~:90-107) is parked on it; re-enablement is tracked as **R1** in `LLMs/Claude/OPEN_ITEMS.md` — viable on libstdc++/MSVC, still blocked for libc++'s POCCA=1/POCMA=0 combos; the libstdc++ fix (`_M_assign`, no trait check) is a ready-made template for an upstream libc++ patch.
