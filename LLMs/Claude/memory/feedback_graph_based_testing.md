---
name: graph-based-testing-recipe
description: How to write sequoia graph-based (transition_checker) tests — user wants me fluent in this; construction recipe, CheckFn menu, gotchas, standalone-compile trick
metadata:
  type: feedback
---

The user asked me (2026-07-17) to learn to *create* graph-based tests, not just review them: the tests are highly expressive — states declared once, behaviour as labelled edges — so they compress context for both of us. Verified by a compiled standalone exercise (g++ 15/WSL, `std::vector<int>` graph, deliberate broken edge correctly attributed). See [[sequoia-graph-based-testing-conviction]] for why this technique matters to the user, and the 2026-07-17 addendum in `LLMs/Claude/reviews/sequoia_review_2026-07-16.md` for the design review.

**Why:** graph-based tests are the preferred idiom for state-rich types in both sequoia and avocet; proposing or writing them should be as natural as writing a `check(equality, ...)`.

**How to apply — construction recipe** (`sequoia/TestFramework/StateTransitionUtilities.hpp`, slated for rename to ~graph-based-testing):

```cpp
using checker = transition_checker<T>;          // T totally_ordered => ordering fields become mandatory
checker::transition_graph g{
  { // outer list: node-indexed; inner lists: out-edges of that node
    { edge{targetNode, "description", [](T t){ ...; return t; }, std::weak_ordering::greater} , ... },
    ...
  },
  { T{...}, [](){ return T{...}; }, {"msg", initCheckFn, ctorArgs...} }  // node states: values, generator lambdas, or init-checked construction
};
checker::check(report("..."), g, checkerFn);
```

- **CheckFn menu** (overload selected by lambda signature): `(desc, obtained, prediction)` → `+parent` → `+parent, parentIdx, targetIdx` → `+parent, weak_ordering` → fully lazy `(desc, function<T()>, function<T()>, function<T()> [, weak_ordering])`. Move-only `T` requires the lazy form **and** generator-lambda node weights (the value/`Args...`/init-check ctors all hard-error for move-only — `std::function` needs copyable).
- **Ordering**: for `totally_ordered` T the per-edge `weak_ordering` (obtained vs parent) is compulsory — omitting it is a compile error (`weak_ordering` has no default ctor). Opt out with `transition_checker<T, check_ordering::no>`.
- **Compile errors are horrifically complicated** when a graph shape or CheckFn signature is wrong (acknowledged by the user, on their improvement list): a mismatched CheckFn fails as "no matching overload" against six constrained templates; a mis-shaped aggregate detonates inside `directed_graph`'s initializer machinery. When helping debug one, jump straight to checking the CheckFn signature against the overload menu and the aggregate nesting — don't wade through the instantiation stack.
- **Gotchas**: node generators run more than once per edge — keep them idempotent/pure; a node with no in-edges is never verified (use the init-check node form); failure messages read "Transition from node i to j" + edge description, so put real text in descriptions (avocet's `CapabilityManagerFreeTest` leaves them empty — the enum helps only the author); traversal covers *all* edges even in disconnected graphs.
- **Hidden-state bridge** (GL contexts etc.): make `T` a pure payload value and have each transition first force the context to the parent state, then verify the context separately in the CheckFn — the `CapabilityManagerFreeTest.cpp` pattern.
- **Standalone experiments** (no test runner needed — CheckFn can be a plain function): compile with `g++ -std=c++23 -I dependencies/sequoia/Source file.cpp <SEQ>/sequoia/TextProcessing/Indent.cpp <SEQ>/sequoia/Maths/Graph/GraphErrors.cpp -ltbb`.
