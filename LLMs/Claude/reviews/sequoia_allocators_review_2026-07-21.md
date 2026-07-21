# Sequoia: allocator-aware assignment & allocation testing

**Date:** 2026-07-21
**Scope:** `Source/sequoia/Core/ContainerUtilities/AssignmentUtilities.hpp` and its customers; `Source/sequoia/TestFramework/AllocationTestUtilities.hpp` + the `AllocationCheckers*` family + `Tests/TestFramework/AllocationTesting/*`.
**Method:** two parallel Claude review agents (one per half), findings cross-checked against `sequoia_review_2026-07-16.md` to avoid re-raises; the highest-impact claims re-verified independently by the orchestrating session against the sources (tagged **[verified]** below).
**Status:** user intends to act on many/all items in due course — **AS1 (missing constexpr) explicitly prioritized.** Documentation-flavoured findings (AS2 contract bullets, AS5 assert message/doc, AT2 scope statement, AT5 doc drift, the derivation comments in the shift constants) are inputs to the user's documentation phase, which deliberately comes as final QA before a big release — expected-later work, not oversights.

**Context:** the session that produced this review also pinned down the framework's real-world track record: the libstdc++ half of the std::string story is **GCC PR 116641** (std::string move assignment incorrectly consulted POCCA on the deep-copy fallback; discovered by the user's framework, filed by Jonathan Wakely, fixed Sept 2024). The libc++ half is corroborated in-tree: the disabled `perfectly_stringy_beast` test (`RegularAllocationTestFalsePositiveDiagnostics.cpp:90-107`) is parked on libc++ bug 48439. PR 116641's failing cell (POCCA=1/POCMA=0) is exactly a cell of the 8-combination battery `basic_regular_allocation_test::do_allocation_tests` runs as a matter of course.

**Verdict up front.** Both halves survive adversarial review. `assignment_helper`'s propagation matrix is correct in every branch — including the POCCA≠POCMA cells where libstdc++ itself stumbled — with strong exception guarantee in practice and self-assignment safety by construction; the allocation-testing machinery makes that a *verified* property via counter-identity attribution plus single-choke-point MSVC-debug normalization. The findings live at the edges: dead plumbing (dealloc ledger), dead annotation (constexpr), under-documented contracts (client obligations; rebinding boundary), and one IFNDR declaration.

---

## Part I — `assignment_helper` (AssignmentUtilities.hpp)

### Mechanism (for orientation)

`assignment_helper::assign(to, from, allocGetters...)` compile-time-filters out getters returning `void` (this is how `connectivity_base` passes edge- and node-allocator getters unconditionally), then dispatches on the *filtered* allocators' traits, where each `X_propagation` is `propagate_on_container_X || is_always_equal`:

- all `is_always_equal` → naive: copy-construct, move-assign.
- otherwise `static_assert(consistency<...>())` (all allocators must agree on all three effective propagation values), then `T tmp{from, get_allocator(...)...}` — *from*'s allocator if copy-propagating, *to*'s otherwise — and:
  - `movePropagation || !copyPropagation` → `to = std::move(tmp)`;
  - else (POCCA ∧ ¬POCMA): if ¬POCS, `to.reset(allocGetters(tmp)...)` first installs from's allocator into empty containers via **copy** assignment (propagation may differ between copy and move — hence the doc's insistence), making the subsequent non-propagating `to.swap(tmp)` an equal-allocator swap and therefore well-defined.

Every branch builds `tmp` fully before touching `to`: self-assignment-safe by construction; strong guarantee in practice (see AS6 for the strictest-standardese footnote). SOCCC correctly not consulted (assignment isn't copy construction). The 2026-07-16 review's branch-matrix verification was independently re-confirmed.

**Actual client contract** (only the first two are documented at `:49-58`): befriend the helper; `reset(allocs...)` that *copy*-assigns freshly-allocated empties; **undocumented:** allocator-extended copy ctor `T(const T&, allocs...)` in getter order; member `swap`; move assignment; `reset`/`swap` non-throwing for the strong guarantee.

### Verdict on "robust without too much work"

"Robust": earned — correct matrix, harness-verified across all 8 propagation combinations via `perfectly_sharing_beast`. "Without too much work": half-earned — the documented contract is ~⅓ of the actual one, and the repo's own history narrows the claim: `bucketed_sequence`/`partitioned_sequence_base` (the doc's cited examples) and `node_storage_base` have all migrated to `operator=(const&) = default`, leaving `connectivity_base` the sole production customer. The defensible claim is "robust without too much work *when defaulting is impossible*" — precisely connectivity's situation (copy needs structural fix-up).

