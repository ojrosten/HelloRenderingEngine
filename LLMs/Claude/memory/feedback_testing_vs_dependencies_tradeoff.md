---
name: feedback-testing-vs-dependencies-tradeoff
description: The user weighs test coverage against dependency hygiene — an anonymous-namespace helper is deliberately left untested rather than hoisted into a header just to reach it.
metadata:
  type: feedback
---

The user's testing philosophy in this codebase: tests assert that exceptions are thrown when expected, and the **exception message is put under version control** (the `*_Exceptions.txt` diagnostics files). That makes "the expected exception, of the expected type, with the expected message, was actually thrown" a powerful regression check.

But that coverage is **not pursued unconditionally**. Making an implementation-detail function testable sometimes means pulling it out of an anonymous namespace and into a header, which leaks an unwanted dependency. When that trade tips the wrong way, the user deliberately leaves the helper untested rather than damage the dependency structure. This tension is itself lecture content.

**Why:** raised over the lecture-48 removal of `unique_image::to_unsigned` (and the `curlew::validate` / `get_current_resource_index` helpers). They threw `std::logic_error` / `std::runtime_error` on negative input but were untested — *because* they sat in anonymous namespaces and making them testable would have generated unwanted dependencies. Lecture 48's generic utilities (`checked_conversion_to`, in a normal header with its own `ArithmeticCastsFreeTest`) give "the best of both worlds": the negative-value check is now tested *once*, generically, and the call sites just reuse it — so the per-site helpers could be deleted, and the exception type unified to `std::domain_error`, without losing coverage.

**How to apply:** Don't reflexively flag an untested anonymous-namespace helper as a coverage gap, nor suggest hoisting it into a header so it can be tested — the user has likely already weighed testability against dependencies and chosen deliberately. The constructive move is the lecture-48 one: if the same un-tested logic recurs at several sites, factor it into a genuinely generic, independently-tested utility, which earns coverage *without* the dependency cost. Relates to [[feedback-tests-are-customers]] (tests as real API customers) and [[user-teaching-project]] (the trade-off is taught, not just applied).
