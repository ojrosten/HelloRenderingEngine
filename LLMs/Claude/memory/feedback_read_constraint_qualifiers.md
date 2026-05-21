---
name: feedback-read-constraint-qualifiers
description: Read concept/constraint qualifiers carefully — `invocable<From>` vs `invocable<From&>` is a meaningful design choice, not a noise difference.
metadata:
  type: feedback
---

When reviewing template constraints in this codebase, treat reference/const qualification inside concepts (e.g. `std::invocable<From>` vs `std::invocable<From&>`) as load-bearing. Don't propose consolidating overloads that differ only in such qualifiers without checking what capability the difference is enabling.

**Why:** I suggested collapsing the two `std::span` overloads of `avocet::to_array` in `Source/avocet/Core/Utilities/ContainerUtilities.hpp` because I thought they "differ only by const-qualification of `From`." They don't. The mutable-span overload's constraint is `std::invocable<From&>` (with `To = std::invoke_result_t<Fn, From&>`), which is what allows a callback to *mutate* source elements in place. The const-span overload uses `std::invocable<From>` over `span<const From, N>` — strictly read-only. Collapsing them would force a choice between the two semantics, losing one. The user pushed back.

**How to apply:** Before suggesting an overload merge or template simplification, identify what each variant's constraint actually permits at the call site. If one constraint reads with `&`, ask "could a caller depend on mutating through that reference?" before assuming the overloads are redundant. Type-level documentation isn't only in the parameter types — concepts are part of the contract too.