### Findings

- **AS1 [medium; verified; user-prioritized] Dead `constexpr` on `assign`/`assign_filtered`.** Every path runs through `impl::invoke_filtered` (`AssignmentUtilities.hpp:26`) → `invoke_with_specified_args` (`Core/Meta/Utilities.hpp:24`), neither `constexpr` — so no specialization of `assign` can be constant-evaluated (strictly IFNDR per [dcl.constexpr]; practically, dead markers). Knock-on: `connectivity_base::operator=(const&)` (`Connectivity.hpp:389`) is `constexpr`, and for *static* graphs both getters filter to void, so the naive branch is constexpr-able in principle — yet a `STATIC_CHECK` copy-assigning a static graph fails to compile today. **Fix: `constexpr` on the two plumbing functions** (`std::tuple`/`std::get` are constexpr-fine), plus a pinning `STATIC_CHECK` copy-assigning a static graph.
- **AS2 [medium-low] Undocumented, unenforced client contract** (see above). A concept or per-branch `requires` clause (`requires requires { to.reset(allocGetters(tmp)...); to.swap(tmp); }`-shaped) would move diagnostics from deep-in-a-trait-dependent-branch to the boundary; at minimum, doc bullets for the missing obligations including the noexcept expectations on `reset`/`swap`. Related silent corner: two getters returning the *same* allocator type, passed in swapped order, compile and construct each container with the other's allocator — in-tree customers are protected only because their allocator types differ.
- **AS3 [low] Constraint-qualifier slips** (same family as A4, 2026-07-16 addendum): `std::invocable<T>` (`:69, :76`) constrains getters against a prvalue but call sites invoke with const lvalues (`:142`) — `operator()(T&&)` or `operator() &&` is admitted-then-rejected; `invocable<AllocGetter, const T&>` matches the call sites. And the traits feed `std::invoke_result_t<AllocGetter, T>` straight into `std::allocator_traits` (`:112` etc.) — a getter returning `const allocator&` hard-errors far from the cause; `std::remove_cvref_t` fixes it (`get_allocator`'s `auto` return already decays).
- **AS4 [low] `copy_propagation<T, FilteredAllocGetters...>()` looks pack-aggregating but ignores the trailing pack** (`:150-172`) — only the first getter is consulted. Sound solely because the `consistency` static_assert at `:86` precedes it; nothing states the dependency. A fold (keeping the assert for the message) or a one-line comment at `:89` localizes the invariant.
- **AS5 [info] The `consistency` static_assert has no message** (`:86`) and the mixed-traits restriction is undocumented — a client hitting it gets no hint that the fix is "make your allocators' traits agree (or handle the containers separately)".
- **AS6 [info] Strictest-standardese exception-safety footnote:** in the `!copyPropagation` branch, non-POCMA move assignment with runtime-equal allocators isn't *guaranteed* non-throwing by the letter of the standard (potentially-throwing noexcept-spec; elementwise move permitted), though every mainstream implementation steals the buffer. Actionable only if that reading matters — the reset-then-swap strategy could serve this branch too.
- **AS7 [question] Public `reset` on `bucketed_sequence`/`partitioned_sequence_base`** — methods whose sole purpose is serving this protocol ("empty me and adopt this allocator; requires POCCA"), publicly visible and readable as "clear". Intended visibility? A doc comment tying them to `assignment_helper` would prevent misreading either way.

**Customer usage: clean.** `connectivity_base` is a model client — friend at `Connectivity.hpp:185`, void-getter trick `:393-409`, `reset` requires-clause `:417-419` exactly matching the one branch that calls it, `swap` `:425`; storage-level `reset`s (`PartitionedData.hpp:434-435, 648-651`) correctly copy-assigning and noexcept.

### Nits (AssignmentUtilities.hpp; all verified)

- `:13` "wraps one or containers" — missing "more"; `:14` "copy assigment", "in situations were"; `:47` "To utilize the this helper".
- `:70` `[[maybe_unused]]` on `assign`'s getter pack is vestigial — used unconditionally at `:72` (unlike `assign_filtered`'s, which genuinely needs it).
- `:88` `T tmp{from, ...}` — brace-init in generic code misroutes for a `T` with a greedy `initializer_list` ctor; parens are the defensive idiom (no current customer affected).
- `impl::type_to_type` (`:31-39`) is opaque for "getter type → allocator type".
- Doc block still cites `bucketed_sequence`/`partitioned_sequence_base` as usage examples (`:54-56`) though both now default `operator=` — doc drift, already noted 2026-07-16, still present.

---

## Part II — Allocation-testing machinery

### Mechanism (for orientation)

- **`shared_counting_allocator<T, PropagateCopy, PropagateMove, PropagateSwap>`** (`AllocationTestUtilities.hpp:39-113`): two `shared_ptr<int>` counters (allocs/deallocs); copies **share** counters; `operator==` is defaulted, i.e. counter *identity*; `is_always_equal = false_type`; the three bools feed POCCA/POCMA/POCS directly, so one template spans the whole matrix. **No converting ctor from `shared_counting_allocator<U,...>`** — deliberate, to stop multi-allocator types cross-contaminating ledgers. Counter identity is the load-bearing decision: SOCCC copies, propagated allocators, and surreptitious by-value allocator copies all land on the right ledger automatically.
- **Predictions**: `alloc_prediction<Event>` stores the platform-neutral count plus a platform-shifted value; consteval UDLs (`1_c`, `1_mu`, `1_anp`, ...) make call sites terse and per-event type-safe. `allocation_info<T, Getter>` bundles getter + predictions, shifted **once** at construction; the scoped-allocator specialization stores per-level inner predictions unshifted and `unpack<I>()` shifts inside per-level construction (no double shift).
- **Measurement**: `allocation_checker` (one container, one prior count) and `dual_allocation_checker` (snapshots both containers' counts *and* whether their allocators share a counter, immediately before an interacting operation). Core comparison normalizes the measured delta back into unshifted space. Propagation handling = choosing which prior snapshot to subtract (POCCA copy-assign: both sides against y's count, y as converted spectator; move-assign: `m_AllocatorsEqual || POCMA` selects y's count; POCS: prior counts swapped before the mutation check; the `(y, x)` inversion in `post_swap_action` correctly compensates for counter exchange — traced, subtle-but-right).
- **Orchestration**: allocation `check_semantics` bolts `allocation_actions` onto the generic Actions pipeline; each `post_*_action` re-snapshots immediately before its operation; comparison/serialization/swap checked as zero-allocation events; `basic_regular_allocation_test::do_allocation_tests` (`RegularAllocationTestCore.hpp:114-125`) instantiates all 8 POCCA×POCMA×POCS combinations.

### MSVC-debug accommodation (assessed: principled architecture, empirical constants)

MSVC with `_ITERATOR_DEBUG_LEVEL != 0` allocates a `_Container_proxy` per container through the allocator rebound to the proxy type. Three cooperating pieces:

1. **Visibility** (`AllocationTestUtilities.hpp:91-112`): under `_MSC_VER` + IDL>0, an explicit conversion operator to `shared_counting_allocator<U, ...>` lets the rebound proxy allocator compile **and share the counters** — proxy allocations are counted, not hidden. (The in-code comment says "ensures successful compilation"; the counter-sharing is load-bearing and deserves the comment.)
2. **Gate** (`AllocationCheckers.hpp:183-196`): `if constexpr (with_msvc_v && (iterator_debug_level() > 0))` — see AT3/AT4.
3. **Shift model** (`AllocationCheckers.hpp:216-331`): `alloc_prediction_shifter<allocation_equivalence_classes::container_of_values<Alloc>>` derives deltas from **container topology** (`container_counts` + `top_level`) — the correction unit is exactly what proxy allocations scale with. `container_of_pointers` overrides assignment shifts with propagation-dependent constants (`!copyProp ? 1 : moveProp ? 2 : swapProp ? 1 : 3`, `:310-324`) encoding `assignment_helper`'s temporary-container strategy — the two halves of this review are coupled, and the class says so. Escape hatch: `allocation_count_shifter<T>` (`:198-207`) for per-type count adjustment (used by the FN suite's pathological types).

Brittle residue, confined: the `1/2/3` constants and test-side `count + 2` patches are observed-behaviour transcriptions with no derivation recorded — one-line comments each would convert magic to model. Known wart not re-raised: `increment_msvc_debug_count` replaces rather than composes user deltas (2026-07-16). Debuggability note: failure messages report the *normalized* value, which on MSVC debug won't match raw counter arithmetic done by hand.

### Findings

- **AT1 [highest-value question; verified] The dealloc ledger is dead plumbing.** `deallocs()` (`AllocationTestUtilities.hpp:84`) has zero callers across Source and Tests. The framework counts allocation *events* only: a leaking copy ctor, a deallocation-skipping destructor, or a shrink-only mutation (`0_mu`) all pass. The maker-based `check_semantics` overloads own their objects' lifetimes, so an end-of-scope `allocs() == deallocs()` balance check is structurally feasible. Deliberately out of scope (ASan's job — then the accessor wants a comment) or the highest-value extension available?
- **AT2 [medium] Rebinding (node-based) containers are outside the envelope, implicitly.** The deliberately-absent converting ctor means `std::list`/`map`/`set`/unordered can't satisfy rebind-construction; the MSVC conversion operator is a proxy-only carve-out (and absent in release). All in-tree customers are contiguous (`vector`/`basic_string`/vector-backed partitioned data). Fidelity claim is real but scoped; a user trying `std::map` gets a compile error deep in container internals, platform-dependently placed. Wants an explicit scope statement on the class doc; *supporting* rebinding via an opt-in tagged converting ctor (preserving the cross-contamination protection) is a design decision.
- **AT3 [medium-low; verified] Off-MSVC `iterator_debug_level()` declared, never defined — IFNDR.** `Source/sequoia/PlatformSpecific/Preprocessor.hpp:41` declares it in the `#else` branch; no definition anywhere; odr-used in eight `if constexpr` guards (AllocationCheckers.hpp:187 + seven test-side sites). Links today only because no call is emitted; any use outside a short-circuited constexpr guard becomes a bewildering link error. Either `constexpr int iterator_debug_level() noexcept { return 0; }` (observably identical) or a comment declaring the intentional tripwire. (Same IFNDR class as the 2026-07-16 Indent.hpp finding; distinct site.)
- **AT4 [watch-item] `with_msvc_v` names a compiler test but means "MSVC STL".** `_MSC_VER` tracks the MS-compatible environment (clang-cl defines it and takes the `#if defined(_MSC_VER)` branch in Preprocessor.hpp, so the accommodation *works* there); the axes could split on the planned Windows+clang Route B (Ninja + GNU driver — no `_MSC_VER`, but also no MSVC STL by default, so co-travel likely persists). Risk is the name being used elsewhere as a genuine compiler test. Cross-references F1 (2026-07-21 type-name addendum) and `memory/sequoia/project_windows_clang_toolchain.md`.
- **AT5 [low; verified] Swap coverage thinner than advertised.** `AllocationCheckersCore.hpp:20-21` lists swap with/without propagation among predicted events, but no swap prediction type exists; swap is only a must-be-zero check, gated on **POCMA ∧ POCS** (`allocation_actions::do_check_swap`, `AllocationCheckersDetails.hpp:606-610`). (i) Doc drift; (ii) why POCMA in the conjunction? A POCS-true/POCMA-false allocator whose container swap allocated goes unchecked; if the gate encodes an MSVC proxy-churn constraint it deserves the comment; no FN test pins it.
- **AT6 [low] `allocation_info_base::count` is `noexcept` yet invokes the user getter** (`AllocationCheckers.hpp:381-385`) — a throwing getter is `std::terminate` instead of the framework's failure channel; sibling `allocator()` isn't `noexcept` (inconsistent pair). Low practical risk (real getters are one-liners).
- **AT7 [low] Asymmetric shifter in `dual_allocation_checker::check_no_allocation`** (`AllocationCheckersDetails.hpp:95-100`): `xCount` passes through `allocation_count_shifter<T>::shift`, `yCount` doesn't. Identity today; a custom shifter for a swap-flavoured event could only correct one side. Deliberate or oversight?

### Structural blind spots (boundary of the technique, not defects)

1. Leaks/dealloc anomalies (AT1). 2. Untracked allocators (global `new`, `make_shared` internals, SBO spill) — only *nominated* allocators are counted. 3. Allocation **size** — counts not bytes; over-reservation undetectable in principle. 4. `construct`/`destroy` churn within a buffer — not intercepted. 5. Library-dependent baselines — predictions are absolute integers with one normalization axis (MSVC IDL); SSO thresholds/growth policies/library bugs platform-fork string-bearing predictions (exactly where the disabled `perfectly_stringy_beast` sits). 6. No FN case for a getter returning the *wrong member's* allocator — today's failure is a confusing count mismatch, not a targeted diagnostic. 7. Nothing enforces or documents that user-supplied `x`/`y` must carry **independent** counters — a `y` copy-constructed from `x` cross-attributes and fails loudly but bafflingly (`m_AllocatorsEqual` is captured but consulted only for move-assign).

### Nits (new; ranked below everything above)

1. `AllocationTestUtilities.hpp` doc typos: "via an counter" (:18), "capactity" (:21), "occured" (:22), "std:allocator" (:30), "propapaged" (:35).
2. `:51` "TO DO: Remove when libc++ is updated" on the nested `rebind` is unactionable as written — `allocator_traits`' default `rebind_alloc` only synthesizes `Alloc<U, Args...>` for trailing *type* parameters; three bool NTTPs make the nested `rebind` permanently required on every conforming library.
3. `AllocationCheckersCore.hpp:46-47` "The implementation is sufficiently for flexible clients" — word order.
4. "propagting" in user-facing advice text (`AllocationCheckersDetails.cpp:20`) — fixing churns versioned FN outputs (which exist for exactly that; 2026-07-16 precedent).
5. "Incorrect assigment outer/inner allocs" (`MoveOnlyScopedAllocationTestDiagnostics.cpp:122,161`) — baked into versioned output.
6. `RegularAllocationCheckersDetails.hpp:66` `std::optional<T>{u}` copies where a move would do — harmless (post-check) but performs uncounted allocations between checked phases, the very slack the framework polices.
7. `MoveOnlyAllocationCheckersDetails.hpp:86,151` double space in `&&  (sizeof...`.
8. `allocation_info` deduction guides (`AllocationCheckers.hpp:470-476,563-571`) require a unique non-template `operator()` — CTAD fails for generic lambdas/function pointers; worth a doc line if public-facing.
9. `allocate` (`AllocationTestUtilities.hpp:65-72`): `n * sizeof(T)` unchecked; `allocate(0)` does a real `::operator new(0)` yet is deliberately uncounted (dealloc side symmetric) — the zero-size normalization deserves its one-line comment.

---

---

## Part III — Overall assessment (same session, user-requested; UDLs read first-hand)

**The core bet:** allocation behaviour as a checkable semantic contract — exact per-event counts, propagation-trait-dependent, stated up front. Everything good and everything limiting follows from it.

**Defended strengths, in order:** (1) counter identity as the attribution mechanism — automatic correct attribution under SOCCC/propagation/surreptitious allocator copies, better than the state of the art; (2) the 8-combination propagation matrix as routine — empirically justified by confirmed bugs in two standard libraries; (3) the prediction-shifting architecture — platform-neutral predictions, derived corrections, equivalence-class tags doubling as documentation (only the constants inside are empirical); (4) the FN suite tests the framework's *teeth* — versioned diagnostics pin that bad code fails informatively, including the negative-count propagation-mismatch signature with dedicated advice.

**UDL verdict:** not a nicety — the enabler of the design's usability. Call sites carry up to eight predictions; `copy_prediction{1}`-style spelling would drown the scenario table, bare ints would be type-unsafe and meaning-free. Quietly excellent details: `consteval` (malformed predictions can't reach runtime); distinct `alloc_prediction<Event>` types make event transposition a compile error; derivation ctors keep the common case at three tokens. Residual weaknesses, mild:

- **U1** Type safety covers the *event* axis, not the *subject* axis — two `1_c` tokens in one aggregate are copies of different objects, distinguished only by position in nested braces. No cheap fix that preserves the tabular terseness; one doc example dissecting a full aggregate token-by-token would pay for itself (documentation phase).
- **U2** `move_prediction` alone has no literal — presumably because a well-behaved move allocates nothing and the default `{}` is the point; the convention deserves one doc sentence.
- **U3** The `static_cast<int>(n)` in each literal wraps silently for absurd inputs (well-defined since C++20, so consteval doesn't reject); a range guard in the consteval body makes overflow a compile error for free. Suffix vocabulary is uneven (`_c/_mu/_pc/_pm` guessable; `_awp/_anp/_ma/_manp` need the legend) — fine for the actual audience.

**Weighed against:** AT1 (half the ledger dead — most surprising boundary for a framework whose brand is exactness); AT2 + counts-not-bytes + nominated-allocators-only (envelope learned by collision, not statement — documentation-phase work); absolute predictions as both the sensitivity and the fragility (stdlib-bug scalps on one side, the disabled stringy test on the other; MSVC-IDL got a principled shifting axis, other baseline axes have none — a `container_of_strings`-style equivalence class is the missing piece if string-bearing types become common customers); plus the AT5/AT7 edge roughness.

**Net:** the most serious allocation-testing apparatus known to me in open C++ — the only one with confirmed stdlib-bug scalps. Weaknesses are around the mechanism, not in it. The one item worth acting on *before* the documentation phase is AT1, because "do we check deallocation balance?" is a design question whose answer might add machinery, not just words.

## R1 — Re-enable the disabled `perfectly_stringy_beast` test (user-flagged 2026-07-21)

`RegularAllocationTestFalsePositiveDiagnostics.cpp:90-107`, disabled pending "this libc++ bug ... 48439". Upstream status established this session:

- **libstdc++ half:** PR 116641 **fixed** (Sept 2024, `_M_assign` instead of `assign` in move-assignment's deep-copy fallback; backported across the 12–15 release branches).
- **libc++ half:** bugzilla 48439 = **github llvm/llvm-project #47783** — reported by the user; **still open upstream as of 2026-07-21**. Same bug shape as PR 116641 (move assignment delegating to copy-assign logic under POCCA=1/POCMA=0; both libraries made the same LWG-2579-adjacent mistake).

**Source-level check (same session): the libc++ bug is still present in llvm-project main as of 2026-07-21.** In current `libcxx/include/string`: `__move_assign(__str, false_type)` with unequal allocators calls `assign(__str)`; `assign(const basic_string&)` is `return *this = __str;` — the **copy** assignment operator — whose first act is `__copy_assign_alloc(__str)`, propagating per **POCCA**. Exactly the reported bug, never fixed (not even incidentally). The fix shape mirrors libstdc++'s: deep-copy via an allocator-agnostic path (`__assign_external(str.data(), str.size())`-style) instead of `assign(str)`.

Consequences for R1:
- **libstdc++/MSVC legs:** re-enable with confidence — PR 116641 is fixed (verify the installed gcc carries the backport: fixed on trunk Sept 2024, backported across 12–15 release branches).
- **libc++ leg:** the test will still fail for the POCCA=1/POCMA=0 instantiations (the FP suite runs all 8 propagation combos through the same templated body). Options: platform-fork the enablement (on for libstdc++/MSVC, off for libc++ — converts the parked test into an active tripwire that *detects the upstream fix* when it lands), or keep fully disabled. A comment on #47783 with the source-level evidence and the libstdc++ precedent (user is the reporter) may be the highest-leverage move — the sibling bug's fix is a ready-made template.

Watch separately on re-enablement: SSO-threshold/growth-policy prediction forks are a distinct failure mode from the propagation bug. Sequoia-side, handled offline by the user.

## Overlaps with 2026-07-16 acknowledged, not re-raised

Branch-matrix correctness + reset-then-swap (independently re-verified, concurs); SOCCC-in-allocator-extended-copy-ctor question (Tier 3); AssignmentUtilities only-indirectly-tested; delta-replacing `increment_msvc_debug_count`; unused `null_allocation_event::comparison`; by-value tuple-unpacking lambda (`AllocationCheckersDetails.hpp:524`); "Inonsistent para-move"/"xand x Equivalent" typos (still present); over-aligned-`T` static_assert; public two-arg `alloc_prediction` ctor; invisibly-derived para predictions; regular/move-only API asymmetries; 8-way instantiation compile weight.

## Deliberately not flagged

`container_tag : bool` (house style, both values used in production); pathological types in the FN suite (the brokenness is the payload); non-atomic counters (single-threaded per test); the `(y, x)` inversion in `post_swap_action` (traced: correct); `assignment_helper` as struct-of-statics (friendship requires a class type); naive branch's copy-then-move-assign (it *is* the implementation of assignment); customers defaulting `operator=` instead of using the helper (right call; feeds the verdict); the `logic_error` container-tag tripwire (house style).
